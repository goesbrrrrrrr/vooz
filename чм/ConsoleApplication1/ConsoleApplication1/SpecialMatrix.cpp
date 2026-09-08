#include "SpecialMatrix.h"
#include <stdexcept>

using namespace std;

SpecialMatrix::SpecialMatrix(int size, int k) : _diag(size),
                                                _n(size),
                                                _k(k),
                                                _kVect(size - 4)
{
    if (k <= 1 || k >= size - 1)
    {
        throw std::logic_error("K should be 1 < k < N-1");
    }

    _nLeft = _n - _k - 2;
    _nRight = _kVect.size() - _nLeft;

    if (_nLeft < 0 || _nRight < 0)
    {
        throw std::logic_error("Invalid nLeft or nRight");
    }
}

Vector SpecialMatrix::operator*(const Vector &v)
{
    if (v.size() != _n)
    {
        throw std::logic_error("Vector size must be same value as matrix size.");
    }

    Vector result(_n);

    for (int i = 1; i <= _n; ++i)
    {
        result(i) = 0;

        // Главная диагональ
        result(i) += _diag.main(i) * v(i);

        // Нижняя диагональ (под главной)
        if (i > 1)
        {
            result(i) += _diag.lower(i) * v(i - 1);
        }

        // Верхняя диагональ (над главной)
        if (i < _n)
        {
            result(i) += _diag.upper(i) * v(i + 1);
        }
    }

    // Вклад дополнительного вектора в строку k+1 (левая часть)
    for (int i = 1; i <= _nLeft; ++i)
    {
        result(_k + 1) += _kVect(i) * v(i);
    }

    // Вклад дополнительного вектора в строку k (правая часть)
    for (int i = 1; i <= _nRight; ++i)
    {
        int kVectIndex = _kVect.size() - i + 1;
        int colIndex = _n - i + 1;
        result(_k) += _kVect(kVectIndex) * v(colIndex);
    }

    return result;
}

Vector SpecialMatrix::solve(Vector &d)
{
    Vector x(_n);
    Vector L(_n - 1);
    Vector M(_n);

    // === Прямой ход сверху (до строки k-1) ===
    L(1) = _diag.upper(1) / _diag.main(1);
    M(1) = d(1) / _diag.main(1);

    for (int i = 2; i < _k; i++)
    {
        auto denominator = _diag.main(i) - (_diag.lower(i) * L(i - 1));
        if (denominator == 0.f)
        {
            throw std::runtime_error("Zero denominator in upper forward sweep");
        }
        L(i) = _diag.upper(i) / denominator;
        M(i) = (d(i) - _diag.lower(i) * M(i - 1)) / denominator;
    }

    PreciseDouble L_km1 = L(_k - 1);
    PreciseDouble M_km1 = M(_k - 1);

    // === Прямой ход снизу (от n до k+2) ===
    Vector L_lower(_n);
    Vector M_lower(_n);

    L_lower(_n) = _diag.lower(_n) / _diag.main(_n);
    M_lower(_n) = d(_n) / _diag.main(_n);

    for (int i = _n - 1; i > _k + 1; i--)
    {
        auto denominator = _diag.main(i) - (_diag.upper(i) * L_lower(i + 1));
        if (denominator == 0.f)
        {
            throw std::runtime_error("Zero denominator in lower forward sweep");
        }
        L_lower(i) = _diag.lower(i) / denominator;
        M_lower(i) = (d(i) - _diag.upper(i) * M_lower(i + 1)) / denominator;
    }

    PreciseDouble L_kp2 = L_lower(_k + 2);
    PreciseDouble M_kp2 = M_lower(_k + 2);

    // === Вычисление alpha(i), beta(i): x(i) = alpha(i) + beta(i)*x(k) для i=1..k-1 ===
    // Из верхней прогонки: x(i) = M(i) - L(i)*x(i+1)
    Vector alpha(_k - 1);
    Vector beta(_k - 1);

    alpha(_k - 1) = M_km1;
    beta(_k - 1) = -L_km1;

    for (int i = _k - 2; i >= 1; i--)
    {
        alpha(i) = M(i) - L(i) * alpha(i + 1);
        beta(i) = -(L(i) * beta(i + 1));
    }

    // === Вычисление gamma(i), delta(i): x(i) = gamma(i) + delta(i)*x(k+1) для i=k+2..n ===
    // Из нижней прогонки: x(i) = M_lower(i) - L_lower(i)*x(i-1)
    Vector gamma(_n - _k - 1, -(_k + 1));
    Vector delta(_n - _k - 1, -(_k + 1));

    gamma(_k + 2) = M_kp2;
    delta(_k + 2) = -L_kp2;

    for (int i = _k + 3; i <= _n; i++)
    {
        gamma(i) = M_lower(i) - L_lower(i) * gamma(i - 1);
        delta(i) = -(L_lower(i) * delta(i - 1));
    }

    // === Вклад kVect в строку k+1 (левые столбцы 1..nLeft) ===
    // sum kVect(j) * x(j) = sum kVect(j)*(alpha(j) + beta(j)*x(k))
    PreciseDouble kVect_const_kp1 = 0.0;
    PreciseDouble kVect_linear_kp1 = 0.0;
    for (int j = 1; j <= _nLeft; j++)
    {
        kVect_const_kp1 += _kVect(j) * alpha(j);
        kVect_linear_kp1 += _kVect(j) * beta(j);
    }

    // === Вклад kVect в строку k (правые столбцы n..n-nRight+1) ===
    // sum kVect(idx) * x(col) = sum kVect(idx)*(gamma(col) + delta(col)*x(k+1))
    PreciseDouble kVect_const_k = 0.0;
    PreciseDouble kVect_linear_k = 0.0;
    for (int i = 1; i <= _nRight; i++)
    {
        int kVectIndex = _kVect.size() - i + 1;
        int col = _n - i + 1;
        kVect_const_k += _kVect(kVectIndex) * gamma(col);
        kVect_linear_k += _kVect(kVectIndex) * delta(col);
    }

    // === Формирование системы 2x2 для строк k и k+1 ===
    PreciseDouble coeff11 = _diag.main(_k) - _diag.lower(_k) * L_km1;
    PreciseDouble coeff12 = _diag.upper(_k) + kVect_linear_k;
    PreciseDouble coeff21 = _diag.lower(_k + 1) + kVect_linear_kp1;
    PreciseDouble coeff22 = _diag.main(_k + 1) - _diag.upper(_k + 1) * L_kp2;

    PreciseDouble rhs1 = d(_k) - _diag.lower(_k) * M_km1 - kVect_const_k;
    PreciseDouble rhs2 = d(_k + 1) - _diag.upper(_k + 1) * M_kp2 - kVect_const_kp1;

    // Решение системы 2x2 методом Крамера
    PreciseDouble det = coeff11 * coeff22 - coeff12 * coeff21;
    if (det == 0.f)
    {
        throw std::runtime_error("Zero determinant in 2x2 system");
    }

    x(_k) = (rhs1 * coeff22 - coeff12 * rhs2) / det;
    x(_k + 1) = (coeff11 * rhs2 - rhs1 * coeff21) / det;

    // === Обратный ход сверху (от k-1 до 1) ===
    for (int i = _k - 1; i >= 1; i--)
    {
        x(i) = M(i) - L(i) * x(i + 1);
    }

    // === Обратный ход снизу (от k+2 до n) ===
    for (int i = _k + 2; i <= _n; i++)
    {
        x(i) = M_lower(i) - L_lower(i) * x(i - 1);
    }

    return x;
}

void SpecialMatrix::stepwiseEquivalenceControl(const Vector &x, const Vector &d_original)
{
    double epsilon = 1e-1;
    double max_residual = 0.0;

    cout << "\n--- EQUIVALENCE CONTROL ---\n";
    cout << fixed << scientific << setprecision(5);

    // Проверяем каждое уравнение
    for (int i = 1; i <= _n; ++i)
    {
        double residual = 0.0;

        // Левая часть исходного уравнения: A*x

        // Главная диагональ
        residual += _diag.main(i).toDouble() * x(i).toDouble();

        // Поддиагональ (i, i-1)
        if (i > 1)
        {
            residual += _diag.lower(i).toDouble() * x(i - 1).toDouble();
        }

        // Наддиагональ (i, i+1)
        if (i < _n)
        {
            residual += _diag.upper(i).toDouble() * x(i + 1).toDouble();
        }

        // Дополнительные связи через kVect
        if (i == _k)
        {
            // Строка k: связь с правыми столбцами
            for (int j = 1; j <= _nRight; ++j)
            {
                int col = _n - _nRight + j;                 // индексы столбцов
                int kVectIdx = _kVect.size() - _nRight + j; // индексы в kVect
                residual += _kVect(kVectIdx).toDouble() * x(col).toDouble();
            }
        }
        else if (i == _k + 1)
        {
            // Строка k+1: связь с левыми столбцами
            for (int j = 1; j <= _nLeft; ++j)
            {
                residual += _kVect(j).toDouble() * x(j).toDouble();
            }
        }

        // Невязка: |A*x - d|
        double error = fabs(residual - d_original(i).toDouble());
        max_residual = max(max_residual, error);

        cout << "Row " << setw(2) << i << ": residual = " << error << endl;
    }

    cout << "Max residual: " << max_residual << endl;

    if (max_residual < epsilon)
    {
        cout << "SYSTEM EQUIVALENT\n";
    }
    else
    {
        cout << "Warning: maybe loss of EQUIVALENCE!\n";
    }
}