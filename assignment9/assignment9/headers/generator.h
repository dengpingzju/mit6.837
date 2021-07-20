#ifndef _H_GENERATOR_
#define _H_GENERATOR_

#include "vectors.h"
#include "object3d.h"
#include "forcefield.h"

class Particle;

class Generator {
public:
	// initialization
	virtual void SetColors(Vec3f color, Vec3f dead_color, float color_randomness) {
		this->color = color;
		this->dead_color = dead_color;
		this->color_randomness = color_randomness;
	}
	virtual void SetLifespan(float lifespan, float lifespan_randomness, int desired_num_particles) {
		this->lifespan = lifespan;
		this->lifespan_randomness = lifespan_randomness;
		this->desired_num_particles = desired_num_particles;
	}
	virtual void SetMass(float mass, float mass_randomness) {
		this->mass = mass;
		this->mass_randomness = mass_randomness;
	}

	// on each timestep, create some particles
	virtual int numNewParticles(float current_time, float dt) const =0;
	virtual Particle* Generate(float current_time, int i) =0;

	// for the gui
	virtual void Paint() const {};
	virtual void Restart() =0;

	static Vec3f randomizeVec(MyRand &mr, Vec3f v, float randomness, float low, float high) {
		double r;
		float tmp[3];
		for (int i = 0; i < 3; i++) {
			r = mr.randDouble();
			tmp[i] = v[i] + (r - 0.5)*randomness*2;
			if (tmp[i] < low) tmp[i] = low;
			else if (tmp[i] > high) tmp[i] = high;
		}
		return Vec3f(tmp[0], tmp[1], tmp[2]);
	}

	static float randomizeFloat(MyRand &mr, float x, float randomness, float low, float high) {
		double r;
		r = mr.randDouble();
		double tmp = x + (r - 0.5)*randomness*2;
		if (tmp < low) tmp = low;
		else if (tmp > high) tmp = high;
		return (float)tmp;
	}
protected:
	Vec3f color, dead_color;
	float color_randomness;
	float lifespan, lifespan_randomness;
	float mass, mass_randomness;
	int desired_num_particles;
	MyRand mr;
};



class HoseGenerator : public Generator {
public:
	HoseGenerator(Vec3f position, float position_randomness, Vec3f velocity, float velocity_randomness);

	// on each timestep, create some particles
	virtual int numNewParticles(float current_time, float dt) const;
	virtual Particle* Generate(float current_time, int i);

	// for the gui
	//virtual void Paint() const {};
	virtual void Restart() {
		mr = MyRand(202107);
	}
private:
	Vec3f position;
	float position_randomness;
	Vec3f velocity;
	float velocity_randomness;
};

class RingGenerator : public Generator {
public:
	RingGenerator(float position_randomness, Vec3f velocity, float velocity_randomness);

	// on each timestep, create some particles
	virtual int numNewParticles(float current_time, float dt) const;
	virtual Particle* Generate(float current_time, int i);

	virtual void SetLifespan(float lifespan, float lifespan_randomness, int desired_num_particles) {
		this->lifespan = lifespan;
		this->lifespan_randomness = lifespan_randomness;
		this->desired_num_particles = desired_num_particles;
		sigma = desired_num_particles / M_PI / max_ringR / max_ringR;
	}

	// for the gui
	//virtual void Paint() const {};
	virtual void Restart() {
		mr = MyRand(202107);
	}
private:
	Vec3f position;
	float position_randomness;
	Vec3f velocity;
	float velocity_randomness;
	float expandSpeed;
	float ringR;
	float curTime;
	float sigma;
	static float max_ringR;
};

#endif // !_H_GENERATOR_

