#include "Matrix.h"


TridiagonalMatrix::TridiagonalMatrix(int n) : _diag(n), n(n) {}

int TridiagonalMatrix::size() const { return n; }

Vector TridiagonalMatrix::operator*(const Vector& v) {
    Vector result(n);
    for (int i = 1; i <= n; ++i) {
        auto sum = _diag.main(i) * v(i);
        if (i > 1) {
            sum += _diag.lower(i) * v(i - 1);
        }
        if (i < n) {
            sum += _diag.upper(i) * v(i + 1);
        }
        result(i) = sum;
    }
    return result;
}

//Метод прогонки
Vector TridiagonalMatrix::solve(const Vector& d) {

    Vector L = Vector(n-1);
    Vector M = Vector(n);
    //Прямой ход
    //Первый шаг
    L(1) = _diag.upper(1) / _diag.main(1);
    M(1) = d(1) / _diag.main(1);

    //Шаг с первого до предпоследнего
    for (int i = 2; i < n; ++i) {
        auto denominator = _diag.main(i) - _diag.lower(i) * L(i - 1);
        if (denominator == 0.f) {
            throw out_of_range("Zero denominator");
        }
        L(i) = _diag.upper(i) / denominator;
        M(i) = (d(i) - _diag.lower(i) * M(i - 1)) / denominator;
    }

    auto denominator = _diag.main(n) - _diag.lower(n) * L(n - 1);
    if (denominator == 0.f) {
        throw out_of_range("Zero denominator");
    }
    M(n) = (d(n) - _diag.lower(n) * M(n - 1)) / denominator;

    //Обратный ход
    Vector x(n);
    x(n) = M(n);
    for (int i = n - 1; i >= 1; --i) {
        x(i) = - L(i) * x(i + 1) + M(i);
    }

    return x;
}