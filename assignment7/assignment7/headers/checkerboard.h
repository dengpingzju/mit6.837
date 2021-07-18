#ifndef _H_CHECKERBOARD_
#define _H_CHECKERBOARD_

#include "material.h"
#include <math.h>

class Checkerboard : public Material {
public:
	Checkerboard(Matrix *m, Material *mat1, Material *mat2);
	~Checkerboard() {}
	void glSetMaterial(void) const;
	//v is the coordinate in texture coordinate system
	int getMaterialIndex(Vec3f v) const;
	Vec3f Shade(const Ray &ray, const Hit &hit, const Vec3f &dirToLight,
		const Vec3f &lightColor) const;
private:
	//v is the coordinate in world coordinate system
	Vec3f getTextureCoord(Vec3f v) const;
	Matrix *m;
	Material *mat[2];
};


#endif // !_H_CHECKERBOARD_

