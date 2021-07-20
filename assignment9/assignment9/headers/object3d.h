#ifndef _OBJECT3D_H_
#define _OBJECT3D_H_

class Ray;
class Hit;
class Material;

#define M_PI 3.14159265359
#define EPS (1e-9)
#define MAX_T (1e10)
#define MIN_T (-1e10)
#define UNIT_EPS (1e-4)

class Object3D {
public:
    Object3D() {material=0;}
    virtual ~Object3D() {}
    virtual bool intersect(const Ray &r, Hit &h, float tmin) = 0;
	virtual void paint() = 0;
protected:
    Material* material;
};
#endif // _OBJECT3D_H_
