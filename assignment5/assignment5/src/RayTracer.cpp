#include "myutil.h"
//visualization is ok, bug fixed
extern int shade_back;
extern int gui;
RayTracer::RayTracer(SceneParser *s, int max_bounces,
	float cutoff_weight, bool shadows) {
	this->pScene = s;
	this->max_bounces = max_bounces;
	this->cutoff_weight = cutoff_weight;
	this->shadows = shadows;
}
bool RayTracer::intersectShadowRay(const Ray &r, Hit &h, float tmin, float tmax) const {
	return pScene->getGroup()->intersectShadowRay(r, h, tmin, tmax);
}
int RayTracer::shade(const Ray *ray, const Hit *hit, bool shadow, Vec3f &ret) const {
	//phong shading
	if (hit->getMaterial() == NULL) return -1;//fail
	Vec3f dir;
	Vec3f cli, shadeColor;
	float distToLight = 0.0f;
	Hit h;
	for (int nli = 0; nli < pScene->getNumLights(); nli++) {
		const Light *li = pScene->getLight(nli);
		li->getIllumination(hit->getIntersectionPoint(), dir, cli, distToLight);
		Ray rToLi(hit->getIntersectionPoint(), dir);
		if (!shadow || !intersectShadowRay(rToLi, h, UNIT_EPS, distToLight)) {
			shadeColor += hit->getMaterial()->Shade(*ray, *hit, dir, cli);
		}
		if (gui) {//for visualization
			if (shadows) {
				if (pScene->getGroup()->intersect(rToLi, h, UNIT_EPS)) {
					if (h.getT() < distToLight) RayTree::AddShadowSegment(rToLi, 0, h.getT());
					else RayTree::AddShadowSegment(rToLi, 0, distToLight);
				}
				else {
					RayTree::AddShadowSegment(rToLi, 0, distToLight);
				}

			}
			else {
				RayTree::AddShadowSegment(rToLi, 0, distToLight);
			}
		}
	}
	ret = pScene->getAmbientLight()*hit->getMaterial()->getDiffuseColor() + shadeColor;
	return 0;//succeed
}
Vec3f RayTracer::mirrorDirection(const Vec3f &normal, const Vec3f &incoming) {
	Vec3f ret;
	float tmp = incoming.Dot3(normal);
	if (tmp > 0 && !shade_back) {
		cerr << "You should activate shade_back option to compute reflective ray." << endl;
		return Vec3f();
	}
	ret = incoming - normal * (2 * tmp);
	ret.Normalize();
	return ret;
}

bool RayTracer::transmittedDirection(const Vec3f &normal, const Vec3f &incoming,
	float index_i, float index_t, Vec3f &transmitted) {

	Vec3f iVec = incoming;
	Vec3f trueNormal = normal;
	iVec.Negate();
	float tmp = iVec.Dot3(normal);
	if (tmp < 0) {
		if (shade_back) trueNormal.Negate();
		else {//error, refraction but without shade_back
			cerr << "You should activate shade_back option to compute refractional ray." << endl;
			return false;
		}
	}
	float nr = index_i / index_t;
	float tmp2;
	tmp2 = trueNormal.Dot3(iVec);
	tmp = tmp2;
	tmp = nr * tmp;
	tmp2 *= tmp2;
	tmp2 = nr * nr*(1 - tmp2);
	if (tmp2 <= 1) {//ok
		transmitted = trueNormal * (tmp - sqrt(1 - tmp2)) - iVec * nr;
		transmitted.Normalize();
		return true;
	}
	else {//internal reflection
		return false;
	}
}

Vec3f RayTracer::traceRay(Ray &ray, float tmin, int bounces, float weight,
	float indexOfRefraction, int role, Hit &hit) const {
	int whichSide = 0;//0: outside, 1: inside
	Camera* cam = pScene->getCamera();
	Group *gro = pScene->getGroup();
	Hit h;
	if (!gro->intersect(ray, h, tmin)) {
		h.setNormal(Vec3f());//set normal to 0 vector to indicate no intersection
		if (role == 2) {//reflective ray
			RayTree::AddReflectedSegment(ray, 0, MAX_T);
		}
		else if (role == 3) {//refracted ray
			RayTree::AddTransmittedSegment(ray, 0, MAX_T);
		}
		return pScene->getBackgroundColor();
	}
	else {
		hit = h;
		if (role == 1) {//main segment
			RayTree::SetMainSegment(ray, 0, h.getT());
		}
		else if (role == 2) {//reflective ray
			RayTree::AddReflectedSegment(ray, 0, h.getT());
		}
		else if (role == 3) {//refracted ray
			RayTree::AddTransmittedSegment(ray, 0, h.getT());
		}
		Material *pMat = h.getMaterial();
		if (ray.getDirection().Dot3(h.getNormal()) > 0) {
			Vec3f flippedNormal = h.getNormal();
			flippedNormal.Negate();
			h.setNormal(flippedNormal);
			whichSide = 1;
		}
		Vec3f shadowColor;
		shade(&ray, &h, shadows, shadowColor);
		Vec3f refColor, refraColor;
		if (bounces < max_bounces) {
			float refWeight = pMat->getReflectiveColor().Length()*weight;
			float refraWeight = pMat->getTransparentColor().Length()*weight;
			if (refWeight > cutoff_weight) {
				Vec3f vRef = mirrorDirection(h.getNormal(), ray.getDirection());
				Ray rRef(h.getIntersectionPoint(), vRef);
				Hit h2;
				refColor = traceRay(rRef, UNIT_EPS, bounces + 1, refWeight, indexOfRefraction, 2, h2);
				refColor = refColor * pMat->getReflectiveColor();
			}
			if (refraWeight > cutoff_weight) {
				Vec3f vRefra;
				float index_t = (whichSide) ? 1.0f : h.getMaterial()->getIndexOfRefraction();
				if (transmittedDirection(h.getNormal(), ray.getDirection(), indexOfRefraction, index_t, vRefra)) {
					Ray rRefra(h.getIntersectionPoint(), vRefra);
					Hit h3;
					refraColor = traceRay(rRefra, UNIT_EPS, bounces + 1, refraWeight, index_t, 3, h3);
					refraColor = refraColor * pMat->getTransparentColor();
				}
			}
		}
		return shadowColor + refColor + refraColor;
	}
}
void RayTracer::traceToImage(Image& img) const {
	int w = img.Width(), h = img.Height();
	for (int i = 0; i < w; i++) {
		for (int j = 0; j < h; j++) {
			Vec2f uc((i + 0.5) / w, (j + 0.5) / h);
			Vec3f pixColor;
			Camera* cam = pScene->getCamera();
			Ray r(cam->generateRay(uc));
			Hit h;
			pixColor = traceRay(r, cam->getTMin(), 0, 1.0f, 1.0f, 1, h);
			img.SetPixel(i, j, pixColor);
		}
	}
}
void RayTracer::traceGridToImage(Image& img) const {
	int w = img.Width(), h = img.Height();
	for (int i = 0; i < w; i++) {
		for (int j = 0; j < h; j++) {
			Vec2f uc((i + 0.5) / w, (j + 0.5) / h);
			Vec3f pixColor;
			Camera* cam = pScene->getCamera();
			Ray r(cam->generateRay(uc));
			Hit h;
			if (grid->intersect(r, h, cam->getTMin())) {
				shade(&r, &h, false, pixColor);
				img.SetPixel(i, j, pixColor);
			}
			else {
				img.SetPixel(i, j, pScene->getBackgroundColor());
			}
		}
	}
}
