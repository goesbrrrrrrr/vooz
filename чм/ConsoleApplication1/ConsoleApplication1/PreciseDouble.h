#pragma once
#include <iostream>
#include <cmath>
#include <limits>
#include <iomanip>

class PreciseDouble {
private:
    double value;
    int exponent;
public:
    // Конструкторы
    PreciseDouble(double val = 0.0, int e = 10) : value(val), exponent(e) {}
    PreciseDouble(int val) : value(static_cast<double>(val)), exponent(10) {}

    // Геттеры
    double toDouble() const { return value; }
    double getEPS() const { return std::pow(10.0, -exponent); }
    int getExponent() const { return exponent; }
    void setValue(double val) { value = val; }

    // Арифметические операторы
    PreciseDouble operator+(const PreciseDouble& other) const {
        return PreciseDouble(value + other.value, std::max(exponent, other.exponent));
    }

    PreciseDouble operator-(const PreciseDouble& other) const {
        return PreciseDouble(value - other.value, std::max(exponent, other.exponent));
    }

    PreciseDouble operator*(const PreciseDouble& other) const {
        return PreciseDouble(value * other.value, std::max(exponent, other.exponent));
    }

    PreciseDouble operator/(const PreciseDouble& other) const {
        return PreciseDouble(value / other.value, std::max(exponent, other.exponent));
    }

    // Операторы сравнения с учетом EPS
    bool operator==(const float& other) const {
        return std::fabs(value - other) <= getEPS();
    }

    bool operator==(const PreciseDouble& other) const {
        double eps = std::max(getEPS(), other.getEPS());
        return std::fabs(value - other.value) <= eps;
    }

    bool operator!=(const PreciseDouble& other) const {
        return !(*this == other);
    }

    bool operator<(const PreciseDouble& other) const {
        double eps = std::max(getEPS(), other.getEPS());
        return value < other.value - eps;
    }

    bool operator>(const PreciseDouble& other) const {
        double eps = std::max(getEPS(), other.getEPS());
        return value > other.value + eps;
    }

    bool operator<=(const PreciseDouble& other) const {
        return !(*this > other);
    }

    bool operator>=(const PreciseDouble& other) const {
        return !(*this < other);
    }

    PreciseDouble operator-() const {
        return PreciseDouble(-value, exponent);
    }

    PreciseDouble& operator+=(const PreciseDouble& rhs) {
        value += rhs.value;
        exponent = std::max(exponent, rhs.exponent);
        return *this;
    }

    PreciseDouble& operator-=(const PreciseDouble& rhs) {
        value -= rhs.value;
        exponent = std::max(exponent, rhs.exponent);
        return *this;
    }

    PreciseDouble& operator*=(const PreciseDouble& rhs) {
        value *= rhs.value;
        exponent = std::max(exponent, rhs.exponent);
        return *this;
    }

    PreciseDouble& operator/=(const PreciseDouble& rhs) {
        value /= rhs.value;
        exponent = std::max(exponent, rhs.exponent);
        return *this;
    }

    // Присваивание
    PreciseDouble& operator=(const PreciseDouble& other) {
        value = other.value;
        exponent = other.exponent;
        return *this;
    }

    operator double() {
        return value;
    }

    PreciseDouble& operator=(const float other) {
        value = other;
        return *this;
    }

    // Ввод/вывод
    friend std::ostream& operator<<(std::ostream& os, const PreciseDouble& pd) {
        // Сохраняем текущие флаги форматирования
        std::ios_base::fmtflags old_flags = os.flags();
        std::streamsize old_precision = os.precision();

        // Устанавливаем научный формат с точностью 6 знаков после запятой
        os << std::scientific << std::setprecision(6) << pd.value;

        // Восстанавливаем старые настройки (опционально)
        // os.flags(old_flags);
        // os.precision(old_precision);

        return os;
    }

    friend std::istream& operator>>(std::istream& is, PreciseDouble& pd) {
        is >> pd.value;
        return is;
    }
};