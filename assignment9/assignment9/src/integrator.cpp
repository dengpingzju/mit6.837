#include "myutil.h"
void EulerIntegrator::Update(Particle *particle, ForceField *forcefield, float t, float dt) {
	//if dead, don't update any more
	if (particle->isDead()) return;
	Vec3f vel = particle->getVelocity();
	Vec3f pos = particle->getPosition();
	particle->setPosition(pos + vel * dt);
	Vec3f acc = forcefield->getAcceleration(pos, particle->getMass(), t);
	particle->setVelocity(vel + acc * dt);
	//must be alive, so update it safely
	particle->increaseAge(dt);
}

void MidpointIntegrator::Update(Particle *particle, ForceField *forcefield, float t, float dt) {
	//if dead, don't update any more
	if (particle->isDead()) return;
	Vec3f vel = particle->getVelocity();
	Vec3f pos = particle->getPosition();
	Vec3f acc = forcefield->getAcceleration(pos, particle->getMass(), t);
	Vec3f pm = pos + vel * (dt / 2);
	Vec3f vm = vel + acc * (dt / 2);
	Vec3f accm = forcefield->getAcceleration(pm, particle->getMass(), t);
	particle->setPosition(pos + vm * dt);
	particle->setVelocity(vel + accm * dt);
	//must be alive, so update it safely
	particle->increaseAge(dt);
}

void Trapezoid::Update(Particle *particle, ForceField *forcefield, float t, float dt) {
	//if dead, don't update any more
	if (particle->isDead()) return;
	Vec3f vel = particle->getVelocity();
	Vec3f pos = particle->getPosition();
	Vec3f acc = forcefield->getAcceleration(pos, particle->getMass(), t);
	Vec3f pm = pos + vel * dt;
	Vec3f vm = vel + acc * dt;
	Vec3f accm = forcefield->getAcceleration(pm, particle->getMass(), t);
	particle->setPosition(pos + (vel + vm)*(dt / 2));
	particle->setVelocity(vel + (acc + accm)*(dt / 2));
	//must be alive, so update it safely
	particle->increaseAge(dt);
}

//https://en.wikipedia.org/wiki/Runge%E2%80%93Kutta_methods
//implementation according to formulas in wikipedia
void RungeKuttaIntegrator::Update(Particle *particle, ForceField *forcefield, float t, float dt) {
	//if dead, don't update any more
	if (particle->isDead()) return;
	Vec3f vel = particle->getVelocity();
	Vec3f pos = particle->getPosition();
	Vec3f acc = forcefield->getAcceleration(pos, particle->getMass(), t);
	//k1: vel,acc
	//k2
	Vec3f p2 = pos + vel * (dt / 2);
	Vec3f v2 = vel + acc * (dt / 2);
	Vec3f acc2 = forcefield->getAcceleration(p2, particle->getMass(), t);
	//k3
	Vec3f p3 = pos + v2 * (dt / 2);
	Vec3f v3 = vel + acc2 * (dt / 2);
	Vec3f acc3 = forcefield->getAcceleration(p3, particle->getMass(), t);
	//k4
	Vec3f p4 = pos + v3 * dt;
	Vec3f v4 = vel + acc3 * dt;
	Vec3f acc4 = forcefield->getAcceleration(p4, particle->getMass(), t);

	Vec3f vtotaldt = (dt / 6)*(vel + 2 * v2 + 2 * v3 + v4);
	Vec3f atotaldt = (dt / 6)*(acc + 2 * acc2 + 2 * acc3 + acc4);

	particle->setPosition(pos + vtotaldt);
	particle->setVelocity(vel + atotaldt);
	//must be alive, so update it safely
	particle->increaseAge(dt);
}
