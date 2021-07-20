#include "forcefield.h"
#include "perlin_noise.h"
#include "myutil.h"

#define MY_OCTAVES 3

MyRand mr(2021);

Vec3f GravityForceField::getAcceleration(const Vec3f &position, float mass, float t) const {
	return gravity;
}

Vec3f ConstantForceField::getAcceleration(const Vec3f &position, float mass, float t) const {
	return this->force*(1/mass);
}

Vec3f VerticalForceField::getAcceleration(const Vec3f &position, float mass, float t) const {
	float sign;
	if (position.y() < 0) sign = 1;
	else sign = -1;
	Vec3f fdir(0, sign, 0);
	return (fabs(position.y())*this->magnitude)*fdir;
}

Vec3f RadialForceField::getAcceleration(const Vec3f &position, float mass, float t) const {
	float f= this->magnitude*(position.Length()/ mass);
	Vec3f fdir = position;
	fdir.Negate();
	fdir.Normalize();
	return fdir*f;
}

double getN(Vec3f v) {
	double di = 1;
	double sum = 0;
	for (int i = 0; i < MY_OCTAVES; i++) {
		sum += PerlinNoise::noise(v.x()*di, v.y()*di, v.z()*di) / di;
		di *= 2;
	}
	return sum;
}

Vec3f WindForceField::getAcceleration(const Vec3f &position, float mass, float t) const {
	Vec3f right(1, 0, 0), up(0, 1, 0);
	double x = getN(position),y= getN(position);
	Vec3f f = (1 - x)*right + x * up;
	float mag = magnitude/(6-position.y());
	if (mr.randDouble() < 0.2) f.Negate();
	f.Normalize();
	if (mr.randDouble() < 0.3) mag = magnitude;
	f *= mag;
	return f;
}
