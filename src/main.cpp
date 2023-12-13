#include <vector>
#include <iostream>
#include <algorithm>
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"
#include "shader.h"
#include "GLCalculate.h"

Model *model     = NULL;
const int width  = 800;
const int height = 800;

Vec3f light_dir(0,1,1);
Vec3f       eye(1,0.5,1.5);
Vec3f    center(0,0,0);
Vec3f        up(0,1,0);


int main(int argc, char** argv) {
    //load model
    if (2==argc) {
        model = new Model(argv[1]);
    } else {
        model = new Model("../../../obj/african_head/african_head.obj");
    }

    lookat(eye, center, up);
    projection(-1.f/(eye-center).norm());
    viewport(width / 8, height / 8, width * 3 / 4, height * 3 / 4);
    light_dir.normalize();

    TGAImage image  (width, height, TGAImage::RGB);
    TGAImage zbuffer(width, height, TGAImage::GRAYSCALE);

    //GouraudShader shader;
    //PhongShader shader;
	ToonShader shader;

    for (int i=0; i<model->nfaces(); i++) 
    {
        Vec4f screen_coords[3];
        for (int j=0; j<3; j++) 
        {
            screen_coords[j] = shader.vertex(i, j);
        }
        triangle(screen_coords, shader, image, zbuffer);
    }

    image.  flip_vertically();
    zbuffer.flip_vertically();
    image.  write_tga_file("ToonShader.tga");
    //zbuffer.write_tga_file("zbuffer.tga");

    delete model;
    return 0;
}
