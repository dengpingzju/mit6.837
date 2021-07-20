#include "myutil.h"
#include "perlin_noise.h"
#include <math.h>
#include <algorithm>
using namespace std;

#ifdef SPECULAR_FIX
// OPTIONAL:  global variable allows (hacky) communication 
// with glCanvas::display
extern int SPECULAR_FIX_WHICH_PASS;
#endif

//to adjust orientation of wood texture to make the vein in different directions
int woodTextOri = 0;

Vec3f PhongMaterial::Shade(const Ray &ray, const Hit &hit, const Vec3f &dirToLight,
	const Vec3f &lightColor) const {
	//ignore the ambient component, leave it to traceRay
	Vec3f V = Vec3f(0,0,0)-ray.getDirection();
	Vec3f H = dirToLight +V;
	H.Normalize();
	if (dirToLight.Dot3(hit.getNormal()) < 0) {//light source on the wrong side
		return Vec3f();
	}
	float specIntensity = max(H.Dot3(hit.getNormal()), 0.0f);
	float diffIntensity = max(dirToLight.Dot3(hit.getNormal()),0.0f);
	Vec3f specComp = lightColor*(specularColor * pow(specIntensity, exponent));
	Vec3f diffComp = lightColor * (diffuseColor*diffIntensity);
	Vec3f phongColor = specComp + diffComp;
	return phongColor;
}

PhongMaterial::PhongMaterial(const Vec3f &diffuseColor,
	const Vec3f &specularColor,
	float exponent,
	const Vec3f &reflectiveColor,
	const Vec3f &transparentColor,
	float indexOfRefraction) {

	this->diffuseColor = diffuseColor;
	this->specularColor = specularColor;
	this->exponent = exponent;
	this->reflectiveColor = reflectiveColor;
	this->transparentColor = transparentColor;
	this->indexOfRefraction = indexOfRefraction;
}

void PhongMaterial::glSetMaterial(void) const {

	GLfloat one[4] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat zero[4] = { 0.0, 0.0, 0.0, 0.0 };
	GLfloat specular[4] = {
	  getSpecularColor().r(),
	  getSpecularColor().g(),
	  getSpecularColor().b(),
	  1.0 };
	GLfloat diffuse[4] = {
	  getDiffuseColor().r(),
	  getDiffuseColor().g(),
	  getDiffuseColor().b(),
	  1.0 };

	// NOTE: GL uses the Blinn Torrance version of Phong...      
	float glexponent = exponent;
	if (glexponent < 0) glexponent = 0;
	if (glexponent > 128) glexponent = 128;

#if !SPECULAR_FIX 

	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, &glexponent);

#else

	// OPTIONAL: 3 pass rendering to fix the specular highlight 
	// artifact for small specular exponents (wide specular lobe)

	if (SPECULAR_FIX_WHICH_PASS == 0) {
		// First pass, draw only the specular highlights
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, zero);
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, zero);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, &glexponent);

	}
	else if (SPECULAR_FIX_WHICH_PASS == 1) {
		// Second pass, compute normal dot light 
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, one);
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, zero);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, zero);
	}
	else {
		// Third pass, add ambient & diffuse terms
		assert(SPECULAR_FIX_WHICH_PASS == 2);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, diffuse);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, zero);
	}

#endif
}

Noise::Noise(Matrix *m, Material *mat1, Material *mat2, int octaves) {
	this->m = m;
	this->mat[0] = mat1;
	this->mat[1] = mat2;
	this->octaves = octaves;
}
float Noise::getN(Vec3f v) const {
	float dividend = 1.0f;
	float sum = 0;
	for (int i = 0; i < octaves; i++) {
		float fNoise=PerlinNoise::noise(v.x()*dividend, v.y()*dividend, v.z()*dividend)/dividend;
		sum += fNoise;
		dividend *= 2;
	}
	sum += 0.5;
	return sum;
}

//surprising: t can be outside [0,1]
float Noise::clamp(float t) {
	return t;//do nothing, return it simply
	if (t < 0) return 0;
	else if (t > 1) return 1;
	else return t;
}
Vec3f Noise::getDiffuseColor(Vec3f v) const {
	this->m->Transform(v);
	float t = clamp(getN(v));
	return mat[0]->getDiffuseColor()*t + mat[1]->getDiffuseColor()*(1 - t);
}
Vec3f Noise::getSpecularColor(Vec3f v) const {
	this->m->Transform(v);
	float t = clamp(getN(v));
	return mat[0]->getSpecularColor()*t + mat[1]->getSpecularColor()*(1 - t);
}
Vec3f Noise::getReflectiveColor(Vec3f v) const {
	this->m->Transform(v);
	float t = clamp(getN(v));
	return mat[0]->getReflectiveColor()*t + mat[1]->getReflectiveColor()*(1 - t);
}
Vec3f Noise::getTransparentColor(Vec3f v) const {
	this->m->Transform(v);
	float t = clamp(getN(v));
	return mat[0]->getTransparentColor()*t + mat[1]->getTransparentColor()*(1 - t);
}
float Noise::getIndexOfRefraction(Vec3f v) const {
	this->m->Transform(v);
	float t = clamp(getN(v));
	return mat[0]->getIndexOfRefraction()*t + mat[1]->getIndexOfRefraction()*(1 - t);
}

Vec3f Noise::Shade(const Ray &ray, const Hit &hit, const Vec3f &dirToLight,
	const Vec3f &lightColor) const {

	Vec3f v = hit.getIntersectionPoint();
	m->Transform(v);
	float t = clamp(getN(v));
	return mat[0]->Shade(ray, hit, dirToLight, lightColor)*t
		+ mat[1]->Shade(ray, hit, dirToLight, lightColor)*(1 - t);
}

void Noise::glSetMaterial(void) const {
	this->mat[0]->glSetMaterial();
}

Marble::Marble(Matrix *m, Material *mat1, Material *mat2, int octaves, float frequency, float amplitude) {
	noise = Noise(m, mat1, mat2, octaves);
	this->frequency = frequency;
	this->amplitude = amplitude;
	this->m = m;
	this->mat[0] = mat1;
	this->mat[1] = mat2;
}

float Marble::getM(Vec3f v) const {
	return sin(frequency * v.x() + amplitude * noise.getN(v)-0.5)/2+0.5;
}
Vec3f Marble::getDiffuseColor(Vec3f v) const {
	m->Transform(v);
	float t = getM(v);
	return mat[0]->getDiffuseColor()*t + mat[1]->getDiffuseColor()*(1 - t);
}
Vec3f Marble::getSpecularColor(Vec3f v) const {
	m->Transform(v);
	float t = getM(v);
	return mat[0]->getSpecularColor()*t + mat[1]->getSpecularColor()*(1 - t);
}
Vec3f Marble::getReflectiveColor(Vec3f v) const {
	m->Transform(v);
	float t = getM(v);
	return mat[0]->getReflectiveColor()*t + mat[1]->getReflectiveColor()*(1 - t);
}
Vec3f Marble::getTransparentColor(Vec3f v) const {
	m->Transform(v);
	float t = getM(v);
	return mat[0]->getTransparentColor()*t + mat[1]->getTransparentColor()*(1 - t);
}
float Marble::getIndexOfRefraction(Vec3f v) const {
	m->Transform(v);
	float t = getM(v);
	return mat[0]->getIndexOfRefraction()*t + mat[1]->getIndexOfRefraction()*(1 - t);
}

Vec3f Marble::Shade(const Ray &ray, const Hit &hit, const Vec3f &dirToLight,
	const Vec3f &lightColor) const {

	Vec3f v = hit.getIntersectionPoint();
	m->Transform(v);
	float t = getM(v);
	return mat[0]->Shade(ray, hit, dirToLight, lightColor)*t
		+ mat[1]->Shade(ray, hit, dirToLight, lightColor)*(1 - t);
}

void Marble::glSetMaterial(void) const {
	this->mat[0]->glSetMaterial();
}

Wood::Wood(Matrix *m, Material *mat1, Material *mat2, int octaves, float frequency, float amplitude) {
	noise = Noise(m, mat1, mat2, octaves);
	this->frequency = frequency;
	this->amplitude = amplitude;
	this->m = m;
	this->mat[0] = mat1;
	this->mat[1] = mat2;
	this->veinDir = woodTextOri;
	woodTextOri = (woodTextOri + 1) % 3;
}

float Wood::getM(Vec3f v) const {
	float tmp;
	switch (veinDir) {
	case 0:
		tmp = Vec2f(v.x(), v.y()).Length();
		break;
	case 1:
		tmp = Vec2f(v.y(), v.z()).Length();
		break;
	case 2:
		tmp = Vec2f(v.x(), v.z()).Length();
		break;
	}
	return sin(frequency * tmp + amplitude * noise.getN(v))/2+0.5;
}
Vec3f Wood::getDiffuseColor(Vec3f v) const {
	m->Transform(v);
	float t = getM(v);
	return mat[0]->getDiffuseColor()*t + mat[1]->getDiffuseColor()*(1 - t);
}
Vec3f Wood::getSpecularColor(Vec3f v) const {
	m->Transform(v);
	float t = getM(v);
	return mat[0]->getSpecularColor()*t + mat[1]->getSpecularColor()*(1 - t);
}
Vec3f Wood::getReflectiveColor(Vec3f v) const {
	m->Transform(v);
	float t = getM(v);
	return mat[0]->getReflectiveColor()*t + mat[1]->getReflectiveColor()*(1 - t);
}
Vec3f Wood::getTransparentColor(Vec3f v) const {
	m->Transform(v);
	float t = getM(v);
	return mat[0]->getTransparentColor()*t + mat[1]->getTransparentColor()*(1 - t);
}
float Wood::getIndexOfRefraction(Vec3f v) const {
	m->Transform(v);
	float t = getM(v);
	return mat[0]->getIndexOfRefraction()*t + mat[1]->getIndexOfRefraction()*(1 - t);
}

Vec3f Wood::Shade(const Ray &ray, const Hit &hit, const Vec3f &dirToLight,
	const Vec3f &lightColor) const {

	Vec3f v = hit.getIntersectionPoint();
	m->Transform(v);
	float t = getM(v);
	return mat[0]->Shade(ray, hit, dirToLight, lightColor)*t
		+ mat[1]->Shade(ray, hit, dirToLight, lightColor)*(1 - t);
}

void Wood::glSetMaterial(void) const {
	this->mat[0]->glSetMaterial();
}
