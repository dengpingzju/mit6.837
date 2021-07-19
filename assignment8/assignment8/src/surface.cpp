#include "myutil.h"
#include "surface.h"
#include "curve.h"
#include "triangle_mesh.h"

TriangleMesh* SurfaceOfRevolution::OutputTriangles(ArgParser *args) {
	int tes = args->revolution_tessellation;
	int curTes = args->curve_tessellation;
	TriangleMesh *tm = NULL;
	double thetaD = 2 * M_PI / tes;
	double td = 1.0 / curTes;
	int seg = curve->getCurveNum();
	for (int k = 0; k < seg; k++) {
		double theta = 0;
		TriangleNet *tm_new = new TriangleNet(tes, curTes);
		for (int i = 0; i <= tes; i++) {
			double t = 0;
			Matrix rotY = Matrix::MakeYRotation(theta);
			for (int j = 0; j <= curTes; j++) {
				Vec3f v1 = curve->getPointAtT(k,t);
				rotY.Transform(v1);
				tm_new->SetVertex(i, j, v1);
				t += td;
			}
			theta += thetaD;
		}
		if (!tm) tm = tm_new;
		else {
			tm->Merge(*tm_new);
			delete tm_new;
		}
	}
	return tm;
}

void BezierPatch::initBc() {
	if (bc) return;
	bc = new BezierCurve[4]{ 4,4,4,4 };
	int cnt = 0;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			bc[i].set(j, controlPts[cnt++]);
		}
	}
}

TriangleMesh* BezierPatch::OutputTriangles(ArgParser *args) {
	int tes = args->patch_tessellation;
	double t = 0;
	double d = 1.0 / tes;
	TriangleNet *tn = new TriangleNet(tes, tes);
	for (int i = 0; i <= tes; i++) {
		double s = 0;
		Vec3f pts[4];
		for (int k = 0; k < 4; k++) {
			pts[k] = bc[k].getPointAtT(0, t);
		}
		BezierCurve new_bc(4);
		for (int k = 0; k < 4; k++) {
			new_bc.set(k, pts[k]);
		}
		for (int j = 0; j <= tes; j++) {
			Vec3f v = new_bc.getPointAtT(0,s);
			tn->SetVertex(i, j, v);
			s += d;
		}
		t += d;
	}
	return tn;
}
void BezierPatch::Paint(ArgParser *args) {
	TriangleMesh *tm = OutputTriangles(args);
	tm->paint();
	delete tm;
}