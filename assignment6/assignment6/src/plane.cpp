#include "myutil.h"
#include <math.h>
Plane::Plane(Vec3f &normal, float d, Material *m) {
    this->normal=normal;
    this->distance=d;
    this->material=m;
	this->pBoundingBox = NULL;

	this->primitiveType = PT_PLANE;
}
bool Plane::intersect(const Ray &r, Hit &h, float tmin) const {
	RayTracingStats::IncrementNumIntersections();
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
bool Plane::intersectShadowRay(const Ray &r, Hit &h, float tmin,float tmax) const {
	if (intersect(r, h, tmin)) {
		return h.getT() < tmax;
	}
	else return false;
}
void Plane::paint() {
	Vec3f ocenter = normal * distance;
	Vec3f bas1(0.0f,0.0f,1.0f), bas2;
	Vec3f temp1;
	Vec3f::Cross3(temp1, bas1, normal);
	if (fabs(temp1.Length()) < EPS) bas1 = Vec3f(0, 1, 0);
	Vec3f::Cross3(bas1, bas1, normal);
	bas1.Normalize();
	Vec3f::Cross3(bas2, bas1, normal);
	bas2.Normalize();
	float w=1000;
	Vec3f pts[4];
	pts[0] = ocenter - bas1 * w - bas2 * w;
	pts[1] = ocenter + bas1 * w - bas2 * w;
	pts[2] = ocenter + bas1 * w + bas2 * w;
	pts[3] = ocenter - bas1 * w + bas2 * w;
	material->glSetMaterial();
	glBegin(GL_QUADS);
	glNormal3f(normal.x(), normal.y(), normal.z());
	for (int i = 0; i < 4; i++) {
		glVertex3f(pts[i].x(), pts[i].y(), pts[i].z());
	}
	glEnd();
}

