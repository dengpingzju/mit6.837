#ifndef _H_RAYTRACER_
#define _H_RAYTRACER_

#include "scene_parser.h"

class RayTracer {
public:
	RayTracer(SceneParser *s, int max_bounces,
		float cutoff_weight, bool shadows);
	//detect if the shadow ray intersect with some object
	virtual bool intersectShadowRay(const Ray &r, Hit &h, float tmin,float tmax) const;
	int shade(const Ray *ray, const Hit *hit, bool shadow, Vec3f &ret) const;
	static Vec3f mirrorDirection(const Vec3f &normal, const Vec3f &incoming);
	static bool transmittedDirection(const Vec3f &normal, const Vec3f &incoming,
		float index_i, float index_t, Vec3f &transmitted);
	Vec3f traceRay(Ray &ray, float tmin, int bounces, float weight,
		float indexOfRefraction, int role, Hit &hit) const;
	void traceToImage(Image& img) const;
private:
	SceneParser *pScene;
	int max_bounces;
	float cutoff_weight;
	bool shadows;
};

#endif // !_H_RAYTRACER_

