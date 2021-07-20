#include "myutil.h"
Triangle::Triangle(Vec3f &a, Vec3f &b, Vec3f &c, Material *m) {
    P=a;
    u=b-a;
    v=c-a;
    Vec3f::Cross3(normal,u,v);
    normal.Normalize();
    material=m;
	pBoundingBox = new BoundingBox(a, a);
	pBoundingBox->Extend(b);
	pBoundingBox->Extend(c);

	//set primitiveType to distinguish it from other primitives
	primitiveType = PT_TRIANGLE;
}
bool Triangle::intersect(const Ray &r, Hit &h, float tmin) const {
	RayTracingStats::IncrementNumIntersections();
    float m[3][4];//matrix of equations
    Vec3f d=r.getDirection();
    Vec3f c=r.getOrigin()-P;
    for (int i=0;i<3;i++) {
        m[i][0]=u[i];
        m[i][1]=v[i];
        m[i][2]=-d[i];
        m[i][3]=c[i];
    }
    //solve the equation with three unknowns

    for (int i=0;i<3;i++) {
        int k=i;
        for (int j=i+1;j<3;j++) {
            if (my_fabsf(m[j][i])>my_fabsf(m[k][i])) k=j;
        }
        if (my_fabsf(m[k][i])<EPS) return false;
        if (k!=i) {
            for (int j=0;j<4;j++) {
                float tmp=m[i][j];
                m[i][j]=m[k][j];
                m[k][j]=tmp;
            }
        }
        for (int j=0;j<3;j++) {
            if (i==j) continue;
            float co=-m[j][i]/m[i][i];
            for (int k=i;k<4;k++) {
                m[j][k]=co*m[i][k]+m[j][k];
            }
        }
    }
    for (int i=0;i<3;i++) {
        m[i][3]=m[i][3]/m[i][i];
    }
    if (m[2][3]>=tmin&&m[0][3]>=0&&m[0][3]<=1&&m[1][3]>=0&&m[1][3]<=1&&(m[0][3]+m[1][3]<=1)) {
        h.set(m[2][3],material,normal,r);
        return true;
    }
    else return false;
}
bool Triangle::intersectShadowRay(const Ray &r, Hit &h, float tmin,float tmax) const {
	if (intersect(r, h, tmin)) {
		return h.getT() < tmax;
	}
	else return false;
}
void Triangle::paint() {
	Vec3f p2 = P + u, p3 = P + v;
	material->glSetMaterial();
	glBegin(GL_TRIANGLES);
	glNormal3f(normal.x(), normal.y(), normal.z());
	glVertex3f(P.x(), P.y(), P.z());
	glVertex3f(p2.x(), p2.y(), p2.z());
	glVertex3f(p3.x(), p3.y(), p3.z());
	glEnd();
}

void Triangle::insertIntoGrid(Grid *g, Matrix *m) {
	//triangle-rasterization seems quite complicated,
    //and requires a lot of computing

	//now simplify the task to judge by distance to plane

	//if m not NULL, insert the boundingbox directly
	if (m) {
		this->Object3D::insertIntoGrid(g, m);
		return;
	}
	BoundingBox *pb = getBoundingBox();
	Vec3f pmin = pb->getMin();
	Vec3f pmax = pb->getMax();
	int imin, jmin, kmin;
	int imax, jmax, kmax;
	g->getIndexFromCoord(pmin, imin, jmin, kmin);
	g->getIndexFromCoord(pmax, imax, jmax, kmax);

	int nx, ny, nz;
	g->getGridSize(nx, ny, nz);

	imin = max2(imin, 0);
	jmin = max2(jmin, 0);
	kmin = max2(kmin, 0);

	imax = min2(nx - 1, imax);
	jmax = min2(ny - 1, jmax);
	kmax = min2(nz - 1, kmax);

	Vec3f minPos = g->getBoundingBox()->getMin();
	Vec3f cellDia = g->getCellDia();
	float centerToVertex = cellDia.Length() / 2;
	float tmp1, tmp2, tmp3;
	tmp1 = (imin + 0.5)*cellDia.x() + minPos.x();
	tmp2 = (jmin + 0.5)*cellDia.y() + minPos.y();
	tmp3 = (kmin + 0.5)*cellDia.z() + minPos.z();
	Vec3f center(tmp1, tmp2, tmp3);
	for (int i = imin; i <= imax; i++) {
		for (int j = jmin; j <= jmax; j++) {
			for (int k = kmin; k <= kmax; k++) {
				float d = (center - P).Dot3(normal);
				if (d < 0) d = -d;
				if (d < centerToVertex) {
					PobjVec *cellObj = g->getCellObj(i, j, k);
					cellObj->push_back(this);
				}
				center.Set(center.x(), center.y(), center.z() + cellDia.z());
			}
			center.Set(center.x(), center.y() + cellDia.y(), tmp3);
		}
		center.Set(center.x() + cellDia.x(), tmp2, tmp3);
	}
}
