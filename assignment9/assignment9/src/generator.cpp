#include "generator.h"
#include "myutil.h"
float RingGenerator::max_ringR = 2.0f;
HoseGenerator::HoseGenerator(Vec3f position, float position_randomness,
	Vec3f velocity, float velocity_randomness) {

	this->position = position;
	this->position_randomness = position_randomness;
	this->velocity = velocity;
	this->velocity_randomness = velocity_randomness;
	mr = MyRand(202107);
}

int HoseGenerator::numNewParticles(float current_time, float dt) const {
	int ret=(int)(dt*desired_num_particles / lifespan);
	if (ret == 0) ret = 1;
	return ret;
}

Particle* HoseGenerator::Generate(float current_time, int i) {
	Vec3f rpos=randomizeVec(mr, position, position_randomness, -5, 5);
	rpos.Set(rpos.x(), rpos.y(), 0);
	Vec3f rv= randomizeVec(mr, velocity, velocity_randomness, -100, 100);//don't clamp velocity
	Vec3f rcol = randomizeVec(mr, color, color_randomness, 0, 1);
	float rmass = randomizeFloat(mr, mass, mass_randomness, 0, 1e5);
	float rlife= randomizeFloat(mr, lifespan, lifespan_randomness, 0, 1e5);
	Particle *p = new Particle(rpos, rv, rcol, dead_color, rmass, rlife);
	return p;
}

RingGenerator::RingGenerator(float position_randomness, Vec3f velocity,
	float velocity_randomness) {

	this->position = Vec3f(0,0,0);
	this->position_randomness = position_randomness;
	this->velocity = velocity;
	this->velocity_randomness = velocity_randomness;
	mr = MyRand(202107);
	ringR = 0.1;
	expandSpeed = 1;
	curTime = 0;

}

int RingGenerator::numNewParticles(float current_time, float dt) const {
	int ret=2 * M_PI*ringR*expandSpeed*dt*sigma;
	return ret;
	//return (int)(dt*desired_num_particles / lifespan);
}

Particle* RingGenerator::Generate(float current_time, int i) {
	float dt = current_time - curTime;
	curTime = current_time;
	float dr = dt * expandSpeed;
	ringR += dr;
	if (ringR > max_ringR) ringR = 0.1;
	float rtheta = mr.randDouble() * 2 * M_PI;
	float v[3];
	v[0] = ringR * cos(rtheta);
	v[1] = 0;
	v[2] = ringR * sin(rtheta);
	Vec3f rpos=randomizeVec(mr, Vec3f(v[0], v[1], v[2]), position_randomness, -5, 5);
	rpos.Set(rpos.x(), -4, rpos.z());
	Vec3f rv = randomizeVec(mr, velocity, velocity_randomness, -100, 100);//don't clamp velocity
	Vec3f rcol = randomizeVec(mr, color, color_randomness, 0, 1);
	float rmass = randomizeFloat(mr, mass, mass_randomness, 0, 1e5);
	float rlife = randomizeFloat(mr, lifespan, lifespan_randomness, 0, 1e5);
	Particle *p = new Particle(rpos, rv, rcol, dead_color, rmass, rlife);
	return p;
}
