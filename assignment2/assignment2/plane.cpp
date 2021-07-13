#include "plane.h"
#include <cmath>
Plane::Plane(Vec3f &normal, float d, Material *m) {
    this->normal=normal;
    this->distance=d;
    this->material=m;
}
bool Plane::intersect(const Ray &r, Hit &h, float tmin) {
    float co=r.getDirection().Dot3(normal);
    if (fabs(co)<EPS) {
        //parallel or along the plane
        return false;
    }
    float t=(distance-normal.Dot3(r.getOrigin()))/co;
    if (t<tmin) return false;
    h.set(t,material,normal,r);
    return true;
}
