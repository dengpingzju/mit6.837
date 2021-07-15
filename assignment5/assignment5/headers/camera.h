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
    //added in assignment 3
    virtual void glInit(int w, int h) = 0;
    virtual void glPlaceCamera(void) = 0;
    virtual void dollyCamera(float dist) = 0;
    virtual void truckCamera(float dx, float dy) = 0;
    virtual void rotateCamera(float rx, float ry) = 0;
	virtual void debug() {}
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
    //added in assignment 3
    virtual void glInit(int w, int h);
    virtual void glPlaceCamera(void);
    virtual void dollyCamera(float dist);
    virtual void truckCamera(float dx, float dy);
    virtual void rotateCamera(float rx, float ry);

	void debug() {
		cout << center << endl << direction << endl << up << endl;
		cout << horizontal << endl;
	}
private:
    float size;
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
        return (float)EPS;
    }
    int getErrCode() const {
        return errCode;
    }
    static double degToArc(double deg) {
        return deg/180.0*M_PI;
    }
    //added in assignment 3
    virtual void glInit(int w, int h);
    virtual void glPlaceCamera(void);
    virtual void dollyCamera(float dist);
    virtual void truckCamera(float dx, float dy);
    virtual void rotateCamera(float rx, float ry);
	void updateUsrSrcOrigin();
	void debug() {
		cout << center << endl << direction << endl << up << endl;
		cout << horizontal << endl;
	}
private:
    Vec3f center,direction,up,horizontal;
    Vec3f usrScrOrigin;
    float angle,angCenToEdge;//angCenToEdge:angle from center to the edge
    int errCode;
};
#endif // _CAMERA_H_
