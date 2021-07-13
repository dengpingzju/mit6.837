#ifndef _GROUP_H_
#define _GROUP_H_

#include "object3d.h"
#include <cstring>
class Group : public Object3D {
public:
    typedef Object3D* pObj;
    Group(int n) {
        if (n<0) {
            objs=0;
            objNum=-1;
            errCode=1;
            return;
        }
        objs=new pObj[n];
        memset(objs,0,sizeof(pObj)*n);
        objNum=n;
        errCode=0;
    }
    ~Group() {
        delete [] objs;
    }
    void addObject(int index, Object3D *obj);
    virtual bool intersect(const Ray &r, Hit &h, float tmin);
private:
    int objNum;
    pObj *objs;
    int errCode;//after any operation which causes errors,the field is set correspondingly
    //0:none,1:negative objNum,2:index out of range,3:no intersections
};

#endif // _GROUP_H_
