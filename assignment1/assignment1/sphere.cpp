#include "sphere.h"
#include "ray.h"
#include <cstdlib>
#include <cmath>
bool Sphere::intersect(const Ray &r, Hit &h, float tmin) {
    if (!checkRayDirection(r)) {
        cout<<"the direction of the ray does not meet requirements"<<endl;
        return false;
    }
    Vec3f R0=r.getOrigin()-rv;
    float b=2*r.getDirection().Dot3(R0);
    float c=R0.Dot3(R0)-radius*radius;
    float delta=b*b-4*c;
    if (delta<0) {
        return false;
    }
    else {
        float sq=sqrt(delta);
        float x1=(-b-sq)/2,x2=(-b+sq)/2;
        if (x1>=tmin) {
            h.set(x1,material,r);
            return true;
        }
        else if (x2>=tmin) {
            h.set(x2,material,r);
            return true;
        }
        else {
            return false;
        }
    }
}
bool Sphere::checkRayDirection(const Ray& r) {
    if (fabs(r.getDirection().Length()-1)>UNIT_EPS) return false;
    else return true;
}
