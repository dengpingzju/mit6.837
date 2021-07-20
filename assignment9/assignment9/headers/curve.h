#ifndef _H_CURVE_
#define _H_CURVE_

#include "vectors.h"
#include "spline.h"
#include "matrix.h"

class TriangleMesh;

class Curve : public Spline {
public:
	Curve() {}
	virtual int getNumVertices() {
		return numVertices;
	}
	virtual Vec3f getVertex(int i) {
		return vertices[i];
	}
	virtual void set(int i, Vec3f v) {
		vertices[i] = v;
	}
	virtual TriangleMesh* OutputTriangles(ArgParser *args) {
		return NULL;
	}
	void outputVertices(FILE *file);
	Matrix *getBezierB() {
		return &Bbezier;
	}
	Matrix *getSplineB() {
		return &Bspline;
	}
	static void init();
	virtual Vec3f getPointAtT(int k, float t) = 0;
	virtual int getCurveNum() =0;
protected:
	int numVertices;
	Vec3f *vertices;
	static Matrix Bspline, Bbezier;
	static Matrix invBspline, invBbezire;
};


class BezierCurve : public Curve {
public:
	//for simplicity, now only allow for adjusting position, adding or deleting control
	//points are not supported
	BezierCurve(int numVertices);
	virtual void Paint(ArgParser *args);
	virtual void OutputBezier(FILE *file);
	virtual void OutputBSpline(FILE *file);

	virtual void moveControlPoint(int selectedPoint, float x, float y);
	virtual void addControlPoint(int selectedPoint, float x, float y) {}
	virtual void deleteControlPoint(int selectedPoint) {}

	virtual Vec3f getPointAtT(int k, float t);
	virtual int getCurveNum();
};

class BSplineCurve : public Curve {
public:
	//for simplicity, now only allow for adjusting position, adding or deleting control
	//points are not supported
	BSplineCurve(int numVertices);
	virtual void Paint(ArgParser *args);
	virtual void OutputBezier(FILE *file);
	virtual void OutputBSpline(FILE *file);

	virtual void moveControlPoint(int selectedPoint, float x, float y);
	virtual void addControlPoint(int selectedPoint, float x, float y);
	virtual void deleteControlPoint(int selectedPoint);
	virtual Vec3f getPointAtT(int k, float t);
	virtual int getCurveNum();
};



#endif // !_H_CURVE_

