#include <iostream>
#include "ifs.h"
#include <cstring>
using namespace std;
char *input_file = NULL;
int num_points = 10000;
int num_iters = 10;
int size = 100;
char *output_file = NULL;
void readPara(int argc,char **argv) {
    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i],"-input")) {
            i++; assert (i < argc);
            input_file = argv[i];
        } else if (!strcmp(argv[i],"-points")) {
            i++; assert (i < argc);
            num_points = atoi(argv[i]);
        } else if (!strcmp(argv[i],"-iters")) {
            i++; assert (i < argc);
            num_iters = atoi(argv[i]);
        } else if (!strcmp(argv[i],"-size")) {
            i++; assert (i < argc);
            size = atoi(argv[i]);
        } else if (!strcmp(argv[i],"-output")) {
            i++; assert (i < argc);
            output_file = argv[i];
        } else {
            printf ("whoops error with command line argument %d: '%s'\n",i,argv[i]);
            assert(0);
        }
    }
}
int main(int argc,char **argv) {
    readPara(argc,argv);
	IFS ifs(input_file);
    Image img(size,size);
    img.SetAllPixels(Vec3f(1,1,1));
    ifs.render(&img,num_points,num_iters);
    img.SaveTGA(output_file);
	return 0;
}
