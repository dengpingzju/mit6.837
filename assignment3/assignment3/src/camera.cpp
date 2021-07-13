#include "myutil.h"
#include <math.h>
#include <stdlib.h>


OrthographicCamera::OrthographicCamera(const Vec3f& _center,const Vec3f& _direction,const Vec3f& _up,float s) {
    size=s;
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
    ans=center+((-0.5+y)*size)*up+((-0.5+x)*size)*horizontal;
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
	/*
    this->angCenToEdge=asin(tan(this->angle/2));
    if (this->angCenToEdge<=0||this->angCenToEdge>=M_PI/2) {
        errCode=-1;//error unexpected
        return;
    }
	*/
    //float dist=0.5/tan(this->angCenToEdge);
	float dist = 0.5 / tan(this->angle/2);
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
//in assignment 3, update usrSrcOrigin when camera moves
//must first update direction and up and horizontal vectors
void PerspectiveCamera::updateUsrSrcOrigin() {
	float dist = 0.5 / tan(this->angle / 2);
	usrScrOrigin = this->center + (dist*this->direction) - 0.5*(this->up + this->horizontal);
}

//------------------added in assignment 3-------------------
// ====================================================================
// Below are the function bodies for
// each of these routines for the OrthographicCamera
// and PerspectiveCamera subclasses
// ====================================================================

// ====================================================================
// Create an orthographic camera with the appropriate dimensions that
// crops the screen in the narrowest dimension.
// ====================================================================

void OrthographicCamera::glInit(int w, int h)
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  if (w > h)
    glOrtho(-size/2.0, size/2.0, -size*(float)h/(float)w/2.0, size*(float)h/(float)w/2.0, 0.5, 40.0);
  else
    glOrtho(-size*(float)w/(float)h/2.0, size*(float)w/(float)h/2.0, -size/2.0, size/2.0, 0.5, 40.0);
}

// ====================================================================
// Place an orthographic camera within an OpenGL scene
// ====================================================================

void OrthographicCamera::glPlaceCamera(void)
{
  gluLookAt(center.x(), center.y(), center.z(),
            center.x()+direction.x(), center.y()+direction.y(), center.z()+direction.z(),
            up.x(), up.y(), up.z());
}

// ====================================================================
// dollyCamera, truckCamera, and RotateCamera
//
// Asumptions:
//  - up is really up (i.e., it hasn't been changed
//    to point to "screen up")
//  - up and direction are normalized
// Special considerations:
//  - If your constructor precomputes any vectors for
//    use in 'generateRay', you will likely to recompute those
//    values at athe end of the these three routines
// ====================================================================

// ====================================================================
// dollyCamera: Move camera along the direction vector
// ====================================================================

void OrthographicCamera::dollyCamera(float dist)
{
  center += direction*dist;

  // ===========================================
  // ASSIGNMENT 3: Fix any other affected values
  // ===========================================

  //no changes here
}

// ====================================================================
// truckCamera: Translate camera perpendicular to the direction vector
// ====================================================================

void OrthographicCamera::truckCamera(float dx, float dy)
{
  Vec3f horizontal;
  Vec3f::Cross3(horizontal, direction, up);
  horizontal.Normalize();

  Vec3f screenUp;
  Vec3f::Cross3(screenUp, horizontal, direction);

  center += horizontal*dx + screenUp*dy;

  // ===========================================
  // ASSIGNMENT 3: Fix any other affected values
  // ===========================================

  //no changes here
}

// ====================================================================
// rotateCamera: Rotate around the up and horizontal vectors
// ====================================================================

void OrthographicCamera::rotateCamera(float rx, float ry)
{
  Vec3f horizontal;
  Vec3f::Cross3(horizontal, direction, up);
  horizontal.Normalize();

  // Don't let the model flip upside-down (There is a singularity
  // at the poles when 'up' and 'direction' are aligned)
  float tiltAngle = acos(up.Dot3(direction));
  if (tiltAngle-ry > 3.13)
    ry = tiltAngle - 3.13;
  else if (tiltAngle-ry < 0.01)
    ry = tiltAngle - 0.01;

  Matrix rotMat = Matrix::MakeAxisRotation(up, rx);
  rotMat *= Matrix::MakeAxisRotation(horizontal, ry);

  rotMat.Transform(center);
  rotMat.TransformDirection(direction);

  // ===========================================
  // ASSIGNMENT 3: Fix any other affected values
  // ===========================================

  //changes required to adjust axes of camera
  Vec3f::Cross3(this->horizontal, direction, up);
  this->horizontal.Normalize();
  Vec3f::Cross3(up, this->horizontal, direction);
  up.Normalize();
}

// ====================================================================
// ====================================================================

// ====================================================================
// Create a perspective camera with the appropriate dimensions that
// crops or stretches in the x-dimension as necessary
// ====================================================================

void PerspectiveCamera::glInit(int w, int h)
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(angle*180.0/3.14159, (float)w/float(h), 0.5, 40.0);
}

// ====================================================================
// Place a perspective camera within an OpenGL scene
// ====================================================================

void PerspectiveCamera::glPlaceCamera(void)
{
  gluLookAt(center.x(), center.y(), center.z(),
            center.x()+direction.x(), center.y()+direction.y(), center.z()+direction.z(),
            up.x(), up.y(), up.z());
}

// ====================================================================
// dollyCamera, truckCamera, and RotateCamera
//
// Asumptions:
//  - up is really up (i.e., it hasn't been changed
//    to point to "screen up")
//  - up and direction are normalized
// Special considerations:
//  - If your constructor precomputes any vectors for
//    use in 'generateRay', you will likely to recompute those
//    values at athe end of the these three routines
// ====================================================================

// ====================================================================
// dollyCamera: Move camera along the direction vector
// ====================================================================

void PerspectiveCamera::dollyCamera(float dist)
{
  center += direction*dist;

  // ===========================================
  // ASSIGNMENT 3: Fix any other affected values
  // ===========================================
  updateUsrSrcOrigin();

}

// ====================================================================
// truckCamera: Translate camera perpendicular to the direction vector
// ====================================================================

void PerspectiveCamera::truckCamera(float dx, float dy)
{
  Vec3f horizontal;
  Vec3f::Cross3(horizontal, direction, up);
  horizontal.Normalize();

  Vec3f screenUp;
  Vec3f::Cross3(screenUp, horizontal, direction);

  center += horizontal*dx + screenUp*dy;

  // ===========================================
  // ASSIGNMENT 3: Fix any other affected values
  // ===========================================

  updateUsrSrcOrigin();
}

// ====================================================================
// rotateCamera: Rotate around the up and horizontal vectors
// ====================================================================

void PerspectiveCamera::rotateCamera(float rx, float ry)
{
  Vec3f horizontal;
  Vec3f::Cross3(horizontal, direction, up);
  horizontal.Normalize();

  // Don't let the model flip upside-down (There is a singularity
  // at the poles when 'up' and 'direction' are aligned)
  float tiltAngle = acos(up.Dot3(direction));
  if (tiltAngle-ry > 3.13)
    ry = tiltAngle - 3.13;
  else if (tiltAngle-ry < 0.01)
    ry = tiltAngle - 0.01;

  Matrix rotMat = Matrix::MakeAxisRotation(up, rx);
  rotMat *= Matrix::MakeAxisRotation(horizontal, ry);

  rotMat.Transform(center);
  rotMat.TransformDirection(direction);
  direction.Normalize();

  // ===========================================
  // ASSIGNMENT 3: Fix any other affected values
  // ===========================================

  //changes required to adjust axes of camera
  Vec3f::Cross3(this->horizontal, direction, up);
  this->horizontal.Normalize();
  Vec3f::Cross3(up, this->horizontal, direction);
  up.Normalize();
  updateUsrSrcOrigin();
}



