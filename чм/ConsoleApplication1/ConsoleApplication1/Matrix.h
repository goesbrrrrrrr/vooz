#pragma once
#include "Vector.h"

struct Diagonal {
    Vector lower; // поддиагональ
    Vector main;  // главная диагональ
    Vector upper; // наддиагональ
    Diagonal(int n) : lower(n - 1, -1), main(n), upper(n - 1) {}
};


class TridiagonalMatrix {
private:
    int n;
    Diagonal _diag;
public:
    explicit TridiagonalMatrix(int n);

    int size() const;

    auto& getDiagonal() {
        return _diag;
    }

    Vector operator*(const Vector& v);

    //Метод прогонки
    Vector solve(const Vector& d);
};