#define _CRT_SECURE_NO_WARNINGS
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "myutil.h"


char input_file[256];
int width = 100;
int height = 100;
char output_file[256];
char normals_file[256];
int shade_back = 0;
int shadows = 0;
int gouraud = 0;
float depth_min, depth_max;
char depth_file[256];
int ntheta, nphi;
int bounces = 3;
float weight = 0.1f;
int gui = 0;

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
	doRayTracing();
	//opengl visualization
	if (gui) {
		GLCanvas glc;
		glc.initialize(pSce_par, doRayTracing, rayVisualization);
	}
	delete pSce_par;
	delete pRt;
	return 0;
}
void doRayTracing() {
	if (!output_file[0]) {
		cerr << "No output file specified." << endl;
		return;
	}
	Image img(width, height);
	pRt->traceToImage(img);
	img.SaveTGA(output_file);
}
void rayVisualization(float x, float y) {
	Vec2f uc(x,y);
	Camera* cam = pSce_par->getCamera();
	Ray r(cam->generateRay(uc));
	Hit h;
	pRt->traceRay(r, cam->getTMin(), 0, 1.0f, 1.0f, 1,h);
}
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
	tmp = pp.getParam("depth", 0, param);
	if (tmp != -1) {
		assert(tmp == 0);
		depth_min = (float)atof(param.c_str());
		assert(pp.getParam("depth", 1, param) == 0);
		depth_max = (float)atof(param.c_str());
		assert(pp.getParam("depth", 2, param) == 0);
		strcpy(depth_file, param.c_str());
	}
	tmp = pp.getParam("tessellation", 0, param);
	if (tmp != -1) {
		assert(tmp == 0);
		assert(pp.getParam("tessellation", 1, param2) == 0);
		ntheta = atoi(param.c_str());
		nphi = atoi(param2.c_str());
	}
}
