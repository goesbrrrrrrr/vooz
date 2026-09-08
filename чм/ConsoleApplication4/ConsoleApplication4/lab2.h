#pragma once
#include <vector>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <iomanip>
#include <limits>
#include <sstream>
#include <algorithm>

class Vector
{
private:
    std::vector<double> data;
    int size;

public:
    Vector(int n = 0, double value = 0.0)
        : size(n), data(n, value) {
    }

    Vector(const Vector& other)
        : size(other.size), data(other.data) {
    }

    Vector(Vector&& other) noexcept
        : size(other.size), data(std::move(other.data)) {
    }

    Vector& operator=(const Vector& other)
    {
        if (this != &other)
        {
            size = other.size;
            data = other.data;
        }
        return *this;
    }

    Vector& operator=(Vector&& other) noexcept
    {
        if (this != &other)
        {
            size = other.size;
            data = std::move(other.data);
        }
        return *this;
    }

    int getLength() const { return size; }

    double& operator[](int i)
    {
        return data[i - 1];
    }

    const double& operator[](int i) const
    {
        return data[i - 1];
    }

    Vector operator+(const Vector& other) const
    {
        Vector result(size);
        for (int i = 1; i <= size; ++i)
            result[i] = (*this)[i] + other[i];
        return result;
    }

    Vector operator-(const Vector& other) const
    {
        Vector result(size);
        for (int i = 1; i <= size; ++i)
            result[i] = (*this)[i] - other[i];
        return result;
    }

    Vector operator*(double scalar) const
    {
        Vector result(size);
        for (int i = 1; i <= size; ++i)
            result[i] = (*this)[i] * scalar;
        return result;
    }

    double operator*(const Vector& other) const
    {
        double sum = 0.0;
        for (int i = 1; i <= size; ++i)
            sum += (*this)[i] * other[i];
        return sum;
    }

    void randomize(double min_val, double max_val)
    {
        for (int i = 1; i <= size; ++i)
        {
            double r = (double)rand() / RAND_MAX;
            (*this)[i] = min_val + r * (max_val - min_val);
        }
    }

    double norm() const
    {
        double sum = 0.0;
        for (int i = 1; i <= size; ++i)
            sum += (*this)[i] * (*this)[i];
        return std::sqrt(sum);
    }

    std::string toString() const
    {
        std::ostringstream oss;
        oss << std::scientific << std::setprecision(5);
        for (int i = 1; i <= size; ++i)
            oss << std::setw(15) << (*this)[i] << "\n";
        return oss.str();
    }

    friend std::ostream& operator<<(std::ostream& os, const Vector& vec)
    {
        os << vec.toString();
        return os;
    }
};

class SquareMatrix
{
private:
    std::vector<Vector> rows;
    int size;

public:
    SquareMatrix(int n = 0)
        : size(n), rows(n, Vector(n, 0.0)) {
    }

    SquareMatrix(const SquareMatrix& other)
        : size(other.size), rows(other.rows) {
    }

    SquareMatrix(SquareMatrix&& other) noexcept
        : size(other.size), rows(std::move(other.rows)) {
    }

    SquareMatrix& operator=(const SquareMatrix& other)
    {
        if (this != &other)
        {
            size = other.size;
            rows = other.rows;
        }
        return *this;
    }

    SquareMatrix& operator=(SquareMatrix&& other) noexcept
    {
        if (this != &other)
        {
            size = other.size;
            rows = std::move(other.rows);
        }
        return *this;
    }

    int getSize() const { return size; }

    Vector& operator[](int i)
    {
        return rows[i - 1];
    }

    const Vector& operator[](int i) const
    {
        return rows[i - 1];
    }

    SquareMatrix operator*(const SquareMatrix& other) const
    {
        SquareMatrix result(size);
        for (int i = 1; i <= size; ++i)
        {
            for (int j = 1; j <= size; ++j)
            {
                double sum = 0.0;
                for (int k = 1; k <= size; ++k)
                    sum += (*this)[i][k] * other[k][j];
                result[i][j] = sum;
            }
        }
        return result;
    }

    Vector operator*(const Vector& vec) const
    {
        Vector result(size);
        for (int i = 1; i <= size; ++i)
        {
            double sum = 0.0;
            for (int j = 1; j <= size; ++j)
                sum += (*this)[i][j] * vec[j];
            result[i] = sum;
        }
        return result;
    }

    void randomize(double min_val, double max_val)
    {
        for (int i = 1; i <= size; ++i)
            rows[i - 1].randomize(min_val, max_val);
    }

    SquareMatrix getTranspon() const
    {
        SquareMatrix result(size);
        for (int i = 1; i <= size; ++i)
            for (int j = 1; j <= size; ++j)
                result[i][j] = (*this)[j][i];
        return result;
    }

    void initializeZeros()
    {
        for (int i = 1; i <= size; ++i)
            for (int j = 1; j <= size; ++j)
                (*this)[i][j] = 0.0;
    }

    std::string toString() const
    {
        std::ostringstream oss;
        oss << std::scientific << std::setprecision(5);
        for (int i = 1; i <= size; ++i)
        {
            for (int j = 1; j <= size; ++j)
                oss << std::setw(15) << (*this)[i][j];
            oss << "\n";
        }
        return oss.str();
    }

    friend std::ostream& operator<<(std::ostream& os, const SquareMatrix& mat)
    {
        os << mat.toString();
        return os;
    }
};

class BandMatrix
{
private:
    std::vector<std::vector<double>> data;
    int size;
    int bandWidth;
    double dummy = 0.0;

public:
    BandMatrix(int n = 0, int bw = 1)
        : size(n), bandWidth(bw)
    {
        data.resize(n, std::vector<double>(bw, 0.0));
    }

    BandMatrix(const BandMatrix& other)
        : size(other.size), bandWidth(other.bandWidth), data(other.data) {
    }

    int getSize() const { return size; }
    int getBandWidth() const { return bandWidth; }

    // Доступ к элементу (i, j) - за пределами ленты возвращаем 0
    double& operator()(int i, int j)
    {
        if (i < 1 || i > size || j < 1 || j > size)
            throw std::out_of_range("Index out of range");

        int diff = j - i;
        if (diff < 0 || diff >= bandWidth)
        {
            dummy = 0.0;
            return dummy;
        }
        return data[i - 1][diff];
    }

    const double& operator()(int i, int j) const
    {
        if (i < 1 || i > size || j < 1 || j > size)
            throw std::out_of_range("Index out of range");

        int diff = j - i;
        if (diff < 0 || diff >= bandWidth)
        {
            static const double zero = 0.0;
            return zero;
        }
        return data[i - 1][diff];
    }

    void randomize(double min_val, double max_val)
    {
        for (int i = 1; i <= size; ++i)
        {
            for (int j = i; j <= std::min(size, i + bandWidth - 1); ++j)
            {
                double r = (double)rand() / RAND_MAX;
                data[i - 1][j - i] = min_val + r * (max_val - min_val);
            }
        }
    }

    void initializeZeros()
    {
        for (int i = 0; i < size; ++i)
            for (int j = 0; j < bandWidth; ++j)
                data[i][j] = 0.0;
    }

    BandMatrix operator*(const BandMatrix& other) const
    {
        BandMatrix result(size, bandWidth);
        for (int i = 1; i <= size; ++i)
        {
            for (int j = 1; j <= size; ++j)
            {
                if (std::abs(j - i) >= bandWidth)
                    continue;

                double sum = 0.0;
                int start = std::max(1, std::max(i - bandWidth + 1, j - bandWidth + 1));
                int end = std::min(size, std::min(i + bandWidth - 1, j + bandWidth - 1));

                for (int k = start; k <= end; ++k)
                {
                    if (std::abs(k - i) < bandWidth && std::abs(j - k) < bandWidth)
                    {
                        sum += (*this)(i, k) * other(k, j);
                    }
                }
                if (std::abs(sum) > 1e-15)
                {
                    result.data[i - 1][j - i] = sum;
                }
            }
        }
        return result;
    }

    Vector operator*(const Vector& vec) const
    {
        Vector result(size);
        for (int i = 1; i <= size; ++i)
        {
            double sum = 0.0;
            for (int j = i; j <= std::min(size, i + bandWidth - 1); ++j)
                sum += (*this)(i, j) * vec[j];
            result[i] = sum;
        }
        return result;
    }

    std::string toString() const
    {
        std::ostringstream oss;
        oss << std::scientific << std::setprecision(5);
        for (int i = 1; i <= size; ++i)
        {
            for (int j = 1; j <= size; ++j)
            {
                if (j - i < 0 || j - i >= bandWidth)
                    oss << std::setw(15) << "0.00000e+00";
                else
                    oss << std::setw(15) << (*this)(i, j);
            }
            oss << "\n";
        }
        return oss.str();
    }

    friend std::ostream& operator<<(std::ostream& os, const BandMatrix& mat)
    {
        os << mat.toString();
        return os;
    }
};

inline double machineEpsilon()
{
    return std::numeric_limits<double>::epsilon();
}

inline double my_abs(double x)
{
    return std::fabs(x);
}

namespace LineralAlgebra
{
    namespace square_matrix_utils
    {
        inline int findPivotRow(const SquareMatrix& matrix, int column)
        {
            int rowNumber = column;
            double maxValue = my_abs(matrix[column][column]);

            for (int i = column + 1; i <= matrix.getSize(); ++i)
            {
                double currentValue = std::abs(matrix[i][column]);
                if (maxValue < currentValue)
                {
                    maxValue = currentValue;
                    rowNumber = i;
                }
            }
            return rowNumber;
        }

        inline void swapRows(SquareMatrix& matrix, int row1, int row2)
        {
            Vector temp(std::move(matrix[row1]));
            matrix[row1] = std::move(matrix[row2]);
            matrix[row2] = std::move(temp);
        }

        Vector solveGauss(SquareMatrix matrix, Vector vec, bool COUT = false)
        {
            if (matrix.getSize() != vec.getLength())
                throw std::invalid_argument("Vector size must match matrix size");

            const double EPS = machineEpsilon();
            int size = vec.getLength();
            Vector res(size, 0);

            // Прямой ход - приводим к верхнетреугольной матрице
            int column = 1;
            for (int i = 1; i <= size; ++i)
            {
                int pivotRow = findPivotRow(matrix, column);
                swapRows(matrix, i, pivotRow);
                std::swap(vec[i], vec[pivotRow]);

                double denominator = matrix[i][column];
                if (my_abs(denominator) < EPS)
                    throw std::runtime_error("Determinant of the matrix is zero.");

                double multiple = 1.0 / denominator;

                matrix[i][column] = 1;
                for (int j = column + 1; j <= size; ++j)
                    matrix[i][j] *= multiple;
                vec[i] *= multiple;

                for (int k = i + 1; k <= size; ++k)
                {
                    double coeff = matrix[k][column];
                    matrix[k] = matrix[k] - (matrix[i] * coeff);
                    vec[k] -= (vec[i] * coeff);
                }

                ++column;
            }

            // Обратный ход - ищем решение
            for (int i = size; i >= 1; --i)
            {
                double sum = 0;
                for (int j = i + 1; j <= size; ++j)
                    sum += matrix[i][j] * res[j];
                res[i] = vec[i] - sum;
            }
            return res;
        }
    }

    namespace band_matrix_utils
    {
        // Разложение Халецкого: A = B * C (B нижнетреугольная, C верхнетреугольная)
        std::pair<BandMatrix, BandMatrix> computeCholeskyLUDecomposition(const BandMatrix& matrix)
        {
            int size = matrix.getSize(), bandWidth = matrix.getBandWidth();

            BandMatrix B(size, bandWidth);
            BandMatrix C(size, bandWidth);

            auto K0 = [bandWidth](int i) -> int
                {
                    return (i <= bandWidth) ? 1 : (i - bandWidth + 1);
                };

            auto KN = [size, bandWidth](int i) -> int
                {
                    return (i <= size - bandWidth) ? (i + bandWidth - 1) : size;
                };

            B.initializeZeros();
            C.initializeZeros();

            for (int j = 1; j <= size; ++j)
            {
                int K_0j = K0(j);
                int K_Nj = KN(j);

                double sum_diag = 0.0;
                for (int k = K_0j; k <= j - 1; ++k)
                {
                    if (std::abs(j - k) < bandWidth && std::abs(j - k) < bandWidth)
                        sum_diag += B(j, k) * C(k, j);
                }
                B(j, j) = matrix(j, j) - sum_diag;

                for (int i = j + 1; i <= K_Nj; ++i)
                {
                    double sum_lower = 0.0;
                    for (int k = K_0j; k <= j - 1; ++k)
                    {
                        if (std::abs(i - k) < bandWidth && std::abs(j - k) < bandWidth)
                            sum_lower += B(i, k) * C(k, j);
                    }
                    B(i, j) = matrix(i, j) - sum_lower;
                }

                C(j, j) = 1.0;

                if (std::abs(B(j, j)) < 1e-15)
                    throw std::runtime_error("Singular matrix");

                for (int i = j + 1; i <= K_Nj; ++i)
                {
                    double sum_upper = 0.0;
                    for (int k = K_0j; k <= j - 1; ++k)
                    {
                        if (std::abs(j - k) < bandWidth && std::abs(i - k) < bandWidth)
                            sum_upper += B(j, k) * C(k, i);
                    }
                    C(j, i) = (matrix(j, i) - sum_upper) / B(j, j);
                }
            }

            return std::make_pair(B, C);
        }

        Vector solveLowerTriangular(const BandMatrix& B, const Vector& f)
        {
            int size = B.getSize();
            if (size != f.getLength())
                throw std::invalid_argument("Vector size must match matrix size");

            Vector y(size, 0);

            for (int i = 1; i <= size; ++i)
            {
                double sum = 0.0;

                int start = std::max(1, i - B.getBandWidth() + 1);
                for (int j = start; j < i; ++j)
                {
                    if (std::abs(i - j) < B.getBandWidth())
                        sum += B(i, j) * y[j];
                }

                if (std::abs(B(i, i)) < 1e-15)
                    throw std::runtime_error("Zero diagonal element in B matrix");

                y[i] = (f[i] - sum) / B(i, i);
            }

            return y;
        }

        Vector solveUpperTriangular(const BandMatrix& C, const Vector& y)
        {
            int size = C.getSize();
            if (size != y.getLength())
                throw std::invalid_argument("Vector size must match matrix size");

            Vector x(size, 0);

            for (int i = size; i >= 1; --i)
            {
                double sum = 0.0;

                int end = std::min(size, i + C.getBandWidth() - 1);
                for (int j = i + 1; j <= end; ++j)
                {
                    if (std::abs(j - i) < C.getBandWidth())
                        sum += C(i, j) * x[j];
                }

                x[i] = y[i] - sum;
            }

            return x;
        }

        Vector solveBandCholesky(const BandMatrix& matrix, const Vector& vec, bool COUT = false)
        {
            if (matrix.getSize() != vec.getLength())
                throw std::invalid_argument("Vector size must match matrix size");

            auto LU = computeCholeskyLUDecomposition(matrix);
            BandMatrix B = LU.first;
            BandMatrix C = LU.second;

            Vector y = solveLowerTriangular(B, vec);
            Vector x = solveUpperTriangular(C, y);

            return x;
        }
    }

    namespace symmetrical_matrix_utils
    {
        // Разложение S^T*D*S для симметричных матриц
        std::pair<SquareMatrix, SquareMatrix> computeSTDSecomposition(const SquareMatrix matrix)
        {
            int n = matrix.getSize();
            double EPS = 1.0E-10;
            SquareMatrix S(n);
            SquareMatrix D(n);

            S.initializeZeros();
            D.initializeZeros();

            for (int i = 1; i <= n; ++i)
            {
                double sum = 0.0;
                for (int k = 1; k <= i - 1; ++k)
                    sum += S[k][i] * S[k][i] * D[k][k];

                double value = matrix[i][i] - sum;

                if (value > 0)
                    D[i][i] = 1.0;
                else if (value < 0)
                    D[i][i] = -1.0;
                else
                    D[i][i] = 1.0;

                S[i][i] = std::sqrt(std::abs(value));

                for (int j = i + 1; j <= n; ++j)
                {
                    sum = 0.0;
                    for (int k = 1; k <= i - 1; ++k)
                        sum += S[k][i] * S[k][j] * D[k][k];

                    if (std::abs(S[i][i]) > EPS)
                        S[i][j] = (matrix[i][j] - sum) / (D[i][i] * S[i][i]);
                    else
                        S[i][j] = 0.0;
                }
            }

            return std::make_pair(S, D);
        }

        Vector solveUpperTriangularTransposed(const SquareMatrix& S, const Vector& f)
        {
            int n = S.getSize();
            if (n != f.getLength())
                throw std::invalid_argument("Vector size must match matrix size");

            Vector y(n, 0);

            for (int i = 1; i <= n; ++i)
            {
                double sum = 0.0;
                for (int k = 1; k <= i - 1; ++k)
                    sum += S[k][i] * y[k];

                if (std::abs(S[i][i]) > 1e-15)
                    y[i] = (f[i] - sum) / S[i][i];
                else
                    y[i] = 0.0;
            }

            return y;
        }

        Vector solveUpperTriangular(const SquareMatrix& S, const SquareMatrix& D, const Vector& y)
        {
            int n = S.getSize();
            if (n != y.getLength())
                throw std::invalid_argument("Vector size must match matrix size");

            Vector x(n, 0);

            for (int i = n; i >= 1; --i)
            {
                double sum = 0.0;
                for (int k = i + 1; k <= n; ++k)
                    sum += S[i][k] * x[k];

                if (std::abs(S[i][i]) > 1e-15)
                    x[i] = (D[i][i] * y[i] - sum) / S[i][i];
                else
                    x[i] = 0.0;
            }

            return x;
        }

        Vector solveViaSTDS(const SquareMatrix& matrix, const Vector& vec, bool COUT = false)
        {
            auto SD = computeSTDSecomposition(matrix);
            SquareMatrix S = SD.first;
            SquareMatrix D = SD.second;

            Vector y = solveUpperTriangularTransposed(S, vec);
            Vector x = solveUpperTriangular(S, D, y);

            return x;
        }
    }

    namespace public_interface
    {
        inline Vector solveGauss(SquareMatrix& matrix, Vector& d_true)
        {
            return square_matrix_utils::solveGauss(matrix, d_true);
        }

        inline Vector solveBandCholesky(BandMatrix& matrix, Vector& d_true)
        {
            return band_matrix_utils::solveBandCholesky(matrix, d_true);
        }

        inline Vector solveViaSTDS(SquareMatrix& matrix, Vector& d_true)
        {
            return symmetrical_matrix_utils::solveViaSTDS(matrix, d_true);
        }

        inline double computeAbsoluteError(const Vector& x_true, const Vector& x_calculated)
        {
            double maxError = 0.0;
            for (int i = 1; i <= x_true.getLength(); ++i)
            {
                double error = my_abs(x_true[i] - x_calculated[i]);
                if (error > maxError)
                    maxError = error;
            }
            return maxError;
        }
    }
}

inline BandMatrix CreateMatrixBand(int size, int bandWidth)
{
    return BandMatrix(size, bandWidth);
}

inline SquareMatrix CreateMatrixSquare(int size, int)
{
    return SquareMatrix(size);
}

inline Vector solveWithGauss(SquareMatrix& matrix, Vector& d_true)
{
    return LineralAlgebra::public_interface::solveGauss(matrix, d_true);
}

inline Vector solveWithCholeskyBand(BandMatrix& matrix, Vector& d_true)
{
    return LineralAlgebra::public_interface::solveBandCholesky(matrix, d_true);
}

inline Vector solveSymmetricSystem(SquareMatrix& matrix, Vector& d_true)
{
    return LineralAlgebra::public_interface::solveViaSTDS(matrix, d_true);
}

template<typename MatrixType>
void runTest(std::vector<int> sizes, std::vector<double> ranges,
    Vector(*solver)(MatrixType&, Vector&),
    MatrixType(*createMatrix)(int, int),
    void (MatrixType::* Rand)(double, double),
    double widthCoeff = 0)
{
    std::cout << std::setw(20) << "Размерность\\Диапазон";

    for (auto range : ranges)
        std::cout << std::setw(20) << range;

    std::cout << std::endl
        << "---------------------------------------------------------------------"
        << std::endl;

    std::cout << std::scientific << std::setprecision(5);

    for (auto N : sizes)
    {
        std::cout << std::setw(20) << std::left << N;

        for (auto range : ranges)
        {
            Vector x_true(N);

            int matrixParam = static_cast<int>(N * widthCoeff);
            if (matrixParam < 1)
                matrixParam = 1;

            MatrixType matrix = createMatrix(N, matrixParam);

            double min_val = -range;
            double max_val = range;
            x_true.randomize(min_val, max_val);
            (matrix.*Rand)(min_val, max_val);

            Vector d_true = matrix * x_true;
            Vector x_calculated = solver(matrix, d_true);

            double errorSolution = LineralAlgebra::public_interface::computeAbsoluteError(x_true, x_calculated);

            std::cout << std::setw(20) << std::right << errorSolution;
        }
        std::cout << std::endl;
    }
    std::cout << std::defaultfloat;
}

void runBandTest(std::vector<int> sizes, std::vector<double> ranges, double widthCoeff, std::string label)
{
    std::cout << "МЕТОД ХАЛЕЦКОГО (ленточные матрицы, L/N = " << label << "):\n";
    std::cout << std::setw(20) << "Размерность\\Диапазон";

    for (auto range : ranges)
        std::cout << std::setw(20) << range;

    std::cout << std::endl
        << "---------------------------------------------------------------------"
        << std::endl;

    std::cout << std::scientific << std::setprecision(5);

    for (auto N : sizes)
    {
        std::cout << std::setw(20) << std::left << N;

        for (auto range : ranges)
        {
            Vector x_true(N);

            int bandWidth = std::max(2, (int)(N * widthCoeff));
            if (bandWidth > N) bandWidth = N;

            BandMatrix matrix(N, bandWidth);

            for (int i = 1; i <= N; ++i)
            {
                for (int j = i; j <= std::min(N, i + bandWidth - 1); ++j)
                {
                    double r = (double)rand() / RAND_MAX;
                    double val = -range + r * (2.0 * range);
                    matrix(i, j) = val;
                }
            }

            for (int i = 1; i <= N; ++i)
            {
                double sum = 0.0;
                for (int j = 1; j <= N; ++j)
                {
                    if (i != j && std::abs(i - j) < bandWidth)
                        sum += std::abs(matrix(i, j));
                }
                matrix(i, i) = sum + N + 1.0;
            }

            x_true.randomize(-range, range);
            Vector d_true = matrix * x_true;
            Vector x_calculated = LineralAlgebra::public_interface::solveBandCholesky(matrix, d_true);

            double errorSolution = LineralAlgebra::public_interface::computeAbsoluteError(x_true, x_calculated);

            std::cout << std::setw(20) << std::right << errorSolution;
        }
        std::cout << std::endl;
    }
    std::cout << std::defaultfloat;
}

void runSymmetricTest(std::vector<int> sizes, std::vector<double> ranges)
{
    std::cout << std::setw(20) << "Размерность\\Диапазон";

    for (auto range : ranges)
        std::cout << std::setw(20) << range;

    std::cout << std::endl
        << "---------------------------------------------------------------------"
        << std::endl;

    std::cout << std::scientific << std::setprecision(5);

    for (auto N : sizes)
    {
        std::cout << std::setw(20) << std::left << N;

        for (auto range : ranges)
        {
            Vector x_true(N);

            SquareMatrix matrix(N);

            matrix.randomize(-range, range);

            for (int i = 1; i <= N; ++i)
            {
                for (int j = i + 1; j <= N; ++j)
                {
                    double avg = (matrix[i][j] + matrix[j][i]) / 2.0;
                    matrix[i][j] = avg;
                    matrix[j][i] = avg;
                }
            }

            for (int i = 1; i <= N; ++i)
            {
                double sum = 0.0;
                for (int j = 1; j <= N; ++j)
                    if (i != j)
                        sum += std::abs(matrix[i][j]);
                matrix[i][i] = sum + N + 1.0;
            }

            x_true.randomize(-range, range);
            Vector d_true = matrix * x_true;
            Vector x_calculated = LineralAlgebra::public_interface::solveViaSTDS(matrix, d_true);

            double errorSolution = LineralAlgebra::public_interface::computeAbsoluteError(x_true, x_calculated);

            std::cout << std::setw(20) << std::right << errorSolution;
        }
        std::cout << std::endl;
    }
    std::cout << std::defaultfloat;
}

int main2()
{
    setlocale(LC_ALL, "ru");
    std::cout << "\n";

    std::cout << "1. МЕТОД ГАУССА С ВЫБОРОМ ГЛАВНОГО ЭЛЕМЕНТА - ТЕСТОВАЯ ЗАДАЧА                                   \n";
    std::cout << "\n";

    SquareMatrix A1(4);
    A1[1][1] = -2.0;    A1[1][2] = 1.0;     A1[1][3] = 1.0;     A1[1][4] = 3.0;
    A1[2][1] = -4.0;    A1[2][2] = -3.0;    A1[2][3] = 0.0;     A1[2][4] = -4.0;
    A1[3][1] = -5.0;    A1[3][2] = -4.0;    A1[3][3] = -5.0;    A1[3][4] = -1.0;
    A1[4][1] = 1.0;     A1[4][2] = -3.0;    A1[4][3] = 4.0;     A1[4][4] = -3.0;

    Vector b1(4);
    b1[1] = -7.0;
    b1[2] = 21.0;
    b1[3] = 48.0;
    b1[4] = -10.0;

    std::cout << "Матрица A (4×4):\n" << A1 << std::endl;
    std::cout << "Вектор b (правая часть):\n" << b1 << std::endl;

    try
    {
        Vector x1 = LineralAlgebra::public_interface::solveGauss(A1, b1);
        Vector check1 = A1 * x1;

        std::cout << "Найденное решение x:\n" << x1 << std::endl;
        std::cout << "Проверка: A*x =\n" << check1 << std::endl;

        double err1 = (check1 - b1).norm();
        std::cout << "Невязка ||A*x - b|| = " << std::scientific << std::setprecision(5) << err1 << "\n\n";
    }
    catch (const std::exception& e)
    {
        std::cout << "ОШИБКА: " << e.what() << std::endl;
    }

    std::cout << "2. МЕТОД ХАЛЕЦКОГО (LU-РАЗЛОЖЕНИЕ ДЛЯ ЛЕНТОЧНЫХ МАТРИЦ) - ТЕСТОВАЯ ЗАДАЧА                      \n";
    std::cout << "\n";

    BandMatrix A2(4, 4);

    A2(1, 1) = 5.0;     A2(1, 2) = -10.0;   A2(1, 3) = 2.0;     A2(1, 4) = 6.0;
    A2(2, 1) = -10.0;   A2(2, 2) = -1.0;    A2(2, 3) = -7.0;    A2(2, 4) = 2.0;
    A2(3, 1) = 2.0;     A2(3, 2) = -7.0;    A2(3, 3) = -9.0;    A2(3, 4) = 9.0;
    A2(4, 1) = 6.0;     A2(4, 2) = 2.0;     A2(4, 3) = 9.0;     A2(4, 4) = 5.0;

    Vector b2(4);
    b2[1] = -1.0;
    b2[2] = -10.0;
    b2[3] = 64.0;
    b2[4] = -82.0;

    std::cout << "Матрица A (4×4):\n" << A2 << std::endl;
    std::cout << "Вектор b (правая часть):\n" << b2 << std::endl;

    try
    {
        Vector x2 = LineralAlgebra::public_interface::solveBandCholesky(A2, b2);
        Vector check2 = A2 * x2;

        std::cout << "Найденное решение x:\n" << x2 << std::endl;

        double err2 = (check2 - b2).norm();
        std::cout << "Невязка ||A*x - b|| = " << std::scientific << std::setprecision(5) << err2 << "\n\n";
    }
    catch (const std::exception& e)
    {
        std::cout << "ОШИБКА: " << e.what() << std::endl;
    }

    std::cout << "3. МЕТОД КВАДРАТНЫХ КОРНЕЙ (S^T D S РАЗЛОЖЕНИЕ) - ТЕСТОВАЯ ЗАДАЧА                              \n";
    std::cout << "\n";

    int stdsSize = 4;
    SquareMatrix A3(stdsSize);
    Vector b3(stdsSize);

    std::cout << "Генерируется симметричная матрица 4×4 из диапазона [-100, 100]...\n\n";

    A3.randomize(-100.0, 100.0);

    for (int i = 1; i <= stdsSize; ++i)
    {
        for (int j = i + 1; j <= stdsSize; ++j)
        {
            double avg = (A3[i][j] + A3[j][i]) / 2.0;
            A3[i][j] = avg;
            A3[j][i] = avg;
        }
    }

    for (int i = 1; i <= stdsSize; ++i)
    {
        double sum = 0.0;
        for (int j = 1; j <= stdsSize; ++j)
            if (i != j)
                sum += std::abs(A3[i][j]);
        A3[i][i] = sum + stdsSize + 1.0;
    }

    b3.randomize(-100.0, 100.0);

    std::cout << "Матрица A (4×4, симметричная, случайная):\n" << A3 << std::endl;
    std::cout << "Вектор решения x (случайный):\n" << b3 << std::endl;

    Vector d3 = A3 * b3;
    std::cout << "Вектор правой части b = A*x:\n" << d3 << std::endl;

    try
    {
        Vector x3 = LineralAlgebra::public_interface::solveViaSTDS(A3, d3);
        Vector check3 = A3 * x3;

        std::cout << "Найденное решение x:\n" << x3 << std::endl;
        std::cout << "Исходное решение (для сравнения):\n" << b3 << std::endl;

        double err3 = (x3 - b3).norm();
        std::cout << "Невязка ||найденное x - исходное x|| = " << std::scientific << std::setprecision(5) << err3 << "\n\n";
    }
    catch (const std::exception& e)
    {
        std::cout << "ОШИБКА: " << e.what() << std::endl;
    }

    std::cout << "4. АНАЛИЗ ТОЧНОСТИ ДЛЯ РАЗНЫХ РАЗМЕРНОСТЕЙ И ДИАПАЗОНОВ ЗНАЧЕНИЙ                               \n";
    std::cout << "\n";

    std::vector<int> sizes = { 4, 8, 16, 32, 64, 128, 256, 512 };
    std::vector<double> ranges = { 0.1, 10.0, 100.0, 1000.0 };

    std::cout << "Вычислительный эксперимент:\n\n";
    std::cout << "МЕТОД ГАУССА (полные матрицы):\n";
    runTest<SquareMatrix>(
        sizes,
        ranges,
        solveWithGauss,
        CreateMatrixSquare,
        &SquareMatrix::randomize
    );

    std::cout << "\n\n";
    runBandTest(sizes, ranges, 0.1, "1/10");

    std::cout << "\n\n";
    runBandTest(sizes, ranges, 0.4, "4/10");

    std::cout << "\n\n";
    runBandTest(sizes, ranges, 0.8, "8/10");

    std::cout << "\n\nМЕТОД STDS (симметричные матрицы):\n";
    runSymmetricTest(sizes, ranges);

    std::cout << "Нажмите Enter для выхода...";
    std::cin.get();

    return 0;
}
