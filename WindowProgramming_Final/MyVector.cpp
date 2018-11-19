#include "MyVector.h"

VECTOR operator*(const double n, const VECTOR& v) {	/* ��Į���  */
	return VECTOR(n*v.x, n*v.y);
}

double makeDegreeToRadian(int degree)
{
	/* ������ �޾Ƽ� �������� ġȯ�����ش�. */
	return degree * PI / 180;
}

double getNorm(const VECTOR& start, const VECTOR& end)
{
	/* ������ ������ ���� �븧 ���ϱ� Ȥ�� �𸣴ϱ� �����ε� �س���*/
	return sqrt((start.x - end.x)*(start.x - end.x) + (start.y - end.y)*(start.y - end.y));
}

double getNorm(const VECTOR& v)
{
	/* ��� ���� ���а��� ���� �븧 ���ϱ� */
	return sqrt(v.x*v.x + v.y* v.y);
}

VECTOR& normalize(VECTOR& v)
{
	/* �־��� ���͸� ����ȭ���� ��������(���⺤�Ϳ�)�� ��������� */
	double vLength = getNorm(v);
	if (vLength != 0) {
		v.x /= vLength, v.y /= vLength;
	}
	return v;
}

double dotproduct(const VECTOR& u, const VECTOR& v, double theta)
{
	/* �������� ��Į���̴ϱ� double ��ȯ����, theta�� ���հ� */
	return getNorm(u)*getNorm(v)*cos(theta);
}

double dotproduct(const VECTOR& u, const VECTOR& v)
{
	/* �������� ��Į���̴ϱ� double ��ȯ����, theta�� ���հ� */
	return (u.x*v.x) + (u.y*v.y);
}

double getAngle(const VECTOR & u, const VECTOR & v)
{
	return acos(dotproduct(u, v));
}
