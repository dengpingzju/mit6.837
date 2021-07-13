#ifndef _OBJECT3D_H_
#define _OBJECT3D_H_

#include <cstring>
#include "ray.h"
#include "hit.h"
#include "material.h"
#include "vectors.h"

float const MAX_T=1e10;
float const MIN_T=-MAX_T;
class Object3D {
public:
    Object3D() {material=0;}
    virtual ~Object3D() {}
    virtual bool intersect(const Ray &r, Hit &h, float tmin) = 0;
protected:
    Material* material;
};
#endif // _OBJECT3D_H_
