#include "group.h"
void Group::addObject(int index, Object3D *obj) {
    if (index<0||index>=objNum) {
        errCode=2;
    }
    else {
        objs[index]=obj;
        errCode=0;
    }
}
bool Group::intersect(const Ray &r, Hit &h, float tmin) {
    float ans=MAX_T;
    Hit th;
    for (int i=0;i<objNum;i++) {
        if (objs[i]&&objs[i]->intersect(r,th,tmin)&&th.getT()<ans) {
            ans=th.getT();
            h=th;
        }
    }
    if (ans==MAX_T) {
        errCode=3;
        return false;
    }
    else {
        errCode=0;
        return true;
    }
}
