#ifndef _GROUP_H_
#define _GROUP_H_

#include "object3d.h"
#include "boundingbox.h"
#include <string.h>
class Group : public Object3D {
public:
    typedef Object3D* pObj;
    Group(int n) {
        if (n<0) {
            objs=0;
            objNum=-1;
			cerr << "Negative number of objects." << endl;
            return;
        }
        objs=new pObj[n];
        memset(objs,0,sizeof(pObj)*n);
        objNum=n;

		pBoundingBox = NULL;
    }
    ~Group() {
        delete [] objs;
		if (pBoundingBox) delete pBoundingBox;
    }
    void addObject(int index, Object3D *obj);
    virtual bool intersect(const Ray &r, Hit &h, float tmin) const;
	virtual bool intersectShadowRay(const Ray &r, Hit &h, float tmin,float tmax) const ;
	virtual void insertIntoGrid(Grid *g, Matrix *m);
	void paint();
private:
    int objNum;
    pObj *objs;
};

#endif // _GROUP_H_
