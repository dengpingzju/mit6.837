#include "myutil.h"
#include <map>

extern int visualize_grid;

MarchingInfo::MarchingInfo(const Ray &r, const Grid &grid, float tmin,float tmax) {
	ray = r;
	this->tmax = tmax;
	BoundingBox *pb = grid.getBoundingBox();
	pGrid = &grid;
	Vec3f origin = r.getOrigin();
	Hit h;
	if (!intersectWithBB(r, h, tmin, *pb)) {
		hasInter = 0;
		return;
	}
	hasInter = 1;
	Vec3f dia = grid.getCellDia();
	Vec3f dir = r.getDirection();
	rayDir = dir;
	float tmp[3];
	for (int i = 0; i < 3; i++) {
		if (dir[i] == 0) {
			tmp[i] = MAX_T;
			sign[i] = 0;
		}
		else {
			tmp[i] = dia[i] / my_fabsf(dir[i]);
			if (dir[i] > 0) sign[i] = 1;
			else sign[i] = -1;
		}
	}
	diffVec.Set(tmp[0], tmp[1], tmp[2]);
	if (inBoundingBox(origin, pb)) {
		grid.getIndexFromCoord(origin, cellIndex);
		//assume that the camera will never be inside a cell with objects
		t = -1;
	}
	else {
		if (h.getT() >= tmax) {
			hasInter = 0;
			return;
		}
		Vec3f p = h.getIntersectionPoint();
		grid.getIndexFromCoord(p, cellIndex);
		//from outside hit the sell, set t as it is
		t = h.getT();
		normal = h.getNormal();
		if (visualize_grid) {
			grid.addEnteredFaceByNormal(cellIndex[0], cellIndex[1], cellIndex[2], normal);
			grid.addHitCellFaces(cellIndex[0], cellIndex[1], cellIndex[2]);
		}
	}
	Vec3f center = grid.getCellCenter(cellIndex[0], cellIndex[1], cellIndex[2]);
	Vec3f adj = Vec3f(sign[0], sign[1], sign[2])*dia*0.5f + center;
	for (int i = 0; i < 3; i++) {
		if (dir[i] == 0) tmp[i] = MAX_T;
		else tmp[i] = (adj[i] - origin[i]) / dir[i];
	}
	nextVec.Set(tmp[0], tmp[1], tmp[2]);
}

float MarchingInfo::nextT() const {
	for (int i = 0; i < 3; i++) {
		int flag = 1;
		for (int j = 0; j < 3; j++) {
			if (nextVec[i] > nextVec[j]) {
				flag = 0;
				break;
			}
		}
		if (flag) return nextVec[i];
	}
	//never come here
	cerr << "error in MarchingInfo::nextT()" << endl;
	return MAX_T;
}

bool MarchingInfo::inBoundingBox(Vec3f p, const BoundingBox* pb) {
	Vec3f pmin = pb->getMin();
	Vec3f pmax = pb->getMax();
	float local_EPS = 1e-5;
	if (p.x() < pmin.x() - local_EPS || p.x() > pmax.x() + local_EPS) return false;
	if (p.y() < pmin.y() - local_EPS || p.y() > pmax.y() + local_EPS) return false;
	if (p.z() < pmin.z() - local_EPS || p.z() > pmax.z() + local_EPS) return false;
	return true;
}
//advance one cell
void MarchingInfo::nextCell() {
	if (hasInter != 1) return;
	if (t > tmax) {
		hasInter = -2;
		return;
	}
	RayTracingStats::IncrementNumGridCellsTraversed();
	Vec3f normals[3];
	normals[0].Set(1, 0, 0);
	normals[1].Set(0, 1, 0);
	normals[2].Set(0, 0, 1);
	for (int i = 0; i < 3; i++) {
		int flag = 1;
		for (int j = 0; j < 3; j++) {
			if (nextVec[i] > nextVec[j]) {
				flag = 0;
				break;
			}
		}
		if (flag) {
			cellIndex[i] += sign[i];
			t = nextVec[i];
			//be careful, here use normals[i] just because
			//it is identity matrix, and has nothing to do with normals
			nextVec += normals[i] * diffVec;
			normal = normals[i];
			if (rayDir.Dot3(normal) > 0) {
				//flip normal oritention
				normal.Negate();
			}
			break;
		}
	}
	if (pGrid->isEmptyCell(cellIndex[0], cellIndex[1], cellIndex[2]) == -1) {
		//out of range
		hasInter = -2;
	}
	else {
		if (visualize_grid) {
			pGrid->addEnteredFaceByNormal(cellIndex[0], cellIndex[1], cellIndex[2], normal);
			pGrid->addHitCellFaces(cellIndex[0], cellIndex[1], cellIndex[2]);
		}
	}
}
bool MarchingInfo::checkHit(Hit &h) {
	if (hasInter != 1) return false;
	if (pGrid->isEmptyCell(cellIndex[0], cellIndex[1], cellIndex[2])) {
		return false;//empty cell, no hit
	}
	else {
		//cell not empty, hit this cell
		h.setT(t);
		if (rayDir.Dot3(normal) > 0) {
			//flip normal oritention
			normal.Negate();
		}
		h.setNormal(normal);
		int objNum = pGrid->getObjNum(cellIndex[0], cellIndex[1], cellIndex[2]);
		Material *pm_cell = Grid::getMaterialByNumberOfObj(objNum);
		h.setMaterial(pm_cell);
		return true;
	}
}

bool MarchingInfo::checkHitAny(Hit &h, float tmin) {
	float local_EPS = 1e-5;
	if (hasInter != 1) return false;
	if (pGrid->isEmptyCell(cellIndex[0], cellIndex[1], cellIndex[2])) {
		return false;//empty cell, no hit
	}
	else {
		//cell not empty, hit this cell
		//go on to check whether intersect with objects in the cell
		PobjVec *cellObj = pGrid->getCellObj(cellIndex[0], cellIndex[1], cellIndex[2]);
		bool hasIntersection;
		for (int i = 0; i < (int)cellObj->size(); i++) {
			hasIntersection = cellObj->at(i)->intersect(ray, h, tmin);
			if (hasIntersection) return true;
		}
		return false;
	}
	//never come to here, leave the code to get rid of warnings of compiler
	cerr << "Unexpected error in MarchingInfo::checkHitObjInCell()";
	return false;
}

bool MarchingInfo::checkHitObjInCell(Hit &h,float tmin) {
	float local_EPS = 1e-5;
	if (hasInter != 1) return false;
	if (pGrid->isEmptyCell(cellIndex[0], cellIndex[1], cellIndex[2])) {
		return false;//empty cell, no hit
	}
	else {
		//cell not empty, hit this cell
		//go on to check whether intersect with objects in the cell
		PobjVec *cellObj = pGrid->getCellObj(cellIndex[0], cellIndex[1], cellIndex[2]);
		float ans = MAX_T;
		float cellMax = nextT();
		Hit ansHit;
		for (int i = 0; i < (int)cellObj->size(); i++) {
			Object3D *o = cellObj->at(i);
			bool hasIntersection = false;
			if (hitTable.find(o) != hitTable.end()) {
				ansHit = hitTable[o];
				if (ansHit.getT() != MAX_T) hasIntersection = true;
			}
			else {
				hasIntersection = o->intersect(ray, ansHit, tmin);
				if (!hasIntersection) ansHit.setT(MAX_T);
				hitTable[o] = ansHit;
			}
			if (hasIntersection) {
				float t0 = ansHit.getT();
				if (t0 <= cellMax + local_EPS && t0 < tmax && t0 < ans) {
					ans = t0;
					h = ansHit;
				}
			}
		}
		if (ans == MAX_T) {//no hit in the cell
			return false;
		}
		else {
			//once found a hit in the cell, the closest intersection must be in the cell
			return true;
		}
	}
	//never come to here, leave the code to get rid of warnings of compiler
	cerr << "Unexpected error in MarchingInfo::checkHitObjInCell()";
	return false;
}

bool MarchingInfo::intersectWithBB(const Ray &r, Hit &h, float tmin, const BoundingBox &bb) {
	RayTracingStats::IncrementNumIntersections();
	Vec3f origin = r.getOrigin();
	Vec3f dir = r.getDirection();
	Vec3f pmin = bb.getMin();
	Vec3f pmax = bb.getMax();
	float possibleT[6];
	int choice = -1;
	Vec3f normals[6];
	normals[0] = Vec3f(-1, 0, 0);
	normals[1] = Vec3f(0, -1, 0);
	normals[2] = Vec3f(0, 0, -1);
	normals[3] = Vec3f(1, 0, 0);
	normals[4] = Vec3f(0, 1, 0);
	normals[5] = Vec3f(0, 0, 1);
	for (int i = 0; i < 3; i++) {
		if (dir[i] == 0) possibleT[i] = MAX_T;
		else possibleT[i] = (pmin[i] - origin[i]) / dir[i];
	}
	for (int i = 0; i < 3; i++) {
		if (dir[i] == 0) possibleT[i + 3] = MAX_T;
		else possibleT[i + 3] = (pmax[i] - origin[i]) / dir[i];
	}
	float ans = MAX_T;
	for (int i = 0; i < 6; i++) {
		if (inBoundingBox(r.pointAtParameter(possibleT[i]), &bb)) {
			if (possibleT[i] < ans&&possibleT[i]>tmin) {
				ans = possibleT[i];
				choice = i;
			}
		}
	}
	if (ans == MAX_T) return false;
	else {
		h.set(ans, NULL, normals[choice], r);
		return true;
	}
}

int Grid::isEmptyCell(int i, int j, int k) const {
	//-1: out of range, 1: empty, 0: not empty
	if (i < 0 || i >= nx) return -1;
	if (j < 0 || j >= ny) return -1;
	if (k < 0 || k >= nz) return -1;

	PobjVec *cellObj = getCellObj(i, j, k);
	return (int)cellObj->empty();
}

int Grid::getObjNum(int i, int j, int k) const {
	//-1: out of range, >=0: number of objects
	if (i < 0 || i >= nx) return -1;
	if (j < 0 || j >= ny) return -1;
	if (k < 0 || k >= nz) return -1;

	PobjVec *cellObj = getCellObj(i, j, k);
	return (int)cellObj->size();
}

void Grid::getSurfaceByNormal(int i, int j, int k, Vec3f normal, Vec3f *surf) const {
	Vec3f normals[6];
	float local_EPS = 1e-5;
	normals[0] = Vec3f(0, 0, 1);
	normals[1] = Vec3f(0, 1, 0);
	normals[2] = Vec3f(1, 0, 0);
	normals[3] = Vec3f(0, -1, 0);
	normals[4] = Vec3f(-1, 0, 0);
	normals[5] = Vec3f(0, 0, -1);
	int numSurface=-1;
	for (int i = 0; i < 6; i++) {
		if ((normal - normals[i]).Length() < local_EPS) {
			numSurface = i;
			break;
		}
	}
	if (numSurface == -1) {
		cerr << "normal wrong, in Grid::getSurfaceByNormal" << endl;
		return;
	}
	getSurfaceByNum(i, j, k, numSurface, surf, NULL);
}

void Grid::getSurfaceByNum(int i, int j, int k, int numSurface,Vec3f *surf,Vec3f *normal) const {
	//the numSurface specify which surface to be painted
	//the number is consistent with dVec in Grid::paintCell()
	//for example, dVec[0]={0,0,1}, and numSurface=0 means
	//paint the surface shared by cell(i,j,k) and cell(i,j,k+1)

	//and vertices also need numbering, first choose surface with smaller
	//z coordinates, and number 4 vertices counterclockwise, then the surface 
	//farther from xy-plane
	Vec3f vertices[8];
	//compute 8 vertices
	Vec3f sur[4];
	sur[0] = Vec3f(-0.5, -0.5, -0.5);
	sur[1] = Vec3f(0.5, -0.5, -0.5);
	sur[2] = Vec3f(0.5, 0.5, -0.5);
	sur[3] = Vec3f(-0.5, 0.5, -0.5);
	Vec3f center = getCellCenter(i, j, k);
	for (int i = 0; i < 4; i++) {
		vertices[i] = center + sur[i] * cellDia;
	}
	for (int i = 0; i < 4; i++) {
		vertices[i + 4] = vertices[i] + Vec3f(0, 0, cellDia.z());
	}

	int surfaces[6][4] = { 4,5,6,7, 3,2,6,7, 1,5,6,2,0,4,5,1, 0,3,7,4,0,1,2,3 };
	Vec3f normals[6];
	normals[0] = Vec3f(0, 0, 1);
	normals[1] = Vec3f(0, 1, 0);
	normals[2] = Vec3f(1, 0, 0);
	normals[3] = Vec3f(0, -1, 0);
	normals[4] = Vec3f(-1, 0, 0);
	normals[5] = Vec3f(0, 0, -1);

	for (int i = 0; i < 4; i++) {
		surf[i] = vertices[surfaces[numSurface][i]];
	}
	if (normal) *normal = normals[numSurface];
}

void Grid::paintSurface(int i, int j, int k, int numSurface) {
	Vec3f vertices[4];
	Vec3f normal;
	getSurfaceByNum(i, j, k, numSurface, vertices, &normal);
	PobjVec *cellObj = getCellObj(i, j, k);
	int numObj = cellObj->size();
	Material *pm_cell = getMaterialByNumberOfObj(numObj);
	CellFace cf(vertices[0], vertices[1], vertices[2], vertices[3],normal,
		pm_cell);
	cf.paint();
	delete pm_cell;
}

Material* Grid::getMaterialByNumberOfObj(int numObj) {
	Vec3f diffColor;
	if (numObj == 0) {
		diffColor.Set(1, 0, 0.5);
	}
	else if (numObj == 1) {
		diffColor.Set(1, 1, 1);
	}
	else if (numObj == 2) {
		diffColor.Set(1, 0, 1);
	}
	else if (numObj == 3) {
		diffColor.Set(0, 0, 1);
	}
	else {
		diffColor.Set(1, 0, 0);
	}
	PhongMaterial *pm_cell = new PhongMaterial(diffColor,
		Vec3f(0, 0, 0),
		0,
		Vec3f(0, 0, 0),
		Vec3f(0, 0, 0),
		0);//memory leak, not a big deal
	return pm_cell;
}

void Grid::paintCell(int i, int j, int k) {
	PobjVec *cellObj = getCellObj(i,j,k);
	if (isEmptyCell(i, j, k)) return;
	int dVec[6][3] = { 0,0,1,0,1,0,1,0,0,0,-1,0,-1,0,0,0,0,-1 };
	for (int s = 0; s < 6; s++) {
		int i2, j2, k2;
		i2 = i + dVec[s][0];
		j2 = j + dVec[s][1];
		k2 = k + dVec[s][2];
		if (isEmptyCell(i2, j2, k2)!=0) {
			//paint a surface
			paintSurface(i, j, k, s);
		}
	}
}

void Grid::getIndexFromCoord(Vec3f coord, int &i, int &j, int &k) const {
	Vec3f minPos = pBoundingBox->getMin();
	Vec3f maxPos = pBoundingBox->getMax();
	Vec3f dia = maxPos - minPos;
	Vec3f tmp = coord - minPos;
	for (int i = 0; i < 3; i++) {
		if (my_fabsf(tmp[i]) < 1e-5) {
			coord.SetIndex(i, coord[i] + 1e-5);
		}
	}
	tmp = coord - maxPos;
	for (int i = 0; i < 3; i++) {
		if (my_fabsf(tmp[i]) < 1e-5) {
			coord.SetIndex(i, coord[i] - 1e-5);
		}
	}
	float dx = dia.x() / nx;
	float dy = dia.y() / ny;
	float dz = dia.z() / nz;
	i = (int)floor((coord.x() - minPos.x()) / dx);
	j = (int)floor((coord.y() - minPos.y()) / dy);
	k = (int)floor((coord.z() - minPos.z()) / dz);
}

Vec3f Grid::getCellCenter(int i, int j, int k) const {
	Vec3f minPos = pBoundingBox->getMin();
	float tmp1, tmp2, tmp3;
	tmp1 = (i + 0.5)*cellDia.x() + minPos.x();
	tmp2 = (j + 0.5)*cellDia.y() + minPos.y();
	tmp3 = (k + 0.5)*cellDia.z() + minPos.z();
	return Vec3f(tmp1, tmp2, tmp3);
}

Grid::Grid(BoundingBox *bb, int nx, int ny, int nz) {
	pBoundingBox = bb;
	this->nx = nx;
	this->ny = ny;
	this->nz = nz;

	Vec3f dia = pBoundingBox->getMax() - pBoundingBox->getMin();
	this->cellDia = dia * Vec3f(1.0f / nx, 1.0f / ny, 1.0f / nz);

	cellsObj = new PobjVec[nx*ny*nz];
}

void Grid::paint() {
	for (int i = 0; i < nx; i++) {
		for (int j = 0; j < ny; j++) {
			for (int k = 0; k < nz; k++) {
				paintCell(i,j,k);
			}
		}
	}
}

void Grid::initializeRayMarch(MarchingInfo &mi, const Ray &r, float tmin,float tmax) const {
	mi = MarchingInfo(r, *this,tmin,tmax);
}

bool Grid::intersect(const Ray &r, Hit &h, float tmin) const {
	MarchingInfo march;
	initializeRayMarch(march, r, tmin,MAX_T);
	while (1) {
		if (march.checkHit(h)) {
			RayTree::SetMainSegment(r, 0, h.getT());
			return true;
		}
		march.nextCell();
		if (march.hasEnded()) {
			RayTree::SetMainSegment(r, 0, MAX_T);
			return false;
		}
	}
	//never come to here
	cerr << "Unexpected error in Grid::intersect()" << endl;
	return false;
}

//be careful not to change Hit &h, if there is no hit
bool Grid::intersectWithObj(const Ray &r, Hit &h, float tmin,float tmax) const {
	MarchingInfo march;
	initializeRayMarch(march, r, tmin,tmax);
	while (1) {
		if (march.checkHitObjInCell(h,tmin)) {
			return true;
		}
		march.nextCell();
		if (march.hasEnded()) {
			return false;
		}
	}
	//never come to here
	cerr << "Unexpected error in Grid::intersectWithObj()" << endl;
	return false;
}

bool Grid::intersectShadowRay(const Ray &r, Hit &h, float tmin, float tmax) const {
	MarchingInfo march;
	initializeRayMarch(march, r, tmin, tmax);
	while (1) {
		if (march.checkHitAny(h, tmin)) {
			return true;
		}
		march.nextCell();
		if (march.hasEnded()) {
			return false;
		}
	}
	//never come to here
	cerr << "Unexpected error in Grid::intersectWithObj()" << endl;
	return false;
}

void Grid::addEnteredFaceByNormal(int i, int j, int k, Vec3f normal) const {
	Vec3f face[4];
	getSurfaceByNormal(i, j, k, normal, face);
	Material *pm_cell = getMaterialByNumberOfObj(getObjNum(i,j,k));
	RayTree::AddEnteredFace(face[0], face[1], face[2], face[3], normal, pm_cell);
}

void Grid::addHitCellFaces(int i, int j, int k) const {
	Vec3f face[4];
	Vec3f normal;
	Material *pm_cell = getMaterialByNumberOfObj(getObjNum(i, j, k));
	for (int numSurface = 0; numSurface < 6; numSurface++) {
		getSurfaceByNum(i, j, k, numSurface, face, &normal);
		RayTree::AddHitCellFace(face[0], face[1], face[2], face[3], normal, pm_cell);
	}
}
