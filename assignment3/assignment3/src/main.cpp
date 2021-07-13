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
float depth_min, depth_max;
char depth_file[256];
int ntheta, nphi;

void parse_para(int argc, char **argv);
int myShade(const Hit *h, const SceneParser& sce, Vec3f &ret);
int rayTracing(Vec2f uc, const SceneParser& sce, Vec3f& ret, int sb, int mode, int debug = 0);
int myRender(Image &img, const SceneParser& sce, int sb, int mode);
int render_depth(Image& img, SceneParser &sce);
void doRayTracing();
SceneParser *pSce_par;

int main(int argc, char **argv) {
	parse_para(argc, argv);
	pSce_par=new SceneParser(input_file);
	//firstly do raytracing once
	doRayTracing();
	//opengl visualization
	GLCanvas glc;
	glc.initialize(pSce_par, doRayTracing);
	delete pSce_par;
	return 0;
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
	assert(pp.getParam("output", 0, param) == 0);
	strcpy(output_file, param.c_str());
	tmp = pp.getParam("normals", 0, param);
	if (tmp != -1) {
		assert(tmp == 0);
		strcpy(normals_file, param.c_str());
	}
	if (pp.getParam("shade_back", 0, param) != -1) {
		shade_back = 1;
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
void doRayTracing() {
	//-------------raytracing-------------
	Image img(width, height);
	myRender(img, *pSce_par, shade_back, 1);
	img.SaveTGA(output_file);
	if (normals_file[0]) {
		myRender(img, *pSce_par, shade_back, 2);
		img.SaveTGA(normals_file);
	}
	if (depth_file[0]) {
		render_depth(img, *pSce_par);
		img.SaveTGA(depth_file);
	}
	//--------------------------------------
}
int myShade(const Ray *ray,const Hit *hit, const SceneParser& sce, Vec3f &ret) {
	//phong shading
	if (hit->getMaterial() == NULL) return -1;//fail
	Vec3f dir;
	Vec3f cdi(hit->getMaterial()->getDiffuseColor());
	Vec3f cli, shadeColor;
	float distToLight=0.0f;
	Hit h;
	for (int nli = 0; nli < sce.getNumLights(); nli++) {
		const Light *li = sce.getLight(nli);
		li->getIllumination(hit->getIntersectionPoint(), dir, cli, distToLight);
		Ray rToLi(hit->getIntersectionPoint(), dir);
		//if (!sce.getGroup()->intersect(rToLi,h,1e-4)) {
		shadeColor += hit->getMaterial()->Shade(*ray, *hit, dir, cli);
		//}
	}
	ret = sce.getAmbientLight()+shadeColor;
	return 0;//succeed
}
int rayTracing(Vec2f uc, const SceneParser& sce, Vec3f& ret, int sb, int mode, int debug) {
	Camera* cam = sce.getCamera();
	Ray r(cam->generateRay(uc));
	Group *gro = sce.getGroup();
	Hit h;
	if (!gro->intersect(r, h, cam->getTMin())) {
		if (mode == 1) ret = sce.getBackgroundColor();
		else if (mode == 2) ret.Set(0, 0, 0);
		return 2;//no intersection,background color
	}
	else {
		if (mode == 1) {//phong shading
			myShade(&r,&h, sce, ret);
			return 0;//right side,the most common case
		}
		else if (mode == 2) {//normal visualization
			Vec3f n = h.getNormal();
			ret.Set(fabs(n[0]), fabs(n[1]), fabs(n[2]));
			return 0;
		}
	}
	return 0;
}

int myRender(Image &img, const SceneParser& sce, int sb, int mode) {
	int w = img.Width(), h = img.Height();
	for (int i = 0; i < w; i++) {
		for (int j = 0; j < h; j++) {
			Vec2f uc(float(i) / w, float(j) / h);
			Vec3f cPix;
			rayTracing(uc, sce, cPix, sb, mode);
			img.SetPixel(i, j, cPix);
		}
	}
	return 0;
}
int render_depth(Image& img, SceneParser &sce) {
	int height = img.Height(), width = img.Width();
	Camera *cam = sce.getCamera();
	Group *gro = sce.getGroup();
	float *ts = new float[width*height];
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			Vec2f uc(float(x) / width, float(y) / height);
			Ray r = cam->generateRay(uc);
			if (cam->getErrCode() == 2) {
				cout << "error:out of range of [0,0] -> [1,1]" << endl;
				return -1;
			}
			Hit h;
			if (gro->intersect(r, h, cam->getTMin())) {
				ts[y*width + x] = h.getT();
			}
			else {
				ts[y*width + x] = MAX_T;
			}
		}
	}
	float nearest = MAX_T;
	float d = 1.0 / (depth_max - depth_min);
	for (int i = width * height - 1; i >= 0; i--) {
		if (ts[i] < nearest) nearest = ts[i];
	}
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			float c = 1.0 - (ts[y*width + x] - nearest)*d;
			img.SetPixel(x, y, Vec3f(c, c, c));
		}
	}
	return 0;
}
