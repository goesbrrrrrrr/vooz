#pragma once
#include "Matrix.h"
#include "Vector.h"

class SpecialMatrix {
public:
    SpecialMatrix(int size, int k);

    auto& getDiagonal() { return _diag; }
    auto& getKVector() { return _kVect; }

    int getNLeft() const { return _nLeft; }
    int getNRight() const { return _nRight; }

    Vector operator*(const Vector& v);
    Vector solve(Vector& d);
    void stepwiseEquivalenceControl(const Vector& x, const Vector& d_original);

private:
    Diagonal _diag;
    int _n;
    int _k;
    Vector _kVect;
    int _nLeft;
    int _nRight;
};