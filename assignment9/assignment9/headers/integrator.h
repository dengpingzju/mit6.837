#ifndef _H_INTEGRATOR_
#define _H_INTEGRATOR_

#include "vectors.h"

class Particle;
class ForceField;

class Integrator {
public:
	virtual Vec3f getColor() const =0;
	virtual void Update(Particle *particle, ForceField *forcefield, float t, float dt) = 0;
};

class EulerIntegrator : public Integrator {
public:
	virtual Vec3f getColor() const {
		return Vec3f(1, 0, 0);//red
	}
	virtual void Update(Particle *particle, ForceField *forcefield, float t, float dt);
};

class MidpointIntegrator : public Integrator {
public:
	virtual Vec3f getColor() const {
		return Vec3f(0, 1, 0);//green
	}
	virtual void Update(Particle *particle, ForceField *forcefield, float t, float dt);
};

class Trapezoid : public Integrator {
public:
	virtual Vec3f getColor() const {
		return Vec3f(0, 0, 1);//blue
	}
	virtual void Update(Particle *particle, ForceField *forcefield, float t, float dt);
};

class RungeKuttaIntegrator : public Integrator {
public:
	virtual Vec3f getColor() const {
		return Vec3f(1, 1, 0);//some different color
	}
	virtual void Update(Particle *particle, ForceField *forcefield, float t, float dt);
};

#endif // !_H_INTEGRATOR_

