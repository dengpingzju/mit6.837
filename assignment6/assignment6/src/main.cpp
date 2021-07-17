#define _CRT_SECURE_NO_WARNINGS
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "myutil.h"

//command line parameters
char input_file[256];
int width = 100;
int height = 100;
char output_file[256];
char normals_file[256];
int shade_back = 0;
int shadows = 0;
int gouraud = 0;
int visualize_grid = 0;
float depth_min, depth_max;
char depth_file[256];
int ntheta, nphi;
int bounces = 3;
float weight = 0.1f;
int gui = 0;
int stats = 0;
int nx, ny, nz;

//global parameters
int accelerate = 0;

void parse_para(int argc, char **argv);
SceneParser *pSce_par;
void doRayTracing();
void rayVisualization(float x, float y);
RayTracer *pRt;
int main(int argc, char **argv) {
	parse_para(argc, argv);
	pSce_par=new SceneParser(input_file);
	//firstly do raytracing once
	pRt=new RayTracer(pSce_par, bounces, weight, shadows);
	Grid *grid;
	if (nx == 0 || ny == 0 || nz == 0) {
		grid = NULL;
	}
	else {
		if (!visualize_grid) accelerate = 1;
		grid = new Grid(pSce_par->getGroup()->getBoundingBox(), nx, ny, nz);
	}
	pSce_par->getGroup()->insertIntoGrid(grid, NULL);
	pRt->setGrid(grid);
	BoundingBox *pbb = NULL;
	if (grid) pbb = grid->getBoundingBox();
	RayTracingStats::Initialize(width,height,pbb,nx,ny,nz);
	doRayTracing();
	if (stats) {
		RayTracingStats::PrintStatistics();
	}
	//opengl visualization
	if (gui) {
		GLCanvas glc;
		glc.initialize(pSce_par, doRayTracing, rayVisualization,
			pRt->getGrid(),visualize_grid);
	}
	else {
		//only on windows
		system("pause");
	}
	delete pSce_par;
	delete pRt;
	delete grid;
	return 0;
}
void doRayTracing() {
	if (!output_file[0]) {
		cerr << "No output file specified." << endl;
		return;
	}
	Image img(width, height);
	if (visualize_grid) pRt->traceGridToImage(img);
	else pRt->traceToImage(img);
	img.SaveTGA(output_file);
}
//trace a single ray by user coordinate
void rayVisualization(float x, float y) {
	Vec2f uc(x,y);
	Camera* cam = pSce_par->getCamera();
	Ray r(cam->generateRay(uc));
	Hit h;
	if (visualize_grid) pRt->getGrid()->intersect(r, h,cam->getTMin());
	else {
		pRt->traceRay(r, cam->getTMin(), 0, 1.0f, 1.0f, 1, h,accelerate);
	}
}
//parse parameters from command line
void parse_para(int argc, char **argv) {
	ParamParser pp(argc, argv);
	int tmp;
	string param,param2;
	assert(pp.getParam("input", 0, param) == 0);
	strcpy(input_file, param.c_str());
	assert(pp.getParam("size", 0, param) == 0);
	assert(pp.getParam("size", 1, param2) == 0);
	width = atoi(param.c_str());
	height = atoi(param2.c_str());
	tmp = pp.getParam("output", 0, param);
	if (tmp != -1) {
		assert(tmp == 0);
		strcpy(output_file, param.c_str());
	}
	tmp = pp.getParam("normals", 0, param);
	if (tmp != -1) {
		assert(tmp == 0);
		strcpy(normals_file, param.c_str());
	}
	tmp = pp.getParam("bounces", 0, param);
	if (tmp != -1) {
		assert(tmp == 0);
		bounces = atoi(param.c_str());
	}
	tmp = pp.getParam("weight", 0, param);
	if (tmp != -1) {
		assert(tmp == 0);
		weight = atof(param.c_str());
	}
	if (pp.getParam("shade_back", 0, param) != -1) {
		shade_back = 1;
	}
	if (pp.getParam("shadows", 0, param) != -1) {
		shadows = 1;
	}
	if (pp.getParam("gouraud", 0, param) != -1) {
		gouraud = 1;
	}
	if (pp.getParam("gui", 0, param) != -1) {
		gui = 1;
	}
	if (pp.getParam("visualize_grid", 0, param) != -1) {
		visualize_grid = 1;
	}
	if (pp.getParam("stats", 0, param) != -1) {
		stats = 1;
	}
	tmp = pp.getParam("depth", 0, param);
	if (tmp != -1) {
		assert(tmp == 0);
		depth_min = (float)atof(param.c_str());
		assert(pp.getParam("depth", 1, param) == 0);
		depth_max = (float)atof(param.c_str());
		assert(pp.getParam("depth", 2, param) == 0);
		strcpy(depth_file, param.c_str());
	}
	tmp = pp.getParam("grid", 0, param);
	if (tmp != -1) {
		assert(tmp == 0);
		nx = atoi(param.c_str());
		assert(pp.getParam("grid", 1, param) == 0);
		ny = atoi(param.c_str());
		assert(pp.getParam("grid", 2, param) == 0);
		nz = atoi(param.c_str());
	}
	tmp = pp.getParam("tessellation", 0, param);
	if (tmp != -1) {
		assert(tmp == 0);
		assert(pp.getParam("tessellation", 1, param2) == 0);
		ntheta = atoi(param.c_str());
		nphi = atoi(param2.c_str());
	}
}
