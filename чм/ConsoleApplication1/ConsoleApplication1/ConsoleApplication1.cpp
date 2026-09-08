#include <iostream>
#include <iomanip>
#include <vector>
#include <cstdlib>
#include <ctime>
#include "Matrix.h"
#include "SpecialMatrix.h"
#include "Vector.h"

using namespace std;

void printVector(const Vector& v, const string& name) {
    cout << name << " = [";
    for (int i = 1; i <= v.size(); ++i) {
        cout << v(i);
        if (i < v.size()) cout << ", ";
    }
    cout << "]" << endl;
}

// Функция для генерации системы с контролируемой обусловленностью
    void generateTridiagonalSystem(TridiagonalMatrix& A, Vector& x_exact, Vector& f,
        double range, bool wellConditioned) {
        int n = A.size();
        auto& diag = A.getDiagonal();
        if (wellConditioned) {
            for (int i = 1; i <= n; ++i) {
                diag.main(i) = range * (10.0 + (rand() % 100) / 100.0);
            }
            for (int i = 2; i <= n; ++i) {
                diag.lower(i) = range * (rand() % 100) / 200.0;
            }
            for (int i = 1; i < n; ++i) {
                diag.upper(i) = range * (rand() % 100) / 200.0;
            }
        }
        else {
            for (int i = 1; i <= n; ++i) {
                diag.main(i) = range * ((rand() % 200 - 100) / 100.0);
            }
            for (int i = 2; i <= n; ++i) {
                diag.lower(i) = range * ((rand() % 200 - 100) / 100.0);
            }
            for (int i = 1; i < n; ++i) {
                diag.upper(i) = range * ((rand() % 200 - 100) / 100.0);
            }
        }
        x_exact.randomFill(1.0, 10.0);
        f = A * x_exact;
    }

void testTridiagonalMatrix() {
    cout << "=========================================" << endl;
    cout << "ТЕСТИРОВАНИЕ ОБЫЧНОЙ ТРЁХДИАГОНАЛЬНОЙ МАТРИЦЫ" << endl;
    cout << "Матрица 5x5" << endl;
    cout << "=========================================" << endl;

    const int n = 5;

    // Создаём матрицу
    TridiagonalMatrix A(n);
    auto& diag = A.getDiagonal();

    // Заполняем диагонали
    cout << "\n--- Заполнение матрицы ---" << endl;

    // Главная диагональ: [4, 6, 5, 11, 8]
    diag.main(1) = 4.0;
    diag.main(2) = 6.0;
    diag.main(3) = 5.0;
    diag.main(4) = 11.0;
    diag.main(5) = 8.0;
    cout << "Главная диагональ: [4, 6, 5, 11, 8]" << endl;

    // Поддиагональ: [2, 5, 2, 5] (элементы под главной диагональю)
    diag.lower(2) = 2.0;
    diag.lower(3) = 5.0;
    diag.lower(4) = 2.0;
    diag.lower(5) = 5.0;
    cout << "Поддиагональ: [2, 5, 2, 5]" << endl;

    // Наддиагональ: [2, 1, 2, 2] (элементы над главной диагональю)
    diag.upper(1) = 2.0;
    diag.upper(2) = 1.0;
    diag.upper(3) = 2.0;
    diag.upper(4) = 2.0;
    cout << "Наддиагональ: [2, 1, 2, 2]" << endl;

    // Точное решение из задания: [1, -1, 2, -1, 1]
    Vector x_exact(n);
    x_exact(1) = 1.0;
    x_exact(2) = -1.0;
    x_exact(3) = 2.0;
    x_exact(4) = -1.0;
    x_exact(5) = 1.0;
    cout << "\n--- Точное решение ---" << endl;
    printVector(x_exact, "x_exact");

    // Вычисляем правую часть f = A * x_exact
    cout << "\n--- Вычисление правой части f = A * x_exact ---" << endl;
    Vector f = A * x_exact;
    printVector(f, "f");

    // Решаем систему методом прогонки
    cout << "\n--- Решение системы методом прогонки ---" << endl;
    Vector x_solution = A.solve(f);
    printVector(x_solution, "x_solution");

    // Проверяем невязку
    cout << "\n--- Проверка невязки ---" << endl;
    Vector residual = f - (A * x_solution);
    printVector(residual, "residual");

    double norm_residual = residual.norm();
    cout << "Норма невязки: " << norm_residual << endl;

    // Проверяем погрешность
    cout << "\n--- Проверка погрешности решения ---" << endl;
    Vector error = x_exact - x_solution;
    printVector(error, "error");

    double norm_error = error.norm();
    cout << "Норма погрешности: " << norm_error << endl;

    // Оценка точности
    cout << "\n--- ОЦЕНКА ТОЧНОСТИ ---" << endl;
    cout << fixed << setprecision(10);
    cout << "Норма невязки:       " << std::scientific << std::setprecision(6) << norm_residual << endl;
    cout << "Норма погрешности:   " << std::scientific << std::setprecision(6) << norm_error << endl;

    if (norm_residual < 1e-14 && norm_error < 1e-14) {
        cout << "\n✓ ТЕСТ ПРОЙДЕН: отличная точность" << endl;
    }
    else if (norm_residual < 1e-10 && norm_error < 1e-10) {
        cout << "\n✓ ТЕСТ ПРОЙДЕН: хорошая точность" << endl;
    }
    else {
        cout << "\n✗ ТЕСТ НЕ ПРОЙДЕН" << endl;
    }
}

void testSpecialMatrix() {
    cout << "\n=========================================" << endl;
    cout << "ТЕСТИРОВАНИЕ СПЕЦИАЛЬНОЙ МАТРИЦЫ" << endl;
    cout << "Матрица 9x9, k=5" << endl;
    cout << "=========================================" << endl;

    const int n = 9;
    const int k = 5;

    SpecialMatrix A(n, k);

    auto& diag = A.getDiagonal();
    auto& kVect = A.getKVector();

    cout << "\n--- Заполнение матрицы ---" << endl;

    // Заполняем диагонали
    for (int i = 1; i <= n; ++i) {
        diag.main(i) = 10.0;
    }
    cout << "Главная диагональ: все = 10" << endl;

    for (int i = 2; i <= n; ++i) {
        diag.lower(i) = 1.0;
    }
    cout << "Поддиагональ: все = 1" << endl;

    for (int i = 1; i <= n - 1; ++i) {
        diag.upper(i) = 1.0;
    }
    cout << "Наддиагональ: все = 1" << endl;

    for (int i = 1; i <= kVect.size(); ++i) {
        kVect(i) = 1.0;
    }
    cout << "Дополнительный вектор kVect: все = 1" << endl;
    cout << "Размер kVect = " << kVect.size() << endl;
    cout << "nLeft = " << A.getNLeft() << ", nRight = " << A.getNRight() << endl;

    Vector x_exact(n);
    for (int i = 1; i <= n; ++i) {
        x_exact(i) = 1.0;
    }
    cout << "\n--- Точное решение ---" << endl;
    printVector(x_exact, "x_exact");

    cout << "\n--- Вычисление правой части f = A * x_exact ---" << endl;
    Vector f = A * x_exact;
    printVector(f, "f");

    cout << "\n--- Решение системы методом прогонки ---" << endl;
    Vector x_solution = A.solve(f);
    printVector(x_solution, "x_solution");

    cout << "\n--- Проверка невязки ---" << endl;
    Vector residual = f - (A * x_solution);
    printVector(residual, "residual");

    double norm_residual = residual.norm();
    cout << "Норма невязки: " << norm_residual << endl;

    cout << "\n--- Проверка погрешности решения ---" << endl;
    Vector error = x_exact - x_solution;
    printVector(error, "error");

    double norm_error = error.norm();
    cout << "Норма погрешности: " << norm_error << endl;

    cout << "\n--- ОЦЕНКА ТОЧНОСТИ ---" << endl;
    cout << fixed << setprecision(10);
    cout << "Норма невязки:       " << norm_residual << endl;
    cout << "Норма погрешности:   " << norm_error << endl;

    if (norm_residual < 0.1 && norm_error < 0.1) {
        cout << "\n✓ ТЕСТ ПРОЙДЕН: приемлемая точность" << endl;
    }
    else {
        cout << "\n✗ ТЕСТ НЕ ПРОЙДЕН" << endl;
    }

    // Пошаговый контроль эквивалентности для специальной матрицы
    cout << "\n";
    A.stepwiseEquivalenceControl(x_solution, f);
}

void runTridiagonalExperiment() {
    cout << "\n=========================================" << endl;
    cout << "ВЫЧИСЛИТЕЛЬНЫЙ ЭКСПЕРИМЕНТ (ОБЫЧНАЯ ТРЁХДИАГОНАЛЬНАЯ МАТРИЦА)" << endl;
    cout << "=========================================" << endl;

    vector<int> dimensions = { 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096 };
    vector<double> ranges = { 1e-1, 1e0, 1e1, 1e2, 1e3 };
    const int NUM_TRIALS = 10;

    cout << fixed << setprecision(6);

    // ХОРОШО ОБУСЛОВЛЕННЫЕ
    cout << "\n--- ХОРОШО ОБУСЛОВЛЕННЫЕ МАТРИЦЫ ---" << endl;
    cout << "--------------------------------------------------------" << endl;
    cout << "|      n | Диапазон | Средняя относительная погрешность |" << endl;
    cout << "--------------------------------------------------------" << endl;

    for (int n : dimensions) {
        for (double range : ranges) {
            double avg_error = 0.0;
            int success = 0;

            for (int trial = 0; trial < NUM_TRIALS; ++trial) {
                try {
                    TridiagonalMatrix A(n);
                    Vector x_exact(n);
                    Vector f(n);

                    // Используем функцию генерации для хорошо обусловленной системы
                    generateTridiagonalSystem(A, x_exact, f, range, true);

                    Vector x_sol = A.solve(f);

                    double max_x = 0.0, max_err = 0.0;
                    for (int i = 1; i <= n; ++i) {
                        double xv = abs(x_exact(i).toDouble());
                        double ev = abs((x_exact(i) - x_sol(i)).toDouble());
                        max_x = max(max_x, xv);
                        max_err = max(max_err, ev);
                    }

                    avg_error += (max_x > 1e-15) ? max_err / max_x : max_err;
                    success++;
                }
                catch (...) {
                    continue;
                }
            }

            if (success > 0) {
                avg_error /= success;
                cout << "| " << setw(5) << n << " | "
                    << setw(8) << range << " | "
                    << setw(32) << std::scientific << std::setprecision(6) << avg_error << " |" << endl;
            }
            else {
                cout << "| " << setw(5) << n << " | "
                    << setw(8) << range << " | "
                    << setw(32) << "ошибка" << " |" << endl;
            }
        }
        cout << "--------------------------------------------------------" << endl;
    }

    // ПЛОХО ОБУСЛОВЛЕННЫЕ
    cout << "\n--- ПЛОХО ОБУСЛОВЛЕННЫЕ МАТРИЦЫ ---" << endl;
    cout << "--------------------------------------------------------" << endl;
    cout << "|      n | Диапазон | Средняя относительная погрешность |" << endl;
    cout << "--------------------------------------------------------" << endl;

    for (int n : dimensions) {
        for (double range : ranges) {
            double avg_error = 0.0;
            int success = 0;

            for (int trial = 0; trial < NUM_TRIALS; ++trial) {
                try {
                    TridiagonalMatrix A(n);
                    Vector x_exact(n);
                    Vector f(n);

                    // Используем функцию генерации для плохо обусловленной системы
                    generateTridiagonalSystem(A, x_exact, f, range, false);

                    Vector x_sol = A.solve(f);

                    double max_x = 0.0, max_err = 0.0;
                    for (int i = 1; i <= n; ++i) {
                        double xv = abs(x_exact(i).toDouble());
                        double ev = abs((x_exact(i) - x_sol(i)).toDouble());
                        max_x = max(max_x, xv);
                        max_err = max(max_err, ev);
                    }

                    avg_error += (max_x > 1e-15) ? max_err / max_x : max_err;
                    success++;
                }
                catch (...) {
                    continue;
                }
            }

            if (success > 0) {
                avg_error /= success;
                cout << "| " << setw(5) << n << " | "
                    << setw(8) << range << " | "
                    << setw(32) << std::scientific << std::setprecision(6) << avg_error << " |" << endl;
            }
            else {
                cout << "| " << setw(5) << n << " | "
                    << setw(8) << range << " | "
                    << setw(32) << "ошибка" << " |" << endl;
            }
        }
        cout << "--------------------------------------------------------" << endl;
    }
}

void runSpecialExperiment() {
    cout << "\n=========================================" << endl;
    cout << "ВЫЧИСЛИТЕЛЬНЫЙ ЭКСПЕРИМЕНТ (СПЕЦИАЛЬНАЯ МАТРИЦА)" << endl;
    cout << "=========================================" << endl;

    vector<int> dimensions = { 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096 };
    vector<double> ranges = { 1e-1, 1e0, 1e1, 1e2, 1e3 };
    const int NUM_TRIALS = 10;

    cout << fixed << setprecision(6);

    // ХОРОШО ОБУСЛОВЛЕННЫЕ
    cout << "\n--- ХОРОШО ОБУСЛОВЛЕННЫЕ МАТРИЦЫ ---" << endl;
    cout << "--------------------------------------------------------" << endl;
    cout << "|      n | Диапазон | Средняя относительная погрешность |" << endl;
    cout << "--------------------------------------------------------" << endl;

    for (int n : dimensions) {
        int k = n / 2;
        if (k <= 1) k = 2;
        if (k >= n - 2) k = n - 3;

        if (k <= 1 || k >= n) {
            cout << "| " << setw(5) << n << " | пропущен (нет допустимого k) |" << endl;
            continue;
        }

        for (double range : ranges) {
            double avg_error = 0.0;
            int success = 0;

            for (int trial = 0; trial < NUM_TRIALS; ++trial) {
                try {
                    SpecialMatrix A(n, k);
                    auto& diag = A.getDiagonal();
                    auto& kVect = A.getKVector();

                    // Главная диагональ с преобладанием
                    for (int i = 1; i <= n; ++i) {
                        diag.main(i) = range * (10.0 + (rand() % 100) / 100.0);
                    }

                    // Побочные диагонали - маленькие
                    for (int i = 2; i <= n; ++i) {
                        diag.lower(i) = range * (rand() % 100) / 200.0;
                    }
                    for (int i = 1; i < n; ++i) {
                        diag.upper(i) = range * (rand() % 100) / 200.0;
                    }
                    for (int i = 1; i <= kVect.size(); ++i) {
                        kVect(i) = range * (rand() % 100) / 200.0;
                    }

                    Vector x_exact(n);
                    x_exact.randomFill(1.0, 10.0);

                    Vector f = A * x_exact;
                    Vector x_sol = A.solve(f);

                    double max_x = 0.0, max_err = 0.0;
                    for (int i = 1; i <= n; ++i) {
                        double xv = abs(x_exact(i).toDouble());
                        double ev = abs((x_exact(i) - x_sol(i)).toDouble());
                        max_x = max(max_x, xv);
                        max_err = max(max_err, ev);
                    }

                    avg_error += (max_x > 1e-15) ? max_err / max_x : max_err;
                    success++;
                }
                catch (...) {
                    continue;
                }
            }

            if (success > 0) {
                avg_error /= success;
                cout << "| " << setw(5) << n << " | "
                    << setw(8) << range << " | "
                    << setw(32) << std::scientific << std::setprecision(6) << avg_error << " |" << endl;
            }
            else {
                cout << "| " << setw(5) << n << " | "
                    << setw(8) << range << " | "
                    << setw(32) << "ошибка" << " |" << endl;
            }
        }
        cout << "--------------------------------------------------------" << endl;
    }

    // ПЛОХО ОБУСЛОВЛЕННЫЕ
    cout << "\n--- ПЛОХО ОБУСЛОВЛЕННЫЕ МАТРИЦЫ ---" << endl;
    cout << "--------------------------------------------------------" << endl;
    cout << "|      n | Диапазон | Средняя относительная погрешность |" << endl;
    cout << "--------------------------------------------------------" << endl;

    for (int n : dimensions) {
        int k = n / 2;
        if (k <= 1) k = 2;
        if (k >= n - 2) k = n - 3;

        if (k <= 1 || k >= n) {
            cout << "| " << setw(5) << n << " | пропущен (нет допустимого k) |" << endl;
            continue;
        }

        for (double range : ranges) {
            double avg_error = 0.0;
            int success = 0;

            for (int trial = 0; trial < NUM_TRIALS; ++trial) {
                try {
                    SpecialMatrix A(n, k);
                    auto& diag = A.getDiagonal();
                    auto& kVect = A.getKVector();

                    // Все элементы одного порядка
                    for (int i = 1; i <= n; ++i) {
                        diag.main(i) = range * ((rand() % 200 - 100) / 100.0);
                    }
                    for (int i = 2; i <= n; ++i) {
                        diag.lower(i) = range * ((rand() % 200 - 100) / 100.0);
                    }
                    for (int i = 1; i < n; ++i) {
                        diag.upper(i) = range * ((rand() % 200 - 100) / 100.0);
                    }
                    for (int i = 1; i <= kVect.size(); ++i) {
                        kVect(i) = range * ((rand() % 200 - 100) / 100.0);
                    }

                    Vector x_exact(n);
                    x_exact.randomFill(1.0, 10.0);

                    Vector f = A * x_exact;
                    Vector x_sol = A.solve(f);

                    double max_x = 0.0, max_err = 0.0;
                    for (int i = 1; i <= n; ++i) {
                        double xv = abs(x_exact(i).toDouble());
                        double ev = abs((x_exact(i) - x_sol(i)).toDouble());
                        max_x = max(max_x, xv);
                        max_err = max(max_err, ev);
                    }

                    avg_error += (max_x > 1e-15) ? max_err / max_x : max_err;
                    success++;
                }
                catch (...) {
                    continue;
                }
            }

            if (success > 0) {
                avg_error /= success;
                cout << "| " << setw(5) << n << " | "
                    << setw(8) << range << " | "
                    << setw(32) << std::scientific << std::setprecision(6) << avg_error << " |" << endl;
            }
            else {
                cout << "| " << setw(5) << n << " | "
                    << setw(8) << range << " | "
                    << setw(32) << "ошибка" << " |" << endl;
            }
        }
        cout << "--------------------------------------------------------" << endl;
    }
}

int main() {
    setlocale(LC_ALL, "RUS");
    srand(static_cast<unsigned>(time(nullptr)));

    // ТЕСТЫ
    testTridiagonalMatrix();
    testSpecialMatrix();

    // ЭКСПЕРИМЕНТЫ
    cout << "\n\n";
    cout << "=========================================" << endl;
    cout << "ЗАПУСК ВЫЧИСЛИТЕЛЬНЫХ ЭКСПЕРИМЕНТОВ" << endl;
    cout << "=========================================" << endl;

    runTridiagonalExperiment();
    runSpecialExperiment();

    cout << "\nПрограмма завершена." << endl;
    return 0;
}