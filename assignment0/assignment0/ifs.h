#ifndef _IFS_H_
#define _IFS_H_

#include "matrix.h"
#include "image.h"
#include <cstdlib>
#include <iostream>
using namespace std;
class IFS {
public:
	typedef Matrix* pMat;
	IFS(const char *file_name);
	IFS();
	~IFS();
	void myDecons();
	void readDescription(const char *file_name);
	void print();
	float drand();
    void render(Image *img,int points,int iter);
	pMat* trans;
	float* prob;
private:
	int good_state;//0:normal;-1:error occured
	int n;

};


#endif
