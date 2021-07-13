#include "myutil.h"
void Group::addObject(int index, Object3D *obj) {
    if (index<0||index>=objNum) {
		cerr << "out of range in Group::addObject()" << endl;
    }
    else {
        objs[index]=obj;
    }
}
bool Group::intersect(const Ray &r, Hit &h, float tmin) const {
    float ans=MAX_T;
    Hit th;
    for (int i=0;i<objNum;i++) {
        if (objs[i]&&objs[i]->intersect(r,th,tmin)&&th.getT()<ans) {
            ans=th.getT();
            h=th;
        }
    }
    if (ans==MAX_T) {//no intersection
        return false;
    }
    else {//otherwise intersection found
        return true;
    }
}
bool Group::intersectShadowRay(const Ray &r, Hit &h, float tmin,float tmax) const {
	Hit th;
	for (int i = 0; i < objNum; i++) {
		if (objs[i] && objs[i]->intersectShadowRay(r, th, tmin,tmax)) {
			h = th;
			return true;
		}
	}
	return false;
}
void Group::paint() {
	for (int i = 0; i < objNum; i++) {
		pObj po = objs[i];
		if (po) po->paint();
	}
}
