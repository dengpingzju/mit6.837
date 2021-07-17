//This header file can only be included in a .cpp file,
//and never be included in .h file
#ifndef _H_MYUTIL_
#define _H_MYUTIL_

#include <Windows.h>
#include <GL/GL.h>
#include <GL/GLU.h>
#include <GL/glut.h>
#include "scene_parser.h"
#include "matrix.h"
#include "object3d.h"
#include "ray.h"
#include "hit.h"
#include "camera.h" 
#include "light.h"
#include "material.h"
#include "group.h" 
#include "sphere.h"
#include "plane.h"
#include "triangle.h"
#include "transform.h"
#include "ParamParser.h"
#include "image.h"
#include "glCanvas.h"
#include "RayTracer.h"
#include "rayTree.h"
#include "boundingbox.h"
#include "grid.h"
#include "raytracing_stats.h"
#endif // !_H_MYUTIL_

