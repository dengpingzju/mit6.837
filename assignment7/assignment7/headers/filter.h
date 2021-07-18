#ifndef _H_FILTER_
#define _H_FILTER_

#include "film.h"

class Filter {
public:
	Filter() {}
	virtual float getWeight(float x, float y) = 0;
	Vec3f getColor(int i, int j, Film *film);
	virtual int getSupportRadius() = 0;
};

class BoxFilter : public Filter {
public:
	BoxFilter(float r) {
		this->radius = r;
	}
	int getSupportRadius() {
		return (int)ceil(radius - 0.5);
	}
	float getWeight(float x, float y);
private:
	float radius;
};
class TentFilter : public Filter {
public:
	TentFilter(float r) {
		this->radius = r;
	}
	int getSupportRadius() {
		return (int)ceil(radius - 0.5);
	}
	float getWeight(float x, float y);
private:
	float radius;
};
class GaussianFilter : public Filter {
public:
	GaussianFilter(float sigma) {
		this->sigma = sigma;
	}
	int getSupportRadius() {
		return (int)ceil(2*sigma - 0.5);
	}
	float getWeight(float x, float y);
private:
	float sigma;
};

#endif // !_H_FILTER_

