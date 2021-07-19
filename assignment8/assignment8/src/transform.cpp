#include "myutil.h"
Transform::Transform(Matrix &m, Object3D *o) {
    mtrans=m;
    obj=o;
    mtrans.Inverse(minv);
    minv.Transpose(minvT);
}
bool Transform::intersect(const Ray &r, Hit &h, float tmin) {
    Vec3f oinv(r.getOrigin());
    minv.Transform(oinv);
    Vec4f dinv(r.getDirection(),0);
    minv.Transform(dinv);
    Vec3f dinv3f(dinv[0],dinv[1],dinv[2]);
    dinv3f.Normalize();
    Ray rinv(oinv,dinv3f);
    if (!obj->intersect(rinv,h,tmin)) return false;
    Vec3f pt(h.getIntersectionPoint());
    mtrans.Transform(pt);
    float t=(pt-r.getOrigin()).Length();
    Vec4f ntr(h.getNormal(),0);
    minvT.Transform(ntr);
    Vec3f ntr3f(ntr.x(),ntr.y(),ntr.z());
    ntr3f.Normalize();
    h.set(t,h.getMaterial(),ntr3f,r);
    return true;
}

void Transform::paint() {
	glPushMatrix();
	GLfloat *glMatrix = mtrans.glGet();
	glMultMatrixf(glMatrix);
	obj->paint();
	glPopMatrix();
	delete[] glMatrix;
}
