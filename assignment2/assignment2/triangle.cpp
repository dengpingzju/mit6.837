#include "triangle.h"
Triangle::Triangle(Vec3f &a, Vec3f &b, Vec3f &c, Material *m) {
    P=a;
    u=b-a;
    v=c-a;
    Vec3f::Cross3(normal,u,v);
    normal.Normalize();
    material=m;
}
bool Triangle::intersect(const Ray &r, Hit &h, float tmin) {
    float m[3][4];//matrix of equations
    Vec3f d=r.getDirection();
    Vec3f c=r.getOrigin()-P;
    for (int i=0;i<3;i++) {
        m[i][0]=u[i];
        m[i][1]=v[i];
        m[i][2]=-d[i];
        m[i][3]=c[i];
    }
    //solve the equation with three unknowns

    for (int i=0;i<3;i++) {
        int k=i;
        for (int j=i+1;j<3;j++) {
            if (fabs(m[j][i])>fabs(m[k][i])) k=j;
        }
        if (fabs(m[k][i])<EPS) return false;
        if (k!=i) {
            for (int j=0;j<4;j++) {
                float tmp=m[i][j];
                m[i][j]=m[k][j];
                m[k][j]=tmp;
            }
        }
        for (int j=0;j<3;j++) {
            if (i==j) continue;
            float co=-m[j][i]/m[i][i];
            for (int k=i;k<4;k++) {
                m[j][k]=co*m[i][k]+m[j][k];
            }
        }
    }
    for (int i=0;i<3;i++) {
        m[i][3]=m[i][3]/m[i][i];
    }
    if (m[2][3]>=tmin&&m[0][3]>=0&&m[0][3]<=1&&m[1][3]>=0&&m[1][3]<=1&&(m[0][3]+m[1][3]<=1)) {
        h.set(m[2][3],material,normal,r);
        return true;
    }
    else return false;
}

