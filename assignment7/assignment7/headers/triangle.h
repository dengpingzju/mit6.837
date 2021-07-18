#ifndef _H_TRIANGLE_
#define _H_TRIANGLE_

#include "object3d.h"
class Triangle : public Object3D {
public:
    Triangle(Vec3f &a, Vec3f &b, Vec3f &c, Material *m);
    ~Triangle() {
		delete pBoundingBox;
	}
    bool intersect(const Ray &r, Hit &h, float tmin) const ;
	bool intersectShadowRay(const Ray &r, Hit &h, float tmin,float tmax) const;
	void paint();
	void insertIntoGrid(Grid *g, Matrix *m);

	//added in assignment 5
	
	//not virtual methods, need type conversion to Triangle class to access the method
	void getPuv(Vec3f& p2,Vec3f& u2,Vec3f& v2) {
		p2 = P;
		u2 = u;
		v2 = v;
	}
private:
    Vec3f P,u,v;
    Vec3f normal;
};

#endif // _H_TRIANGLE_
