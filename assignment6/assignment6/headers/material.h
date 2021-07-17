#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include "vectors.h"
class Ray;
class Hit;

// ====================================================================
// ====================================================================

// You will extend this class in later assignments

class Material {
public:

  // CONSTRUCTORS & DESTRUCTOR
  Material(const Vec3f &d_color) { diffuseColor = d_color; }
  Material() {}
  virtual ~Material() {}
  virtual void glSetMaterial(void) const = 0;

  // ACCESSORS
  //v is the position in world space
  virtual Vec3f getDiffuseColor(Vec3f v=Vec3f()) const { return diffuseColor; }
  virtual Vec3f getSpecularColor(Vec3f v = Vec3f()) const { return Vec3f(); }
  virtual Vec3f getReflectiveColor(Vec3f v = Vec3f()) const { return Vec3f(); }
  virtual Vec3f getTransparentColor(Vec3f v = Vec3f()) const { return Vec3f(); }
  virtual float getIndexOfRefraction(Vec3f v = Vec3f()) const { return 0.0f; }

  virtual Vec3f Shade(const Ray &ray, const Hit &hit, const Vec3f &dirToLight,
	  const Vec3f &lightColor) const = 0;
protected:
  // REPRESENTATION
  Vec3f diffuseColor;
  
};

// ====================================================================
// ====================================================================

class PhongMaterial : public Material {
public:
	PhongMaterial(const Vec3f &diffuseColor,
		const Vec3f &specularColor,
		float exponent,
		const Vec3f &reflectiveColor,
		const Vec3f &transparentColor,
		float indexOfRefraction);

	Vec3f getSpecularColor(Vec3f v = Vec3f()) const { return specularColor; }
	Vec3f getReflectiveColor(Vec3f v = Vec3f()) const { return reflectiveColor; }
	Vec3f getTransparentColor(Vec3f v = Vec3f()) const { return transparentColor; }
	float getIndexOfRefraction(Vec3f v = Vec3f()) const { return indexOfRefraction; }

	Vec3f Shade(const Ray &ray, const Hit &hit, const Vec3f &dirToLight,
		const Vec3f &lightColor) const;
	
	void glSetMaterial(void) const;
private:
	Vec3f specularColor;
	Vec3f reflectiveColor;
	Vec3f transparentColor;
	float indexOfRefraction;
	float exponent;
};

class Noise : public Material {
public:
	Noise(Matrix *m, Material *mat1, Material *mat2, int octaves);
	float getN(Vec3f v) const;
	Noise() {}
	Vec3f getDiffuseColor(Vec3f v = Vec3f()) const;
	Vec3f getSpecularColor(Vec3f v = Vec3f()) const ;
	Vec3f getReflectiveColor(Vec3f v = Vec3f()) const;
	Vec3f getTransparentColor(Vec3f v = Vec3f()) const;
	float getIndexOfRefraction(Vec3f v = Vec3f()) const;

	Vec3f Shade(const Ray &ray, const Hit &hit, const Vec3f &dirToLight,
		const Vec3f &lightColor) const;
	void glSetMaterial(void) const;
	static float clamp(float t);
private:
	
	Matrix *m;
	Material *mat[2];
	int octaves;
};

class Marble : public Material {
public:
	Marble(Matrix *m, Material *mat1, Material *mat2, int octaves, float frequency, float amplitude);
	float getM(Vec3f v) const;
	Vec3f getDiffuseColor(Vec3f v = Vec3f()) const;
	Vec3f getSpecularColor(Vec3f v = Vec3f()) const;
	Vec3f getReflectiveColor(Vec3f v = Vec3f()) const;
	Vec3f getTransparentColor(Vec3f v = Vec3f()) const;
	float getIndexOfRefraction(Vec3f v = Vec3f()) const;

	Vec3f Shade(const Ray &ray, const Hit &hit, const Vec3f &dirToLight,
		const Vec3f &lightColor) const;
	void glSetMaterial(void) const;
private:
	Noise noise;
	Material *mat[2];
	Matrix *m;
	float frequency, amplitude;
};

class Wood : public Material {
public:
	Wood(Matrix *m, Material *mat1, Material *mat2, int octaves, float frequency, float amplitude);
	float getM(Vec3f v) const;
	Vec3f getDiffuseColor(Vec3f v = Vec3f()) const;
	Vec3f getSpecularColor(Vec3f v = Vec3f()) const;
	Vec3f getReflectiveColor(Vec3f v = Vec3f()) const;
	Vec3f getTransparentColor(Vec3f v = Vec3f()) const;
	float getIndexOfRefraction(Vec3f v = Vec3f()) const;

	Vec3f Shade(const Ray &ray, const Hit &hit, const Vec3f &dirToLight,
		const Vec3f &lightColor) const;
	void glSetMaterial(void) const;
private:
	Noise noise;
	Matrix *m;
	Material *mat[2];
	float frequency, amplitude;
	int veinDir;
};

typedef Material* PMaterial;

#endif
