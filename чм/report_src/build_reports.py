# -*- coding: utf-8 -*-
"""
Двухпроходная сборка отчётов.

Проход 1: docx с прочерками вместо номеров -> конвертация в PDF ->
          по PDF определяются реальные страницы каждого заголовка.
Проход 2: docx пересобирается с найденными номерами -> конвертация в PDF.

Номера в содержании и docx, и PDF совпадают с фактической вёрсткой,
потому что берутся из неё самой.
"""
import json
import os
import re
import subprocess
import sys

import fitz
from docx import Document

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(HERE)
SOFFICE = r"C:\Program Files\LibreOffice\program\soffice.exe"

JOBS = [
    ("4", os.path.join(ROOT, "4"), "!чм 4 Юрканов", "make_lab4.py"),
    ("5", os.path.join(ROOT, "5"), "!чм 5 Юрканов", "make_lab5.py"),
]


def unlock(path):
    if os.path.exists(path):
        os.chmod(path, 0o666)


def build_docx(script):
    subprocess.run([sys.executable, os.path.join(HERE, script)],
                   check=True, cwd=HERE,
                   env=dict(os.environ, PYTHONIOENCODING="utf-8"))


def to_pdf(folder, name):
    docx = os.path.join(folder, name + ".docx")
    pdf = os.path.join(folder, name + ".pdf")
    unlock(pdf)
    # Отдельный профиль обязателен: с профилем по умолчанию LibreOffice в
    # headless-режиме падает с 0xC0000409, если он же открыт или ещё не
    # инициализирован после установки.
    profile = os.path.join(HERE, ".loprofile").replace("\\", "/")
    subprocess.run([SOFFICE,
                    "-env:UserInstallation=file:///" + profile,
                    "--headless", "--norestore",
                    "--convert-to", "pdf", "--outdir", folder, docx],
                   check=True, capture_output=True, timeout=600)
    if not os.path.exists(pdf):
        raise RuntimeError("PDF не создан: " + pdf)
    return pdf


def heading_pages(pdf, headings):
    """
    Находит номер страницы для каждого заголовка.

    Искать по одному номеру раздела нельзя: «1.», «2.» и т.д. совпадают с
    пунктами нумерованных списков в тексте, и все разделы верхнего уровня
    получают страницу первого же списка. Поэтому ищем полный текст
    заголовка вместе с номером — такая строка в документе уникальна.
    Пробелы схлопываем, потому что PDF может переносить длинный заголовок.

    Содержание пропускаем: поиск начинается со страницы, следующей за
    последней страницей содержания.
    """
    doc = fitz.open(pdf)
    toc_end = 0
    for i, page in enumerate(doc):
        if "Содержание" in page.get_text():
            toc_end = i
    norm = lambda s: re.sub(r"\s+", " ", s).strip()
    body = [norm(doc[i].get_text()) for i in range(len(doc))]
    pages = {}
    for lvl, text in headings:
        needle = norm(text)
        for i in range(toc_end + 1, len(doc)):
            if needle in body[i]:
                pages[text] = i + 1                 # нумерация с единицы
                break
    doc.close()
    return pages


def headings_of(docx):
    d = Document(docx)
    return [(0 if p.style.name == "Heading 1" else 1, p.text.strip())
            for p in d.paragraphs if p.style.name in ("Heading 1", "Heading 2")]


for tag, folder, name, script in JOBS:
    print("=" * 70)
    print("ЛР%s" % tag)
    jspath = os.path.join(HERE, "pages_%s.json" % tag)

    # --- проход 1: черновая вёрстка, чтобы узнать страницы ---
    if os.path.exists(jspath):
        os.remove(jspath)
    build_docx(script)
    pdf = to_pdf(folder, name)
    heads = headings_of(os.path.join(folder, name + ".docx"))
    pages = heading_pages(pdf, heads)
    missing = [t for _, t in heads if t not in pages]
    print("  проход 1: заголовков %d, найдено страниц %d%s"
          % (len(heads), len(pages),
             ", НЕ НАЙДЕНО: %s" % missing if missing else ""))
    json.dump(pages, open(jspath, "w", encoding="utf-8"), ensure_ascii=False)

    # --- проход 2: с настоящими номерами ---
    build_docx(script)
    pdf = to_pdf(folder, name)

    # --- проверка: номера в содержании совпали с вёрсткой ---
    heads2 = headings_of(os.path.join(folder, name + ".docx"))
    pages2 = heading_pages(pdf, heads2)
    bad = [(t, pages.get(t), pages2.get(t))
           for _, t in heads2 if pages.get(t) != pages2.get(t)]
    print("  проход 2: PDF %d страниц" % fitz.open(pdf).page_count)
    if bad:
        print("  РАСХОЖДЕНИЯ после пересборки:")
        for t, a, b in bad:
            print("     %-55s было %s, стало %s" % (t[:55], a, b))
        json.dump(pages2, open(jspath, "w", encoding="utf-8"), ensure_ascii=False)
        build_docx(script)
        to_pdf(folder, name)
        print("  выполнен третий проход для стабилизации")
    else:
        print("  номера в содержании совпадают с вёрсткой")
