#include <iostream>
#include <cstdio>
#include <algorithm>
#include "sphere.h"
#include "camera.h"
#include "scene_parser.h"
#include "image.h"
#include "group.h"
using namespace std;
float const tmin=1e-9;
char *input_file = NULL;
int width = 100;
int height = 100;
char *output_file = NULL;
float depth_min = 0;
float depth_max = 1;
char *depth_file = NULL;
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
int getIndex(int x,int y) {
    return y*width+x;
}
int solve() {
    SceneParser sp(input_file);
    Camera *cam=sp.getCamera();
    Group *gro=sp.getGroup();
    int L=max(width,height);
    Image img(width,height);
    Image imgDep(width,height);
    img.SetAllPixels(sp.getBackgroundColor());
    imgDep.SetAllPixels(sp.getBackgroundColor());
    float *ts=new float[width*height];
    for (int y=0;y<height;y++) {
        for (int x=0;x<width;x++) {
            Vec2f scrCoord((float)x/L,(float)y/L);
            Ray r=cam->generateRay(scrCoord);
            if (cam->getErrCode()==2) {
                cout<<"error:out of range of [0,0] -> [1,1]"<<endl;
                return -1;
            }
            Hit h;
            if (gro->intersect(r,h,cam->getTMin())) {
                img.SetPixel(x,y,h.getMaterial()->getDiffuseColor());
                ts[getIndex(x,y)]=h.getT();
            }
            else {
                ts[getIndex(x,y)]=MAX_T;
            }
        }
    }
    img.SaveTGA(output_file);
    float nearest=MAX_T;
    float d=1.0/(depth_max-depth_min);
    for (int i=width*height-1;i>=0;i--) {
        if (ts[i]<nearest) nearest=ts[i];
    }
    for (int y=0;y<height;y++) {
        for (int x=0;x<width;x++) {
            float c=1.0-(ts[getIndex(x,y)]-nearest)*d;
            imgDep.SetPixel(x,y,Vec3f(c,c,c));
        }
    }
    imgDep.SaveTGA(depth_file);
    return 0;
}
int test() {
    SceneParser sp(input_file);
    Ray r(Vec3f(-0.5,0.5,10),Vec3f(0,0,-1));
    Group *gro=sp.getGroup();
    Hit h;
    Sphere sph(Vec3f(),1,NULL);
    Object3D *pObj=&sph;

    if (pObj->intersect(r,h,MIN_T)) {
        cout<<h.getT()<<endl;
    }
    else {
        cout<<"No intersection"<<endl;
    }
    if (gro->intersect(r,h,MIN_T)) {
        cout<<h.getT()<<endl;
    }
    else {
        cout<<"No intersection"<<endl;
    }
    return 0;
}
int main(int argc,char **argv) {
    parse_para(argc,argv);
    //test();
    solve();
    return 0;
}
