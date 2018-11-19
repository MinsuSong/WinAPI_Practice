#pragma once
#include <math.h>

//	MATH DEFINES of mine! 
/* ����� ���� ��ġ�� */
#define PI					3.141592653589

struct VECTOR {
	double x, y;
	VECTOR() : x(0), y(0) {}
	VECTOR(int n) : x(static_cast<double>(n)), y(static_cast<double>(n)) {}
	VECTOR(double x, double y) : x(x), y(y) {}
	VECTOR(const VECTOR& v) : x(v.x), y(v.y) {}
	VECTOR& operator*(const double n) {		/* ��Į��� */
		x = n * x, y = n * y;
		return *this;
	}
	VECTOR& operator*=(const double n) {	/* ��Į��� �޼��� */
		x *= n, y *= n;
		return *this;
	}
};
VECTOR operator*(const double n, const VECTOR& v);

double makeDegreeToRadian(int degree);
double getNorm(const VECTOR& start, const VECTOR& end);
double getNorm(const VECTOR& v);
double dotproduct(const VECTOR& u, const VECTOR& v, double theta);
double dotproduct(const VECTOR& u, const VECTOR& v);
double getAngle(const VECTOR& u, const VECTOR& v);
