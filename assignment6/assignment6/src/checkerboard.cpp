#include "checkerboard.h"
#include "myutil.h"
Checkerboard::Checkerboard(Matrix *m, Material *mat1, Material *mat2) {
	this->m = m;
	this->mat[0] = mat1;
	this->mat[1] = mat2;
}

void Checkerboard::glSetMaterial(void) const {
	this->mat[0]->glSetMaterial();
}

int Checkerboard::getMaterialIndex(Vec3f v) const {
	int iIndex = 0;
	for (int i = 0; i < 3; i++) {
		iIndex += (int)floor(v[i]);
	}
	if (iIndex < 0) iIndex = -iIndex;
	return iIndex & 1;
}
Vec3f Checkerboard::getTextureCoord(Vec3f v) const {
	m->Transform(v);
	return v;
}

Vec3f Checkerboard::Shade(const Ray &ray, const Hit &hit, const Vec3f &dirToLight,
	const Vec3f &lightColor) const {
	Vec3f vText = getTextureCoord(hit.getIntersectionPoint());
	Material *m = mat[getMaterialIndex(vText)];
	return m->Shade(ray, hit, dirToLight, lightColor);
}
