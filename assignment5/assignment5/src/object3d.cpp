#include "myutil.h"
void Object3D::insertIntoGrid(Grid *g, Matrix *m) {
	Transform tf(*m, this);
	BoundingBox *pb = tf.getBoundingBox();
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
				PobjVec *cellObj = g->getCellObj(i, j, k);
				cellObj->push_back(this);
				center.Set(center.x(), center.y(), center.z() + cellDia.z());
			}
			center.Set(center.x(), center.y() + cellDia.y(), tmp3);
		}
		center.Set(center.x() + cellDia.x(), tmp2, tmp3);
	}
}
