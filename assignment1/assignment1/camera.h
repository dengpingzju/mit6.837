#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "vectors.h"
#include "object3d.h"
class Camera {
public:
    Camera() {}
    virtual ~Camera() {}
    virtual Ray generateRay(Vec2f point) = 0;
    virtual float getTMin() const = 0;
    virtual int getErrCode() const = 0;
};
class OrthographicCamera : public Camera {
public:
    OrthographicCamera(const Vec3f& _center,const Vec3f& _direction,const Vec3f& _up,float s) {
        sz=s;
        center=_center;
        Vec3f::Cross3(up,_direction,_up);
        Vec3f::Cross3(up,up,_direction);
        up.Normalize();
        direction=_direction;
        direction.Normalize();
        Vec3f::Cross3(horizental,direction,up);
        horizental.Normalize();
        errCode=0;
    }
    ~OrthographicCamera() {}
    virtual Ray generateRay(Vec2f point);
    virtual float getTMin() const {
        return MIN_T;
    }
    int getErrCode() const {
        return errCode;
    }
private:
    float sz;
    mutable int errCode;//0:none,2:out of range
    Vec3f center,direction,up,horizental;
    bool getOriginCoordinate(Vec3f& ans,const Vec2f& scrCoord) const {
        float x,y;
        scrCoord.Get(x,y);
        if (x<0||x>1||y<0||y>1) {
            errCode=2;
            return false;//error
        }
        ans=center+((-0.5+y)*sz)*up+((-0.5+x)*sz)*horizental;
        errCode=0;
        return true;
    }
};
#endif // _CAMERA_H_
