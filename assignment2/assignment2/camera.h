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
    OrthographicCamera(const Vec3f& _center,const Vec3f& _direction,const Vec3f& _up,float s);
    ~OrthographicCamera() {}
    virtual Ray generateRay(Vec2f point);//point from (0,0) to (1,1)
    virtual float getTMin() const {
        return MIN_T;
    }
    int getErrCode() const {
        return errCode;
    }
private:
    float sz;
    mutable int errCode;//0:none,2:out of range
    Vec3f center,direction,up,horizontal;
    //translate the coordinate with respect to left-bottem,ranging from (0,0) to (1,1)
    //to the one in the world space
    bool getOriginCoordinate(Vec3f& ans,const Vec2f& scrCoord) const;
};
class PerspectiveCamera : public Camera {
public:
    PerspectiveCamera(Vec3f &center,Vec3f &direction,Vec3f &up,float angle);
    Ray generateRay(Vec2f point);
    float getTMin() const {
        return EPS;
    }
    int getErrCode() const {
        return errCode;
    }
    static double degToArc(double deg) {
        return deg/180.0*M_PI;
    }
private:
    Vec3f center,direction,up,horizontal;
    Vec3f usrScrOrigin;
    float angle,angCenToEdge;//angCenToEdge:angle from center to the edge
    int errCode;
};
#endif // _CAMERA_H_
