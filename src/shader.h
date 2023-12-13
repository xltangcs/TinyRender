#ifndef __SHADER__
#define __SHADER__

#include "tgaimage.h"
#include "geometry.h"
#include "model.h"

extern Model* model;

extern Matrix ModelView;
extern Matrix Viewport;
extern Matrix Projection;

extern Vec3f light_dir;
extern Vec3f       eye;
extern Vec3f    center;
extern Vec3f        up;

class Shader {
public:
    Shader() = default;
    ~Shader() = default;
    virtual Vec4f vertex(int iface, int nthvert) = 0;
    virtual bool fragment(Vec3f bar, TGAColor& color) = 0;
};


class GouraudShader : public Shader {
public:
    Vec3f varying_intensity;
    mat<2, 3, float> varying_uv;
    Vec4f vertex(int iface, int nthvert);
    bool fragment(Vec3f bar, TGAColor& color);
    GouraudShader() = default;
};

class ToonShader : public Shader {
public:
    mat<3, 3, float> varying_tri;
    Vec3f          varying_ity;
    Vec4f vertex(int iface, int nthvert);
    bool fragment(Vec3f bar, TGAColor& color);

    ToonShader() = default;
};

class FlatShader : public Shader {
public:
    mat<3, 3, float> varying_tri;
    Vec4f vertex(int iface, int nthvert);
    bool fragment(Vec3f bar, TGAColor& color);
    FlatShader() = default;
};

class PhongShader : public Shader {
public:
    mat<2, 3, float> varying_uv;  // same as above
    mat<4, 4, float> uniform_M = Projection * ModelView;
    mat<4, 4, float> uniform_MIT = ModelView.invert_transpose();
    Vec4f vertex(int iface, int nthvert);
    bool fragment(Vec3f bar, TGAColor& color);

    PhongShader() = default;
    ~PhongShader() = default;
};

#endif //__SHADER__

