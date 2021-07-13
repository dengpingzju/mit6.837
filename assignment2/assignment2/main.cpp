#include <iostream>
#include <cstring>
#include "hit.h"
#include "scene_parser.h"
#include "image.h"
#include "material.h"
#include "light.h"
#include "group.h"
#include "camera.h"
#include "object3d.h"
#include "triangle.h"

using namespace std;

char *input_file = NULL;
int width = 100;
int height = 100;
char *output_file = NULL;
char *normals_file=NULL;
int shade_back=0;
float depth_min,depth_max;
char *depth_file=NULL;


void parse_para(int argc,char **argv);
int myShade(const Hit *h,const SceneParser& sce,Vec3f &ret);
int rayTracing(Vec2f uc,const SceneParser& sce,Vec3f& ret,int sb,int mode,int debug=0);
int myRender(Image &img,const SceneParser& sce,int sb,int mode);
int render_depth(Image& img,SceneParser &sce);

void test() {
    Vec3f a(1,0,0),b(0,1,0),c(0,0,1);
    Triangle tri(a,b,c,NULL);
    Vec3f d(1,1,-0.5);
    d.Normalize();
    Ray r(Vec3f(),d);
    Hit h;
    if (tri.intersect(r,h,EPS)) {
        cout<<h<<endl;
    }
    else {
        cout<<"No intersection"<<endl;
    }
}

int main(int argc,char **argv)
{
    //test();
    //return 0;
    parse_para(argc,argv);
    SceneParser sce_par(input_file);
    Image img(width,height);
    myRender(img,sce_par,shade_back,1);
    img.SaveTGA(output_file);
    if (normals_file) {
        myRender(img,sce_par,shade_back,2);
        img.SaveTGA(normals_file);
    }
    if (depth_file) {
        render_depth(img,sce_par);
        img.SaveTGA(depth_file);
    }
    return 0;
}
void parse_para(int argc,char **argv) {
    // sample command line:
    // raytracer -input scene1_1.txt -size 200 200 -output output1_1.tga -depth 9 10 depth1_1.tga

    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i],"-input")) {
            i++; assert (i < argc);
            input_file = argv[i];
        } else if (!strcmp(argv[i],"-size")) {
            i++; assert (i < argc);
            width = atoi(argv[i]);
            i++; assert (i < argc);
            height = atoi(argv[i]);
        } else if (!strcmp(argv[i],"-output")) {
            i++; assert (i < argc);
            output_file = argv[i];
        } else if (!strcmp(argv[i],"-normals")) {
            i++; assert (i < argc);
            normals_file=argv[i];
        } else if (!strcmp(argv[i],"-shade_back")) {
            shade_back=1;
        } else if (!strcmp(argv[i],"-depth")) {
            i++; assert (i < argc);
            depth_min = atof(argv[i]);
            i++; assert (i < argc);
            depth_max = atof(argv[i]);
            i++; assert (i < argc);
            depth_file = argv[i];
        } else {
            printf ("whoops error with command line argument %d: '%s'\n",i,argv[i]);
            assert(0);
        }
    }
}
int myShade(const Hit *hit,const SceneParser& sce,Vec3f &ret) {//diffuse
    if (hit->getMaterial()==NULL) return -1;//fail
    Vec3f dir;
    Vec3f cdi(hit->getMaterial()->getDiffuseColor());
    Vec3f cli,csha;
    Hit h;
    for (int nli=0;nli<sce.getNumLights();nli++) {
        const Light *li=sce.getLight(nli);
        li->getIllumination(hit->getIntersectionPoint(),dir,cli);
        Ray rToLi(hit->getIntersectionPoint(),dir);
        //if (!sce.getGroup()->intersect(rToLi,h,1e-4)) {
            float d=dir.Dot3(hit->getNormal());
            if (d<0) continue;
            csha+=d*(cli*cdi);
        //}
    }
    ret=csha;
    return 0;//succeed
}
int rayTracing(Vec2f uc,const SceneParser& sce,Vec3f& ret,int sb,int mode,int debug) {
    Camera* cam=sce.getCamera();
    Ray r(cam->generateRay(uc));
    Group *gro=sce.getGroup();
    Hit h;
    if (!gro->intersect(r,h,cam->getTMin())) {
        if (mode==1) ret=sce.getBackgroundColor();
        else if (mode==2) ret.Set(0,0,0);
        return 2;//no intersection,background color
    }
    else {
        if (mode==1) {//diffuse shading
            if (h.getNormal().Dot3(r.getDirection())>0) {//on the wrong side of surface
                if (sb) {
                    h=Hit(h.getT(),h.getMaterial(),-1*h.getNormal());
                }
                else {
                    ret.Set(0,0,0);
                    return 1;//wrong side,the color is black
                }
            }
            Vec3f cMat=h.getMaterial()->getDiffuseColor();
            Vec3f cSha;
            //ambient term
            ret=sce.getAmbientLight()*cMat;
            //diffuse shading
            if (!myShade(&h,sce,cSha)) {
                ret+=cSha;
            }
            //specular highlight to be added
            return 0;//right side,the most common case
        }
        else if (mode==2) {//normal visualization
            Vec3f n=h.getNormal();
            ret.Set(fabs(n[0]),fabs(n[1]),fabs(n[2]));
            return 0;
        }
    }
    return 0;
}

int myRender(Image &img,const SceneParser& sce,int sb,int mode) {
    int w=img.Width(),h=img.Height();
    for (int i=0;i<w;i++) {
        for (int j=0;j<h;j++) {
            Vec2f uc(float(i)/w,float(j)/h);
            Vec3f cPix;
            rayTracing(uc,sce,cPix,sb,mode);
            img.SetPixel(i,j,cPix);
        }
    }
    return 0;
}
int render_depth(Image& img,SceneParser &sce) {
    int height=img.Height(),width=img.Width();
    Camera *cam=sce.getCamera();
    Group *gro=sce.getGroup();
    float *ts=new float[width*height];
    for (int y=0;y<height;y++) {
        for (int x=0;x<width;x++) {
            Vec2f uc(float(x)/width,float(y)/height);
            Ray r=cam->generateRay(uc);
            if (cam->getErrCode()==2) {
                cout<<"error:out of range of [0,0] -> [1,1]"<<endl;
                return -1;
            }
            Hit h;
            if (gro->intersect(r,h,cam->getTMin())) {
                ts[y*width+x]=h.getT();
            }
            else {
                ts[y*width+x]=MAX_T;
            }
        }
    }
    float nearest=MAX_T;
    float d=1.0/(depth_max-depth_min);
    for (int i=width*height-1;i>=0;i--) {
        if (ts[i]<nearest) nearest=ts[i];
    }
    for (int y=0;y<height;y++) {
        for (int x=0;x<width;x++) {
            float c=1.0-(ts[y*width+x]-nearest)*d;
            img.SetPixel(x,y,Vec3f(c,c,c));
        }
    }
    return 0;
}




