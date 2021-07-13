#include <cmath>
#include "camera.h"
#include "ray.h"
OrthographicCamera::OrthographicCamera(const Vec3f& _center,const Vec3f& _direction,const Vec3f& _up,float s) {
    sz=s;
    center=_center;
    Vec3f::Cross3(up,_direction,_up);
    Vec3f::Cross3(up,up,_direction);
    up.Normalize();
    direction=_direction;
    direction.Normalize();
    Vec3f::Cross3(horizontal,direction,up);
    horizontal.Normalize();
    errCode=0;
}
bool OrthographicCamera::getOriginCoordinate(Vec3f& ans,const Vec2f& scrCoord) const {
    float x,y;
    scrCoord.Get(x,y);
    if (x<0||x>1||y<0||y>1) {
        errCode=2;
        return false;//error
    }
    ans=center+((-0.5+y)*sz)*up+((-0.5+x)*sz)*horizontal;
    errCode=0;
    return true;
}
Ray OrthographicCamera::generateRay(Vec2f point) {
    Vec3f ori;
    getOriginCoordinate(ori,point);
    return Ray(ori,direction);
}
PerspectiveCamera::PerspectiveCamera(Vec3f &center,Vec3f &direction,Vec3f &up,float angle) {
    //possible to change parameters!!!
    direction.Normalize();
    up.Normalize();
    Vec3f h;
    Vec3f::Cross3(h,direction,up);
    if (h.Length()<0.9) {
        errCode=1;//initialization error:vector up and direction make a angle from away from 90 degrees
        return;
    }
    h.Normalize();
    Vec3f::Cross3(this->up,h,direction);
    this->up.Normalize();
    this->horizontal=h;
    this->direction=direction;
    this->center=center;

    this->angle=angle;
    this->angCenToEdge=asin(tan(this->angle/2));
    if (this->angCenToEdge<=0||this->angCenToEdge>=M_PI/2) {
        errCode=-1;//error unexpected
        return;
    }
    float dist=0.5/tan(this->angCenToEdge);
    usrScrOrigin=this->center+(dist*this->direction)-0.5*(this->up+this->horizontal);
    errCode=0;//all right
}
Ray PerspectiveCamera::generateRay(Vec2f point) {
    float x,y;
    point.Get(x,y);
    Vec3f scrPoint=this->usrScrOrigin+x*this->horizontal+y*this->up;
    Vec3f d=scrPoint-this->center;
    d.Normalize();
    return Ray(this->center,d);
}




