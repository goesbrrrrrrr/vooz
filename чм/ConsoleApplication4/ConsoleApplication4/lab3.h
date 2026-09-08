#pragma once
#include <iostream>
#include <cmath>
#include <iomanip>
#include <vector>
#include <random>
#include <numeric>
#include <sstream>
#include <algorithm>

const double EPS = 1e-15;

struct RotationParams {
    int p;
    int q;
    double c;
    double s;
};

double randomDouble(double min, double max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dis(min, max);
    return dis(gen);
}

RotationParams findMaxOffDiagonal(const std::vector<std::vector<double>>& A, int n) {
    RotationParams maxElem{ 1, 2, 0.0, 0.0 };
    double maxVal = 0.0;
    for (int i = 1; i <= n; ++i) {
        for (int j = i + 1; j <= n; ++j) {
            if (std::abs(A[i][j]) > maxVal) {
                maxVal = std::abs(A[i][j]);
                maxElem.p = i;
                maxElem.q = j;
            }
        }
    }
    return maxElem;
}

RotationParams computeRotation(const std::vector<std::vector<double>>& A, int p, int q) {
    RotationParams rot{ p, q, 0.0, 0.0 };

    double app = A[p][p];
    double aqq = A[q][q];
    double apq = A[p][q];

    if (std::abs(apq) < EPS) {
        rot.c = 1.0;
        rot.s = 0.0;
        return rot;
    }

    double tau = (aqq - app) / (2.0 * apq);
    double t;

    if (tau >= 0) {
        t = 1.0 / (tau + std::sqrt(1.0 + tau * tau));
    }
    else {
        t = -1.0 / (-tau + std::sqrt(1.0 + tau * tau));
    }

    rot.c = 1.0 / std::sqrt(1.0 + t * t);
    rot.s = t * rot.c;

    return rot;
}

void applyRotation(std::vector<std::vector<double>>& A,
    std::vector<std::vector<double>>& T,
    const RotationParams& rot) {
    int n = A.size() - 1;
    int p = rot.p;
    int q = rot.q;
    double c = rot.c;
    double s = rot.s;

    double app = A[p][p];
    double aqq = A[q][q];
    double apq = A[p][q];

    A[p][p] = c * c * app + s * s * aqq - 2.0 * c * s * apq;
    A[q][q] = s * s * app + c * c * aqq + 2.0 * c * s * apq;

    for (int j = 1; j <= n; ++j) {
        if (j != p && j != q) {
            double apj = A[p][j];
            double aqj = A[q][j];
            A[p][j] = c * apj - s * aqj;
            A[q][j] = s * apj + c * aqj;
            A[j][p] = A[p][j];
            A[j][q] = A[q][j];
        }
    }

    A[p][q] = 0.0;
    A[q][p] = 0.0;

    for (int i = 1; i <= n; ++i) {
        double tip = T[i][p];
        double tiq = T[i][q];
        T[i][p] = c * tip - s * tiq;
        T[i][q] = s * tip + c * tiq;
    }
}

double computeAccuracy(const std::vector<std::vector<double>>& A_orig,
    const std::vector<std::vector<double>>& T,
    const std::vector<double>& lambda,
    int n) {
    std::vector<std::vector<double>> B(n + 1, std::vector<double>(n + 1, 0.0));
    std::vector<std::vector<double>> C(n + 1, std::vector<double>(n + 1, 0.0));

    for (int i = 1; i <= n; ++i) {
        for (int j = 1; j <= n; ++j) {
            double sum = 0.0;
            for (int k = 1; k <= n; ++k) {
                sum += A_orig[i][k] * T[k][j];
            }
            C[i][j] = sum;
        }
    }

    for (int i = 1; i <= n; ++i) {
        for (int j = 1; j <= n; ++j) {
            double sum = 0.0;
            for (int k = 1; k <= n; ++k) {
                sum += T[k][i] * C[k][j];
            }
            B[i][j] = sum;
        }
    }

    double maxDiff = 0.0;
    for (int i = 1; i <= n; ++i) {
        for (int j = 1; j <= n; ++j) {
            double expected = (i == j) ? lambda[i] : 0.0;
            double diff = std::abs(B[i][j] - expected);
            if (diff > maxDiff) maxDiff = diff;
        }
    }
    return maxDiff;
}

double computeEigenvalueError(std::vector<double> lambda_true,
    std::vector<double> lambda_computed,
    int n) {
    std::sort(lambda_true.begin() + 1, lambda_true.end());
    std::sort(lambda_computed.begin() + 1, lambda_computed.end());

    double maxError = 0.0;
    for (int i = 1; i <= n; ++i) {
        double error = std::abs(lambda_true[i] - lambda_computed[i]);
        if (error > maxError) maxError = error;
    }
    return maxError;
}

int jacobiMethod(int N,
    std::vector<std::vector<double>>& A,
    double epsilon,
    int M,
    std::vector<std::vector<double>>& T,
    int& K,
    std::vector<double>& lambda,
    double& maxOffDiag) {
    K = 0;
    T.assign(N + 1, std::vector<double>(N + 1, 0.0));
    for (int i = 1; i <= N; ++i) T[i][i] = 1.0;

    while (K < M) {
        RotationParams maxElem = findMaxOffDiagonal(A, N);
        maxOffDiag = std::abs(A[maxElem.p][maxElem.q]);

        if (maxOffDiag < epsilon) {
            lambda.resize(N + 1);
            for (int i = 1; i <= N; ++i) lambda[i] = A[i][i];
            return 0;
        }

        RotationParams rot = computeRotation(A, maxElem.p, maxElem.q);
        applyRotation(A, T, rot);
        K++;
    }

    lambda.resize(N + 1);
    for (int i = 1; i <= N; ++i) lambda[i] = A[i][i];
    return 1;
}

void generateTestMatrix(int N,
    double lambda_min, double lambda_max,
    std::vector<std::vector<double>>& A,
    std::vector<double>& lambda_true) {
    lambda_true.resize(N + 1);
    for (int i = 1; i <= N; ++i) lambda_true[i] = randomDouble(lambda_min, lambda_max);

    std::vector<double> omega(N + 1);
    double norm = 0.0;
    for (int i = 1; i <= N; ++i) {
        omega[i] = randomDouble(-1.0, 1.0);
        norm += omega[i] * omega[i];
    }
    norm = std::sqrt(norm);
    for (int i = 1; i <= N; ++i) omega[i] /= norm;

    std::vector<std::vector<double>> H(N + 1, std::vector<double>(N + 1));
    for (int i = 1; i <= N; ++i) {
        for (int j = 1; j <= N; ++j) {
            if (i == j) H[i][j] = 1.0 - 2.0 * omega[i] * omega[j];
            else H[i][j] = -2.0 * omega[i] * omega[j];
        }
    }

    A.assign(N + 1, std::vector<double>(N + 1, 0.0));
    std::vector<std::vector<double>> temp(N + 1, std::vector<double>(N + 1, 0.0));

    for (int i = 1; i <= N; ++i) {
        for (int j = 1; j <= N; ++j) {
            temp[i][j] = lambda_true[i] * H[i][j];
        }
    }

    for (int i = 1; i <= N; ++i) {
        for (int j = 1; j <= N; ++j) {
            double sum = 0.0;
            for (int k = 1; k <= N; ++k) {
                sum += H[k][i] * temp[k][j];
            }
            A[i][j] = sum;
        }
    }
}

struct TestResult {
    int testNumber;
    int N;
    double lambda_min;
    double lambda_max;
    double maxOffDiagFinal;
    double avgIterations;
    double avgEigenvalueError;
    double avgAccuracy;
};

TestResult runExperiment(int N,
    double lambda_min, double lambda_max,
    double epsilon,
    int numTrials) {
    TestResult result;
    result.testNumber = 0;
    result.N = N;
    result.lambda_min = lambda_min;
    result.lambda_max = lambda_max;

    std::vector<double> iterations(numTrials);
    std::vector<double> eigenvalueErrors(numTrials);
    std::vector<double> accuracies(numTrials);
    std::vector<double> maxOffDiagValues(numTrials);

    for (int trial = 0; trial < numTrials; ++trial) {
        std::vector<std::vector<double>> A;
        std::vector<double> lambda_true;
        generateTestMatrix(N, lambda_min, lambda_max, A, lambda_true);
        std::vector<std::vector<double>> A_orig = A;

        std::vector<std::vector<double>> T;
        int K;
        std::vector<double> lambda_computed;
        double maxOffDiag;

        int ier = jacobiMethod(N, A, epsilon, 10000, T, K, lambda_computed, maxOffDiag);

        iterations[trial] = K;
        maxOffDiagValues[trial] = maxOffDiag;
        eigenvalueErrors[trial] = computeEigenvalueError(lambda_true, lambda_computed, N);
        accuracies[trial] = computeAccuracy(A_orig, T, lambda_computed, N);
    }

    result.maxOffDiagFinal = std::accumulate(maxOffDiagValues.begin(), maxOffDiagValues.end(), 0.0) / numTrials;
    result.avgIterations = std::accumulate(iterations.begin(), iterations.end(), 0.0) / numTrials;
    result.avgEigenvalueError = std::accumulate(eigenvalueErrors.begin(), eigenvalueErrors.end(), 0.0) / numTrials;
    result.avgAccuracy = std::accumulate(accuracies.begin(), accuracies.end(), 0.0) / numTrials;

    return result;
}

std::string formatWithSignificantDigits(double value, int significantDigits = 2) {
    if (value == 0.0) return "0";
    double absValue = std::abs(value);
    double exponent = std::floor(std::log10(absValue));
    double mantissa = absValue / std::pow(10.0, exponent);
    double factor = std::pow(10.0, significantDigits - 1);
    mantissa = std::round(mantissa * factor) / factor;
    double result = mantissa * std::pow(10.0, exponent);
    if (value < 0) result = -result;
    std::ostringstream oss;
    oss << std::scientific << std::setprecision(significantDigits - 1) << result;
    return oss.str();
}

int main3() {
    setlocale(LC_ALL, "ru");
    std::cout << "ÂÛ×ÈÑËÈÒÅËÜÍÛÅ ÝÊÑÏÅÐÈÌÅÍÒÛ ÄËß ÌÅÒÎÄÀ ßÊÎÁÈ (ËÅÂÎÅ ÂÐÀÙÅÍÈÅ)\n";
    std::cout << "================================================================\n\n";

    std::vector<int> dimensions = { 10, 30 };
    std::vector<std::pair<double, double>> lambdaRanges = { {-2.0, 2.0}, {-50.0, 50.0} };
    std::vector<double> epsilons = { 1e-5, 1e-7, 1e-9 };

    int numTrials = 10;
    int testNumber = 0;
    std::vector<TestResult> allResults;

    for (int N : dimensions) {
        for (auto& range : lambdaRanges) {
            for (double eps : epsilons) {
                testNumber++;
                std::cout << "Âûïîëíÿåòñÿ òåñò " << testNumber << ": N=" << N
                    << ", lambda: [" << range.first << "," << range.second << "]"
                    << ", eps=" << eps << "...\n";
                TestResult result = runExperiment(N, range.first, range.second, eps, numTrials);
                result.testNumber = testNumber;
                allResults.push_back(result);
            }
        }
    }

    std::cout << "\n\n";
    std::cout << "================================================================================================================\n";
    std::cout << "ÐÅÇÓËÜÒÀÒÛ ÒÅÑÒÈÐÎÂÀÍÈß ÌÅÒÎÄÀ ßÊÎÁÈ (ËÅÂÎÅ ÂÐÀÙÅÍÈÅ)\n";
    std::cout << "================================================================================================================\n";
    std::cout << std::left
        << std::setw(10) << "| ¹ òåñòà"
        << std::setw(25) << "| Ðàçìåðíîñòü N"
        << std::setw(30) << "| Äèàïàçîí çíà÷åíèé lambda"
        << std::setw(30) << "| Ìàêñ. Aij (i!=j)"
        << std::setw(20) << "| Ñð. èòåðàöèé"
        << std::setw(25) << "| Ñð. òî÷íîñòü lambda"
        << std::setw(20) << "| Ñð. ìåðà r |" << "\n";
    std::cout << "----------------------------------------------------------------------------------------------------------------\n";

    for (const auto& res : allResults) {
        std::cout << "| " << std::setw(8) << std::left << res.testNumber << " ";
        std::cout << "| " << std::setw(23) << std::left << res.N << " ";
        std::ostringstream rangeStream;
        rangeStream << "[" << res.lambda_min << ", " << res.lambda_max << "]";
        std::cout << "| " << std::setw(28) << std::left << rangeStream.str() << " ";
        std::cout << "| " << std::setw(28) << std::left << formatWithSignificantDigits(res.maxOffDiagFinal, 2) << " ";
        std::cout << "| " << std::setw(18) << std::left << std::fixed << std::setprecision(1) << res.avgIterations << " ";
        std::cout << "| " << std::setw(23) << std::left << formatWithSignificantDigits(res.avgEigenvalueError, 2) << " ";
        std::cout << "| " << std::setw(18) << std::left << formatWithSignificantDigits(res.avgAccuracy, 2) << " |\n";
    }

    std::cout << "================================================================================================================\n";

    return 0;
}