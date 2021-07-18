#include "sampler.h"

Vec2f RandomSampler::getSamplePosition(int n) {
	//don't use n
	//first get random integers in [0,2^30-1]
	unsigned mask = (1U << 30) - 1;
	unsigned x = mr.rand()&mask;
	unsigned y = mr.rand()&mask;
	return Vec2f((float)x / mask, (float)y / mask);
}

Vec2f UniformSampler::getSamplePosition(int n) {
	int i = n % edgeNum, j = n / edgeNum;
	return Vec2f((i + 0.5) / edgeNum, (j + 0.5) / edgeNum);
}

Vec2f JitteredSamples::getSamplePosition(int n) {
	int i = n % edgeNum, j = n / edgeNum;
	Vec2f v = Vec2f(i, j);
	v+=shifts[i*edgeNum + j];
	v.Set(v.x() / edgeNum, v.y() / edgeNum);
	return v;
}