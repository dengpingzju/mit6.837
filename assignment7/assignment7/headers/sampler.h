#ifndef _H_SAMPLER_
#define _H_SAMPLER_

#include "vectors.h"
#include <time.h>
#include <math.h>

class MyRand {
public:
	MyRand() {
		x = time(NULL);
	}
	MyRand(unsigned seed) {
		x = seed;
	}
	unsigned rand() {
		x = (x * 997267) + 10007;
		return x;
	}
private:
	unsigned x;
};

class Sampler {
public:
	Sampler(int n) {
		this->n = n;
	}
	~Sampler() {}
	virtual Vec2f getSamplePosition(int n) = 0;
	int getNumOfSamples() const {
		return n;
	}
protected:
	int n;
};

class RandomSampler : public Sampler {
public:
	RandomSampler(int n) :Sampler(n) {}
	Vec2f getSamplePosition(int n);
private:
	MyRand mr;
};

class UniformSampler : public Sampler {
public:
	UniformSampler(int n):Sampler(n) {
		edgeNum = (int)round(sqrt(n));
	}
	Vec2f getSamplePosition(int n);
private:
	int edgeNum;
};

class JitteredSamples : public Sampler {
public:
	JitteredSamples(int n) :Sampler(n) {
		edgeNum = (int)round(sqrt(n));
		RandomSampler rs(n);
		shifts = new Vec2f[n];
		for (int i = 0; i < n; i++) {
			shifts[i] = rs.getSamplePosition(i);
		}
	}
	//no copy constructor
	~JitteredSamples() {
		delete[] shifts;
	}
	Vec2f getSamplePosition(int n);
private:
	int edgeNum;
	Vec2f *shifts;
};

#endif
