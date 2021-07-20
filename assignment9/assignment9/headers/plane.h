#ifndef _H_PLANE_
#define _H_PLANE_

#include "object3d.h"
class Plane : public Object3D {
public:
    Plane(Vec3f &normal, float d, Material *m);
    ~Plane() {}
    virtual bool intersect(const Ray &r, Hit &h, float tmin);
	void paint();
private:
    Vec3f normal;
    float distance;
};

#endif // _H_PLANE_
