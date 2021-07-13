#include "ifs.h"
#include "matrix.h"
#include <cstdio>
#include <ctime>
#include <cstdlib>
IFS::IFS(const char *file_name) {
    good_state=-1;
    srand(time(0));
	readDescription(file_name);
}
IFS::IFS() {
    good_state=-1;
    srand(time(0));
}
void IFS::myDecons() {
    delete [] prob;
	for (int i=0;i<n;i++) {
		delete trans[i];
	}
	delete [] trans;
	good_state=-1;
}
IFS::~IFS() {
    myDecons();
}
void IFS::readDescription(const char *file_name) {
    if (good_state==0) {
        myDecons();
    }
    FILE* fp=fopen(file_name,"r");
	if (!fp) {
		good_state=-1;
		return;
	}
	else {
		fscanf(fp,"%d",&n);
		if (n<=0) {
			good_state=-1;
			return;
		}
		trans=new pMat[n];
		prob=new float[n];
		float data;
		for (int c=0;c<n;c++) {
            trans[c]=new Matrix();
			fscanf(fp,"%f",prob+c);
			for (int i=0;i<3;i++) {
				for (int j=0;j<3;j++) {
                    fscanf(fp,"%f",&data);
					trans[c]->Set(j,i,data);
				}
			}
		}
	}
	fclose(fp);
	good_state=0;
}
void IFS::print() {
	if (good_state==-1) {
		cout<<"error has occured"<<endl;
		return;
	}
	cout<<n<<endl;
	for (int c=0;c<n;c++) {
		cout<<prob[c]<<endl;
		for (int i=0;i<3;i++) {
			for (int j=0;j<3;j++) {
				cout<<trans[c]->Get(j,i)<<" ";
			}
			cout<<endl;
		}
	}
}
float IFS::drand() {
    return (float)rand()/(RAND_MAX+1.0);
}
void IFS::render(Image *img,int points,int iter) {
    int w=img->Width();
    int h=img->Height();
    for (int i=0;i<points;i++) {
        float x=drand(),y=drand();
        Vec2f p(x,y);
        for (int j=0;j<iter;j++) {
            float dr=drand();
            float l=0.0;
            for (int k=0;k<n;k++) {
                if (dr>=l&&dr<l+prob[k]) {
                    trans[k]->Transform(p);
                    break;
                }
                l+=prob[k];
            }
        }
        Vec3f color(0,0,0);
        int ix=p.x()*w,iy=p.y()*h;
        if (ix<0||ix>=w||iy<0||iy>=h) {
            //cout<<ix<<" "<<iy<<endl;
            //cout<<"coordinates out of range!"<<endl;
            //return;
            continue;
        }
        img->SetPixel(p.x()*w,p.y()*h,color);
    }
}
