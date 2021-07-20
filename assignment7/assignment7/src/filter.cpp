#include "filter.h"
#include "myutil.h"

float BoxFilter::getWeight(float x, float y) {
	if (x>-radius&&x<=radius&&y>-radius&&y<=radius) return 1.0f;
	else return 0;
}
float TentFilter::getWeight(float x, float y) {
	float len = Vec2f(x, y).Length();
	if (len >= radius) return 0;
	else return 1 - len / radius;
	/*
	//separatable 2d tent filter
	x = my_fabsf(x);
	y = my_fabsf(y);
	float tmp1 = 1 - x / this->radius;
	float tmp2 = 1 - y / this->radius;
	if (tmp1 >= 0 && tmp2 >= 0) return tmp1 * tmp2;
	else return 0;
	*/
}
float GaussianFilter::getWeight(float x, float y) {
	float d2 = x * x + y * y;
	//if (d2 > 4 * sigma*sigma) return 0;
	//else return exp(-0.5*d2 / sigma / sigma);

	//don't clamp, return directly
	return exp(-0.5*d2 / sigma / sigma);
}
Vec3f Filter::getColor(int i, int j, Film *film) {
	int supR = getSupportRadius();
	int ns = film->getNumSamples();
	Vec3f totalColor;
	int cnt = 0;
	float sumOfWeights = 0;
	for (int i0 = i - supR; i0 <= i + supR; i0++) {
		for (int j0 = j - supR; j0 <= j + supR; j0++) {
			if (i0 < 0 || i0 >= film->getWidth()) continue;
			if (j0 < 0 || j0 >= film->getHeight()) continue;
			for (int k0 = 0; k0 < ns; k0++) {
				Vec2f vSamp(i0 - i - 0.5, j0 - j - 0.5);
				Sample samp = film->getSample(i0, j0, k0);
				vSamp += samp.getPosition();
				float w = getWeight(vSamp.x(), vSamp.y());
				totalColor+=w*samp.getColor();
				cnt += 1;
				sumOfWeights += w;
			}
		}
	}
	//return totalColor * Vec3f(1.0 / cnt, 1.0 / cnt, 1.0 / cnt);
	return totalColor * (1 / sumOfWeights);
}