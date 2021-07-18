#ifndef _OBJECT3D_H_
#define _OBJECT3D_H_

#include <vector>

class Ray;
class Hit;
class Material;
class BoundingBox;
class Grid;
class Matrix;

#define M_PI 3.14159265359
#define EPS (1e-9)
#define MAX_T (1e10)
#define MIN_T (-1e10)
#define UNIT_EPS (1e-4)

enum PrimitiveType {
	PT_NOTSET,
	PT_TRIANGLE,
	PT_PLANE,
};

class Object3D {
public:
    Object3D() {
		material=0;
		primitiveType = PT_NOTSET;//not set yet
	}
    virtual ~Object3D() {}
    virtual bool intersect(const Ray &r, Hit &h, float tmin) const = 0;
	virtual bool intersectShadowRay(const Ray &r, Hit &h, float tmin,float tmax) const = 0;
	virtual void paint() = 0;

	//added in assignment5
	BoundingBox* getBoundingBox() const {
		return pBoundingBox;
	}
	virtual void insertIntoGrid(Grid *g, Matrix *m);
	int getPrimitiveType() const {
		return primitiveType;
	}
protected:
    Material* material;

	//added in assignment5
	BoundingBox* pBoundingBox;
	int primitiveType;//set when needing
};

typedef std::vector<Object3D*> PobjVec;
#endif // _OBJECT3D_H_
