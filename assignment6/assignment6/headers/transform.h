#ifndef _H_TRANSFORM_
#define _H_TRANSFORM_

#include "object3d.h"
#include "matrix.h"
class Transform :public Object3D {
public:
    Transform(Matrix &m, Object3D *o);
    ~Transform() {
		delete pBoundingBox;
	}
    bool intersect(const Ray &r, Hit &h, float tmin) const;
	virtual bool intersectShadowRay(const Ray &r, Hit &h, float tmin,float tmax) const;
	void paint();
	void insertIntoGrid(Grid *g, Matrix *m);
private:
    Matrix mtrans,minv,minvT;
    Object3D* obj;
};

#endif // _H_TRANSFORM_
