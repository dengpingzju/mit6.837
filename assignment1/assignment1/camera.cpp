#include "camera.h"
#include "ray.h"
Ray OrthographicCamera::generateRay(Vec2f point) {
    Vec3f ori;
    getOriginCoordinate(ori,point);
    return Ray(ori,direction);
}
