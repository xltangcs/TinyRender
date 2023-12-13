#include <cmath>
#include <limits>
#include <cstdlib>
#include <algorithm>
#include <iostream>

#include "shader.h"

Vec4f GouraudShader::vertex(int iface, int nthvert) {
    
    Vec4f gl_Vertex = embed<4>(model->vert(iface, nthvert));
    varying_uv.set_col(nthvert, model->uv(iface, nthvert));
    mat<4, 4, float> uniform_M = Projection * ModelView;
    mat<4, 4, float> uniform_MIT = ModelView.invert_transpose();
    gl_Vertex = Viewport * uniform_M * gl_Vertex;
    Vec3f normal = proj<3>(embed<4>(model->normal(iface, nthvert))).normalize();
    varying_intensity[nthvert] = (std::max)(0.f, model->normal(iface, nthvert) * light_dir); // get diffuse lighting intensity
    return gl_Vertex;
}

bool GouraudShader::fragment(Vec3f bar, TGAColor& color) {
    Vec2f uv = varying_uv * bar;
    TGAColor c = model->diffuse(uv);
    float intensity = varying_intensity * bar;
    color = c * intensity;
    return false;
}
Vec4f ToonShader::vertex(int iface, int nthvert) {
    Vec4f gl_Vertex = embed<4>(model->vert(iface, nthvert));
    gl_Vertex = Projection * ModelView * gl_Vertex;
    varying_tri.set_col(nthvert, proj<3>(gl_Vertex / gl_Vertex[3]));
    varying_ity[nthvert] = model->normal(iface, nthvert) * light_dir;
    gl_Vertex = Viewport * gl_Vertex;
    return gl_Vertex;
}
bool ToonShader::fragment(Vec3f bar, TGAColor& color) {
    float intensity = varying_ity * bar;
    if (intensity > .85) intensity = 1;
    else if (intensity > .60) intensity = .80;
    else if (intensity > .45) intensity = .60;
    else if (intensity > .30) intensity = .45;
    else if (intensity > .15) intensity = .30;
    color = TGAColor(255, 155, 0) * intensity;
    return false;
}
Vec4f FlatShader::vertex(int iface, int nthvert) {
    Vec4f gl_Vertex = embed<4>(model->vert(iface, nthvert));
    gl_Vertex = Projection * ModelView * gl_Vertex;
    varying_tri.set_col(nthvert, proj<3>(gl_Vertex / gl_Vertex[3]));
    gl_Vertex = Viewport * gl_Vertex;
    return gl_Vertex;
}
bool FlatShader::fragment(Vec3f bar, TGAColor& color) {
    Vec3f n = cross(varying_tri.col(1) - varying_tri.col(0), varying_tri.col(2) - varying_tri.col(0)).normalize();
    float intensity = n * light_dir;
    color = TGAColor(255, 255, 255) * intensity;
    return false;
}
Vec4f PhongShader::vertex(int iface, int nthvert) {
    varying_uv.set_col(nthvert, model->uv(iface, nthvert));
    Vec4f gl_Vertex = embed<4>(model->vert(iface, nthvert)); // read the vertex from .obj file
    return Viewport * Projection * ModelView * gl_Vertex; // transform it to screen coordinates
}
bool PhongShader::fragment(Vec3f bar, TGAColor& color) {
    Vec2f uv = varying_uv * bar;
    Vec3f n = proj<3>(uniform_MIT * embed<4>(model->normal(uv))).normalize();
    Vec3f l = proj<3>(uniform_M * embed<4>(light_dir)).normalize();
    Vec3f r = (n * (n * l * 2.f) - l).normalize();   // reflected light
    float spec = pow(std::max(r.z, 0.0f), model->specular(uv));
    float diff = std::max(0.f, n * l);
    TGAColor c = model->diffuse(uv);
    color = c;
    for (int i = 0; i < 3; i++) color[i] = std::min<float>(5 + c[i] * (diff + .6 * spec), 255);
    return false;
}


