#define _CRT_SECURE_NO_WARNINGS
#include "curve.h"
#include "arg_parser.h"
#include "myutil.h"

#define MY_CURVE_PTS_SIZE 3
#define MY_HULL_WIDTH 1
#define MY_CONTROL_PTS_SIZE 7

Matrix Curve::Bspline;
Matrix Curve::Bbezier;
Matrix Curve::invBspline;
Matrix Curve::invBbezire;

void Curve::outputVertices(FILE *file) {
	fprintf(file, "num_vertices %d\n", numVertices);
	for (int i = 0; i < numVertices; i++) {
		fprintf(file, "%f %f %f\n", vertices[i].x(), vertices[i].y(), vertices[i].z());
	}
}
void Curve::init() {
	float bezier[16] = { -1,3,-3,1,3,-6,3,0,-3,3,0,0,1,0,0,0 };
	Bbezier = Matrix(bezier);
	Bbezier.Inverse(invBbezire);
	float spline[16] = { -1,3,-3,1,3,-6,0,4,-3,3,3,1,1,0,0,0 };
	Bspline = Matrix(spline);
	Bspline *= 1.0 / 6;
	Bspline.Inverse(invBspline);
}
BezierCurve::BezierCurve(int numVertices) {
	if (numVertices % 3 != 1) {
		//not 3*n+1 control points, error
		std::cerr << "Not 3*n+1 control points for BezierCurve" << endl;
		this->numVertices = 0;
		vertices = NULL;
	}
	vertices = new Vec3f[numVertices];
	this->numVertices = numVertices;
}
void BezierCurve::Paint(ArgParser *args) {
	int tes = args->curve_tessellation;
	float seg = 1.0 / tes;
	int segNum = (numVertices - 1) / 3;
	glLineWidth(MY_CURVE_PTS_SIZE);//points on curve
	glColor3f(0, 1, 0);
	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < segNum; i++) {
		float t = 0;
		for (int j = 0; j <= tes; j++) {
			Vec3f curvePoint = getPointAtT(i, t);
			glVertex3f(curvePoint.x(), curvePoint.y(), curvePoint.z());
			t += seg;
		}
	}
	glEnd();
	glPointSize(MY_CONTROL_PTS_SIZE);//control points
	glColor3f(1, 0, 0);
	glBegin(GL_POINTS);
	for (int i = 0; i < numVertices; i++) {
		glVertex3f(vertices[i].x(), vertices[i].y(), vertices[i].z());
	}
	glEnd();
	glLineWidth(MY_HULL_WIDTH);//draw lines connecting control points
	glColor3f(0, 0, 1);
	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < numVertices; i++) {
		glVertex3f(vertices[i].x(), vertices[i].y(), vertices[i].z());
	}
	glEnd();
}
Vec3f BezierCurve::getPointAtT(int k, float t) {
	int p = 3 * k;
	if (p + 4 > numVertices) {
		std::cerr << "Access curves not exsisting" << endl;
		return Vec3f();
	}
	Vec4f T(t*t*t, t*t, t, 1);
	getBezierB()->Transform(T);
	Vec3f ans;
	for (int i = 0; i < 4; i++) {
		ans += T[i] * vertices[p + i];
	}
	return ans;
}
int BezierCurve::getCurveNum() {
	return (numVertices - 1) / 3;
}
void BezierCurve::OutputBezier(FILE *file) {
	fprintf(file, "bezier\n");
	outputVertices(file);
}
void BezierCurve::OutputBSpline(FILE *file) {
	if (numVertices != 4) {
		std::cerr << "Conversion can be conducted only for those having 4 control points." << endl;
		return;
	}
	fprintf(file, "bspline\n");
	fprintf(file, "num_vertices 4\n");
	Matrix m = Bbezier * invBspline;
	float gbezier[16] = { 0.0f };
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 3; j++) {
			gbezier[i * 4 + j] = vertices[i][j];
		}
	}
	Matrix G(gbezier);
	G.Transpose();
	G = G * m;
	for (int i = 0; i < 4; i++) {
		float tmp[3];
		for (int j = 0; j < 3; j++) {
			tmp[j] = G.Get(i, j);
		}
		fprintf(file, "%f %f %f\n", tmp[0], tmp[1], tmp[2]);
	}
}

void BezierCurve::moveControlPoint(int selectedPoint, float x, float y) {
	float z = vertices[selectedPoint].z();
	vertices[selectedPoint].Set(x, y, z);
}

BSplineCurve::BSplineCurve(int numVertices) {
	vertices = new Vec3f[numVertices];
	this->numVertices = numVertices;
}
void BSplineCurve::Paint(ArgParser *args) {
	int tes = args->curve_tessellation;
	float seg = 1.0 / tes;
	int segNum = numVertices - 3;
	glLineWidth(MY_CURVE_PTS_SIZE);//points on curve
	glColor3f(0, 1, 0);
	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < segNum; i++) {
		float t = 0;
		for (int j = 0; j <= tes; j++) {
			Vec3f curvePoint = getPointAtT(i, t);
			glVertex3f(curvePoint.x(), curvePoint.y(), curvePoint.z());
			t += seg;
		}
	}
	glEnd();
	glPointSize(MY_CONTROL_PTS_SIZE);//control points
	glColor3f(1, 0, 0);
	glBegin(GL_POINTS);
	for (int i = 0; i < numVertices; i++) {
		glVertex3f(vertices[i].x(), vertices[i].y(), vertices[i].z());
	}
	glEnd();
	glLineWidth(MY_HULL_WIDTH);//draw lines connecting control points
	glColor3f(0, 0, 1);
	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < numVertices; i++) {
		glVertex3f(vertices[i].x(), vertices[i].y(), vertices[i].z());
	}
	glEnd();
}
Vec3f BSplineCurve::getPointAtT(int k, float t) {
	if (k + 4 > numVertices) {
		std::cerr << "Access curves not exsisting" << endl;
		return Vec3f();
	}
	Vec4f T(t*t*t, t*t, t, 1);
	getSplineB()->Transform(T);
	Vec3f ans;
	for (int i = 0; i < 4; i++) {
		ans += T[i] * vertices[k + i];
	}
	return ans;
}

int BSplineCurve::getCurveNum() {
	return numVertices - 3;
}

void BSplineCurve::OutputBezier(FILE *file) {
	if (numVertices != 4) {
		std::cerr << "Conversion can be conducted only for those having 4 control points." << endl;
		return;
	}
	fprintf(file, "bezier\n");
	fprintf(file, "num_vertices 4\n");
	Matrix m = Bspline * invBbezire;
	float gspline[16] = { 0.0f };
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 3; j++) {
			gspline[i * 4 + j] = vertices[i][j];
		}
	}
	Matrix G(gspline);
	G.Transpose();
	G = G * m;
	for (int i = 0; i < 4; i++) {
		float tmp[3];
		for (int j = 0; j < 3; j++) {
			tmp[j] = G.Get(i, j);
		}
		fprintf(file, "%f %f %f\n", tmp[0], tmp[1], tmp[2]);
	}
}
void BSplineCurve::OutputBSpline(FILE *file) {
	fprintf(file, "bspline\n");
	outputVertices(file);
}
void BSplineCurve::moveControlPoint(int selectedPoint, float x, float y) {
	float z = vertices[selectedPoint].z();
	vertices[selectedPoint].Set(x, y, z);
}
void BSplineCurve::addControlPoint(int selectedPoint, float x, float y) {
	Vec3f *new_vs = new Vec3f[numVertices + 1];
	Vec3f p(x, y, 0);
	for (int i = 0; i < selectedPoint; i++) new_vs[i] = vertices[i];
	vertices[selectedPoint] = p;
	for (int i = selectedPoint; i < numVertices; i++) {
		new_vs[i + 1] = vertices[i];
	}
	numVertices++;
	delete[] vertices;
	vertices = new_vs;
	new_vs = NULL;
}
void BSplineCurve::deleteControlPoint(int selectedPoint) {
	if (numVertices <= 4) return;
	for (int i = selectedPoint; i < numVertices - 1; i++) {
		vertices[i] = vertices[i + 1];
	}
	numVertices--;
}