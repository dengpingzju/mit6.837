#ifndef _H_PLANE_
#define _H_PLANE_

#include "object3d.h"
class Plane : public Object3D {
public:
    Plane(Vec3f &normal, float d, Material *m);
    ~Plane() {}
    virtual bool intersect(const Ray &r, Hit &h, float tmin) const;
	virtual bool intersectShadowRay(const Ray &r, Hit &h, float tmin,float tmax) const;
	void paint();
	//plane doesn't have a boundingbox, so it does nothing to insert itself into a grid
	void insertIntoGrid(Grid *g, Matrix *m) {}
private:
    Vec3f normal;
    float distance;
};

#endif // _H_PLANE_
