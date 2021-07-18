#include "myutil.h"

Transform::Transform(Matrix &m, Object3D *o) {
    mtrans=m;
    obj=o;
    mtrans.Inverse(minv);
    minv.Transpose(minvT);

	//calculate bounding box
	if (o->getPrimitiveType() == PT_TRIANGLE) {
		//special case for triangles to get tighter bounding boxes
		Triangle *tri = (Triangle*)o;
		Vec3f p, p2, p3;
		tri->getPuv(p, p2, p3);
		p2 = p + p2;
		p3 = p + p3;
		m.Transform(p);
		m.Transform(p2);
		m.Transform(p3);
		pBoundingBox = new BoundingBox(p, p);
		pBoundingBox->Extend(p2);
		pBoundingBox->Extend(p3);
	}
	else {
		//general code based on transforming the bounding box
		BoundingBox *pbb0 = o->getBoundingBox();
		Vec3f dia = pbb0->getMax() - pbb0->getMin();
		Vec3f edges[3];
		edges[0] = Vec3f(dia.x(), 0, 0);
		edges[1] = Vec3f(0, dia.y(), 0);
		edges[2] = Vec3f(0, 0, dia.z());
		Vec3f mvmin = pbb0->getMin();
		m.Transform(mvmin);
		pBoundingBox = new BoundingBox(mvmin, mvmin);
		for (int i = 1; i < 8; i++) {
			Vec3f vertex = pbb0->getMin();
			for (int j = 0; j < 3; j++) {
				if ((1 << j)&i) {
					vertex += edges[j];
				}
			}
			//here for simplicity I use vec3f to transform
			m.Transform(vertex);
			pBoundingBox->Extend(vertex);
		}
	}
}
bool Transform::intersect(const Ray &r, Hit &h, float tmin) const {
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
bool Transform::intersectShadowRay(const Ray &r, Hit &h, float tmin,float tmax) const {
	if (intersect(r, h, tmin)) {
		return h.getT() < tmax;
	}
	else return false;
}
void Transform::paint() {
	glPushMatrix();
	GLfloat *glMatrix = mtrans.glGet();
	glMultMatrixf(glMatrix);
	obj->paint();
	glPopMatrix();
	delete[] glMatrix;
}

void Transform::insertIntoGrid(Grid *g, Matrix *m) {
	Matrix mnew;
	if (!m) mnew = mtrans;
	else  mnew = (*m)*mtrans;
	obj->insertIntoGrid(g, &mnew);
}