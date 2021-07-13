#ifndef _SPHERE_H_
#define _SPHERE_H_

#include "object3d.h"
float const EPS=1e-9;
float const UNIT_EPS=1e-4;
class Sphere : public Object3D {
public:
    Sphere() {
        setPos(Vec3f(0,0,0));
        radius=1.0;
        material=0;
    }
    Sphere(const Vec3f& center,float r,Material* m) {
        rv=center;
        setRadius(r);
        material=m;
    }
    ~Sphere() {}
    void setPos(const Vec3f& p) {
        rv=p;
    }
    bool setRadius(float r) {
        if (r>EPS) {
            radius=r;
            return true;
        }
        else return false;
    }
    virtual bool intersect(const Ray &r, Hit &h, float tmin);
private:
    static bool checkRayDirection(const Ray& r);
    float radius;
    Vec3f rv;//position
};

#endif // _SPHERE_H_
