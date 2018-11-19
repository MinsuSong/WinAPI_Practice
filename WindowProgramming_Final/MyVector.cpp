#include "MyVector.h"

VECTOR operator*(const double n, const VECTOR& v) {	/* 스칼라곱  */
	return VECTOR(n*v.x, n*v.y);
}

double makeDegreeToRadian(int degree)
{
	/* 각도를 받아서 라디안으로 치환시켜준다. */
	return degree * PI / 180;
}

double getNorm(const VECTOR& start, const VECTOR& end)
{
	/* 시점과 종점을 통한 노름 구하기 혹시 모르니까 오버로딩 해놓자*/
	return sqrt((start.x - end.x)*(start.x - end.x) + (start.y - end.y)*(start.y - end.y));
}

double getNorm(const VECTOR& v)
{
	/* 요건 벡터 성분값을 통한 노름 구하기 */
	return sqrt(v.x*v.x + v.y* v.y);
}

VECTOR& normalize(VECTOR& v)
{
	/* 주어진 벡터를 정규화시켜 단위벡터(방향벡터용)로 만들어주자 */
	double vLength = getNorm(v);
	if (vLength != 0) {
		v.x /= vLength, v.y /= vLength;
	}
	return v;
}

double dotproduct(const VECTOR& u, const VECTOR& v, double theta)
{
	/* 내적값은 스칼라값이니까 double 반환하자, theta는 사잇각 */
	return getNorm(u)*getNorm(v)*cos(theta);
}

double dotproduct(const VECTOR& u, const VECTOR& v)
{
	/* 내적값은 스칼라값이니까 double 반환하자, theta는 사잇각 */
	return (u.x*v.x) + (u.y*v.y);
}

double getAngle(const VECTOR & u, const VECTOR & v)
{
	return acos(dotproduct(u, v));
}
