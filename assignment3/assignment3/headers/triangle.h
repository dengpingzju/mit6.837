#ifndef _H_TRIANGLE_
#define _H_TRIANGLE_

#include "object3d.h"
class Triangle : public Object3D {
public:
    Triangle(Vec3f &a, Vec3f &b, Vec3f &c, Material *m);
    ~Triangle() {}
    bool intersect(const Ray &r, Hit &h, float tmin);
	void paint();
private:
    Vec3f P,u,v;
    Vec3f normal;
};

#endif // _H_TRIANGLE_
