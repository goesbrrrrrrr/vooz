#include <iostream>
#include <vector>
#include "Vector.h"

using namespace std;


Vector::Vector(int n, int shiftIndex) : data(n, 0.0), shift(shiftIndex) {}

int Vector::size() const {
    return static_cast<int>(data.size());
}

PreciseDouble& Vector::operator()(int i) {
    if (i < 1 - shift || i > size() - shift) throw out_of_range("Index out of range");
    return data[i - 1 + shift];
}

const PreciseDouble& Vector::operator()(int i) const {
    if (i < 1 - shift || i > size() - shift) throw out_of_range("Index out of range");
    return data[i - 1 + shift];
}

void Vector::randomFill(double low, double high) {
    for (int i = 1; i <= size(); ++i) {
        double r = (rand() % 1000) / 1000.0; 
        (*this)(i) = low + r * (high - low);
    }
}

Vector Vector::operator-(const Vector& other) {
    if (size() != other.size())
        throw out_of_range("Vector sizes do not match");
    Vector result(size());
    for (int i = 1; i <= size(); ++i) {
        result(i) = (*this)(i) - other(i);
    }
    return result;
}

Vector& Vector::operator=(const Vector& other) {
    data = other.data;
    return (*this);
}

double Vector::norm() {
    double max_val = 0;
    for (int i = 1; i <= size(); ++i) {
        max_val = max(max_val, abs((*this)(i)));
    }
    return max_val;
}

void Vector::print() {
    for (int i = 1; i <= size(); ++i) {
        cout << (*this)(i) << " ";
    }
    cout << "\n";
}

void Vector::reverse() {
    for (int i = 1; i <= size()/2; ++i) {
        std::swap((*this)(i), (*this)(size() - i + 1));
    }
}