#pragma once

#include <iostream>
#include <vector>
#include "PreciseDouble.h"

using namespace std;

class Vector {
private:
	vector<PreciseDouble> data;
	int shift;
public:
	explicit Vector(int n = 0, int shiftIndex = 0);

	int size() const;

	PreciseDouble& operator()(int i);

	const PreciseDouble& operator()(int i) const;

	void randomFill(double low, double high);

	Vector operator-(const Vector& other);

	Vector& operator=(const Vector& other);

	double norm();

	void print();

	void reverse();
};
