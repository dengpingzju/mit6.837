#ifndef _H_GRID_
#define _H_GRID_

#include "object3d.h"
#include "hit.h"
#include "vectors.h"
#include <math.h>
#include <map>

class Ray;
class BoundingBox;

typedef std::map<Object3D*, Hit> HitTable;

class MarchingInfo {
public:
	MarchingInfo(const Ray &r, const Grid &grid, float tmin,float tmax);
	MarchingInfo() {
		hasInter = -1;
	}
	~MarchingInfo() {}
	int hasEnded() {
		return hasInter != 1;
	}
	void nextCell();
	//don't go inside objects in scene
	bool checkHit(Hit &h);
	bool checkHitAny(Hit &h, float tmin);
	bool checkHitObjInCell(Hit &h, float tmin);

private:
	static bool inBoundingBox(Vec3f p, const BoundingBox* pb);
	static bool intersectWithBB(const Ray &r, Hit &h, float tmin, const BoundingBox &bb);
	float nextT() const;
	
	Vec3f diffVec;
	Vec3f nextVec;
	int cellIndex[3];
	const Grid *pGrid;
	int sign[3];//sign of dir
	float t;//have the same meaning as that in Ray class
	Vec3f normal;//normal of cell currently checking
	//the numbers keep consistent with those in grid.cpp
	int hasInter;//whether or not the ray intersects with the grid(0 or 1),
	//-1 means initial state, for simply default construct object
	//and extend this field to describe the state, -2 means arriving at end
	Vec3f rayDir;
	Ray ray;
	HitTable hitTable;
	float tmax;
};

class Grid : public Object3D {
public:
	//----------only for debugging------------
	void printCellMaxNum() {
		int ans = 0;
		for (int i = 0; i < nx; i++) {
			for (int j = 0; j < ny; j++) {
				for (int k = 0; k < nz; k++) {
					int num = getObjNum(i, j, k);
					if (num > ans) ans = num;
				}
			}
		}
		std::cout << "maximum number of objects in a single cell: " << ans << endl;
	}

	//----------------------------------------
	Grid(BoundingBox *bb, int nx, int ny, int nz);
	//no range check!
	PobjVec* getCellObj(int i, int j, int k) const {
		return &cellsObj[getLinearIndex(i, j, k)];
	}

	const PobjVec* getCellObjConst(int i, int j, int k) const {
		return &cellsObj[getLinearIndex(i, j, k)];
	}

	void getIndexFromCoord(Vec3f coord, int &i, int &j, int &k) const;

	void getIndexFromCoord(Vec3f coord, int *cellIndex) const {
		getIndexFromCoord(coord, cellIndex[0], cellIndex[1], cellIndex[2]);
	}

	void getGridSize(int &i, int &j, int &k) const {
		i = nx;
		j = ny;
		k = nz;
	}
	Vec3f getCellDia() const {
		return this->cellDia;
	}
	Vec3f getCellCenter(int i, int j, int k) const;
	void paint();
	void initializeRayMarch(MarchingInfo &mi, const Ray &r, float tmin,float tmax) const;
	bool intersect(const Ray &r, Hit &h, float tmin) const;
	bool intersectWithObj(const Ray &r, Hit &h, float tmin,float tmax) const;
	bool intersectShadowRay(const Ray &r, Hit &h, float tmin, float tmax) const;
	int isEmptyCell(int i, int j, int k) const;
	int getObjNum(int i, int j, int k) const;
	~Grid() {
		delete[] cellsObj;
	}
	static Material* getMaterialByNumberOfObj(int numObj);
	void getSurfaceByNum(int i, int j, int k, int numSurface, Vec3f *surf, Vec3f *normal) const ;
	void getSurfaceByNormal(int i, int j, int k, Vec3f normal, Vec3f *surf) const ;
	void addEnteredFaceByNormal(int i,int j,int k, Vec3f normal) const ;
	void addHitCellFaces(int i, int j, int k) const;
private:
	int nx, ny, nz;
	PobjVec* cellsObj;
	Vec3f cellDia;
	int getLinearIndex(int i, int j, int k) const {
		return ny * nz*i + nz * j + k;
	}
	void paintCell(int i, int j, int k);
	void paintSurface(int i, int j, int k, int numSurface);
};

#endif // !_H_GRID_

