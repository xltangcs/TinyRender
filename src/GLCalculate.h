#ifndef __CALCULATE_H__
#define __CALCULATE_H__

#include "geometry.h"
#include "shader.h"

void viewport(int x, int y, int w, int h);
void projection(float coeff=0.f); // coeff = -1/c
void lookat(Vec3f eye, Vec3f center, Vec3f up);

void triangle(Vec4f *pts, Shader &shader, TGAImage &image, TGAImage &zbuffer);


#endif //__CALCULATE_H__
