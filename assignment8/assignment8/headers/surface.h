#ifndef _H_SURFACE_
#define _H_SURFACE_

#include "spline.h"
#include "curve.h"

class Surface : public Spline {
public:
	virtual void Paint(ArgParser *args) {}
	// FOR CONVERTING BETWEEN SPLINE TYPES
	virtual void OutputBezier(FILE *file) {}
	virtual void OutputBSpline(FILE *file) {}

	// FOR CONTROL POINT PICKING
	virtual int getNumVertices() {
		return 0;
	}
	virtual Vec3f getVertex(int i) {
		return Vec3f();
	}
	// FOR EDITING OPERATIONS
	virtual void moveControlPoint(int selectedPoint, float x, float y) {}
	virtual void addControlPoint(int selectedPoint, float x, float y) {}
	virtual void deleteControlPoint(int selectedPoint) {}

	// FOR GENERATING TRIANGLES
	virtual TriangleMesh* OutputTriangles(ArgParser *args) = 0;
};

class SurfaceOfRevolution : public Surface {
public:
	SurfaceOfRevolution(Curve *curve) {
		this->curve = curve;
	}
	virtual TriangleMesh* OutputTriangles(ArgParser *args);
	virtual void Paint(ArgParser *args) {
		curve->Paint(args);
	}
	virtual void OutputBezier(FILE *file) {
		fprintf(file, "surface_of_revolution\n");
		curve->OutputBezier(file);
	}
	virtual void OutputBSpline(FILE *file) {
		fprintf(file, "surface_of_revolution\n");
		curve->OutputBSpline(file);
	}
	virtual int getNumVertices() {
		return curve->getNumVertices();
	}
	virtual Vec3f getVertex(int i) {
		return curve->getVertex(i);
	}

	// FOR EDITING OPERATIONS
	virtual void moveControlPoint(int selectedPoint, float x, float y) {
		curve->moveControlPoint(selectedPoint, x, y);
	}
	virtual void addControlPoint(int selectedPoint, float x, float y) {
		curve->addControlPoint(selectedPoint, x, y);
	}
	virtual void deleteControlPoint(int selectedPoint) {
		curve->deleteControlPoint(selectedPoint);
	}
private:
	Curve *curve;
};

class BezierPatch : public Surface {
public:
	BezierPatch() {
		bc = NULL;
	}
	virtual void set(int i, Vec3f v) {
		controlPts[i] = v;
	}
	virtual TriangleMesh* OutputTriangles(ArgParser *args);
	void initBc();
	virtual void Paint(ArgParser *args);
private:
	Vec3f controlPts[16];
	BezierCurve *bc;
};

#endif // !_H_SURFACE_

