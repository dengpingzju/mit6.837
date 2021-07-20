#ifndef _H_FORCEFIELD_
#define _H_FORCEFIELD_

#include "vectors.h"
#include <time.h>

class MyRand {
public:
	MyRand() {
		x = (unsigned)time(NULL);
	}
	MyRand(unsigned seed) {
		x = seed;
	}
	unsigned rand() {
		x = (x * 997267) + 10007;
		return x;
	}
	double randDouble() {
		unsigned mask = (1U<<30)-1;
		return (double)(rand()&mask) / mask;
	}
private:
	unsigned x;
};

class ForceField {
public:
	ForceField() {}
	virtual ~ForceField() {}
	virtual Vec3f getAcceleration(const Vec3f &position, float mass, float t) const = 0;
};

class GravityForceField : public ForceField {
public:
	GravityForceField(Vec3f gravity) {
		this->gravity = gravity;
	}
	virtual Vec3f getAcceleration(const Vec3f &position, float mass, float t) const;
private:
	//the gravity should not be force but an acceleration, which is normally 9.8m/s^2
	Vec3f gravity;
};

class ConstantForceField : public ForceField {
public:
	ConstantForceField(Vec3f force) {
		this->force = force;
	}
	virtual Vec3f getAcceleration(const Vec3f &position, float mass, float t) const;
private:
	//the gravity should not be force but an acceleration, which is normally 9.8m/s^2
	Vec3f force;
};

class RadialForceField : public ForceField {
public:
	RadialForceField(float magnitude) {
		this->magnitude = magnitude;
	}
	virtual Vec3f getAcceleration(const Vec3f &position, float mass, float t) const;
private:
	float magnitude;
};

class VerticalForceField : public ForceField {
public:
	VerticalForceField(float magnitude) {
		this->magnitude = magnitude;
	}
	virtual Vec3f getAcceleration(const Vec3f &position, float mass, float t) const;
private:
	float magnitude;
};

class WindForceField : public ForceField {
public:
	WindForceField(float magnitude) {
		this->magnitude = magnitude;
	}
	virtual Vec3f getAcceleration(const Vec3f &position, float mass, float t) const;
private:
	float magnitude;
};
#endif // !_H_FORCEFIELD_

