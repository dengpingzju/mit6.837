#include "myutil.h"
#include <cstdlib>
#include <cmath>

extern int ntheta, nphi;

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
        Vec3f p;
        double ans;
        if (x1>=tmin) ans=x1;
        else if (x2>=tmin) ans=x2;
        else return false;
        p=r.pointAtParameter(ans);
        p-=rv;
        p.Normalize();
        h.set(ans,material,p,r);
        return true;
    }
}
bool Sphere::checkRayDirection(const Ray& r) {
    if (fabs(r.getDirection().Length()-1)>UNIT_EPS) return false;
    else return true;
}

void Sphere::getSphereCoord(const Vec2f &sph,Vec3f &coord) {
	//sph: (phi,theta)
	//phi: [-pi/2,pi/2], theta: [0,2*pi]
	float phi2=sph.x() + M_PI / 2;
	float projection = sin(phi2);
	float z = radius * cos(phi2);
	float x = radius * cos(sph.y())*projection;
	float y = radius * sin(sph.y())*projection;
	coord.Set(x, y, z);
	coord += rv;
}

void Sphere::paint() {
	material->glSetMaterial();
	glBegin(GL_QUADS);
	float dphi=M_PI / nphi;
	float dtheta = 2 * M_PI / ntheta;
	float iPhi, iTheta;
	for (iPhi =-M_PI/2+0.001; iPhi < M_PI / 2; iPhi += dphi) {
		for (iTheta = 0; iTheta < 2*M_PI; iTheta += dtheta) {
			Vec2f sph_pts[4];
			Vec3f coords[4];
			float dphi2, dtheta2;
			if (iPhi + dphi > M_PI / 2) {
				dphi2 = M_PI / 2 - iPhi;
			}
			else {
				dphi2 = dphi;
			}
			if (iTheta + dtheta > 2*M_PI) {
				dtheta2 = 2 * M_PI-iTheta;
			}
			else {
				dtheta2 = dtheta;
			}
			sph_pts[0] = Vec2f(iPhi, iTheta);
			sph_pts[1] = Vec2f(iPhi, iTheta+dtheta2);
			sph_pts[2] = Vec2f(iPhi+dphi2, iTheta + dtheta2);
			sph_pts[3] = Vec2f(iPhi + dphi2, iTheta);
			for (int i = 0; i < 4; i++) {
				getSphereCoord(sph_pts[i], coords[i]);
			}
			Vec3f v1 = coords[1] - coords[0];
			Vec3f v2 = coords[3] - coords[0];
			Vec3f normal;
			Vec3f::Cross3(normal, v2, v1);
			normal.Normalize();
			glNormal3f(normal.x(), normal.y(), normal.z());
			for (int i = 0; i < 4; i++) {
				glVertex3f(coords[i].x(), coords[i].y(), coords[i].z());
			}
		}
	}
	glEnd();
}
