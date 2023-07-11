#include <vector>
#include <iostream>
#include <cmath>
#include <cstdlib>
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"
#include "shaders.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);
const TGAColor green = TGAColor(0,   255, 0,   255);
const TGAColor blue  = TGAColor(0,   0,   255, 255);

Model *model = NULL;
const int width  = 800;
const int height = 800;
const int depth = 255;
Vec3f light_dir(0, 0, -1); // define light_dir
Vec3f cameraPos(0, 0, 3);
Vec3f worldUp(0, 1, 0);
Vec3f centerPosition(0, 0, 1);

Vec3f barycentric(Vec3f* pts, Vec3f P) {
    float xa = pts[0].x;
    float ya = pts[0].y;
    float xb = pts[1].x;
    float yb = pts[1].y;
    float xc = pts[2].x;
    float yc = pts[2].y;
    float x = P.x;
    float y = P.y;

    float gamma = static_cast<float>((ya - yb) * x + (xb - xa) * y + xa * yb - xb * ya) / static_cast<float>((ya - yb) * xc + (xb - xa) * yc + xa * yb - xb * ya);
    float beta = static_cast<float>((ya - yc) * x + (xc - xa) * y + xa * yc - xc * ya) / static_cast<float>((ya - yc) * xb + (xc - xa) * yb + xa * yc - xc * ya);
    float alpha = 1 - gamma - beta;
    
    return Vec3f(alpha, beta, gamma);
}

Vec3f world2Scrern(Vec3f v)
{
    return Vec3f((int)((v.x+1.0)*width/2.0), (int)((v.y+1.0)*height/2.0), v.z);
}

int findIndex(int x, int y)
{
    return x + y * width;
}

bool isinside(Vec2i p, Vec2i t0, Vec2i t1, Vec2i t2)    //wrong
{
    Vec2i t01 = {t1.x-t0.x, t1.y-t0.y};
    Vec2i t12 = {t2.x-t1.x, t2.y-t1.y};
    Vec2i t20 = {t0.x-t2.x, t0.y-t2.y};
    Vec2i t0p = {p.x-t0.x,  p.y-t0.y };
    Vec2i t1p = {p.x-t1.x,  p.y-t1.y };
    Vec2i t2p = {p.x-t2.x,  p.y-t2.y };

    if((t01.x*t0p.y-t01.y*t0p.x > 0) || (t12.x*t1p.y-t12.y*t1p.x > 0) || (t20.x*t2p.y-t20.y*t2p.x > 0))
        return false;
    return true;
}

void line(Vec2i p0, Vec2i p1, TGAImage& image, const TGAColor& color)
{
    int x0 = p0.x, y0 = p0.y;
    int x1 = p1.x, y1 = p1.y;
    bool steep = false;
    if(std::abs(x0-x1) < std::abs(y0-y1))
    {
        steep = true;
        std::swap(x0,y0);
        std::swap(x1,y1);
    }
    if(x0>x1)
    {
        std::swap(x0,x1);
        std::swap(y0,y1);
    }
    int dx = x1 - x0;
    int dy = std::abs(y1 - y0);
    int incrE = 2 * dy;
    int incrNE = 2 * (dy - dx);
    int d = 2 * dy - dx;
    int y = y0;
    
    for(int x = x0; x<= x1; x++)
    {
        if(steep)
        {
            image.set(y, x, color);
        }
        else{
            image.set(x, y, color);
        }
        if(d < 0)
        {
            d += incrE;
        }
        else{
            y += (y1 > y0 ? 1 : -1);
            d += incrNE;
        }
    }
}

Matrix local2homo(Vec3f v)  // to homogeneous coordinates
{
    Matrix m(4, 1);
    m[0][0] = v.x;
    m[1][0] = v.y;
    m[2][0] = v.z;
    m[3][0] = 1.0f;
    return m;
}

Vec3f homo2vertices(Matrix m)
{
    return Vec3f(m[0][0], m[1][0], m[2][0]);
}

//模型变换矩阵
Matrix modelMatrix()
{
    return Matrix::identity(4);
}

//视图变换矩阵
Matrix viewMatrix(Vec3f camera, Vec3f worldup, Vec3f center)
{
    Matrix rotation = Matrix::identity(4);
    Matrix translation = Matrix::identity(4);

    Vec3f z = (camera - center).normalize();
    Vec3f x = (worldup ^ z).normalize();
    Vec3f y = (z ^ x).normalize();

    translation[0][3] = -center[0];
    translation[1][3] = -center[1];
    translation[2][3] = -center[2];
    for(int i=0; i<3; i++)
    {
        rotation[0][i] = x[i];
        rotation[1][i] = y[i];
        rotation[2][i] = z[i];
    }

    return rotation * translation;
}

//透视投影变换矩阵
Matrix projectionMatrix()
{
    Matrix projection = Matrix::identity(4);
    projection[3][2] = -1.0f / cameraPos.z;
    return projection;
}

Matrix projectionDivision(Matrix m) 
{
    m[0][0] = m[0][0] / m[3][0];
    m[1][0] = m[1][0] / m[3][0];
    m[2][0] = m[2][0] / m[3][0];
    m[3][0] = 1.0f;
    return m;
}

Matrix viewportMatrix(int x, int y, int w, int h) {
    Matrix m = Matrix::identity(4);
    m[0][3] =  x + w / 2.f;
    m[1][3] =  y + h / 2.f;
    m[2][3] = depth / 2.f;

    m[0][0] = w / 2.f;
    m[1][1] = h / 2.f;
    m[2][2] = depth / 2.f;
    return m;
}

void triangle(Shaders shader, float *zBuffer, TGAImage& image)
{
    Vec3f t0 = shader.screenCoords[0], t1 = shader.screenCoords[1], t2 = shader.screenCoords[2];
    float f_max_x = std::max(t0.x,std::max(t1.x, t2.x));
    float f_max_y = std::max(t0.y,std::max(t1.y, t2.y));
    float f_min_x = std::min(t0.x,std::min(t1.x, t2.x));
    float f_min_y = std::min(t0.y,std::min(t1.y, t2.y));

    int max_x = std::ceil(f_max_x);
    int max_y = std::ceil(f_max_y);
    int min_x = std::floor(f_min_x);
    int min_y = std::floor(f_min_y);

    for(int x=min_x;x<=max_x;x++)
    {
        for(int y=min_y;y<=max_y;y++)
        {
            Vec3f p = {x, y, 0.0};
            Vec3f baryCoord = barycentric(shader.screenCoords, p);
            TGAColor color;
            float intensity = shader.gouraudShader(color, baryCoord);
            if (baryCoord.x < 0 || baryCoord.y < 0 || baryCoord.z < 0 || intensity < 0)
                continue;
            float z_interpolation = baryCoord.x * shader.screenCoords[0].z + baryCoord.y * shader.screenCoords[1].z + baryCoord.z * shader.screenCoords[2].z;
            if(z_interpolation > zBuffer[findIndex(x, y)])
            {
                zBuffer[findIndex(x, y)] = z_interpolation;
                image.set(x, y, color);
            }
        }
    }
}


int main(int argc, char** argv) 
{
    if (2 == argc) {
        model = new Model(argv[1]);
    }
    else {
        model = new Model("../obj/african_head/african_head.obj");
    }
    
    TGAImage image(width, height, TGAImage::RGB);

    Matrix model_ = modelMatrix();
    Matrix view_ = viewMatrix(cameraPos, worldUp, centerPosition);
    Matrix projection_ = projectionMatrix();
    Matrix viewport_ = viewportMatrix(width / 8, height / 8, width * 3 / 4, height * 3 / 4);

    float *zBuffer = new float[width*height];
    for (int i = 0; i < width * height; i++)
    {
        zBuffer[i] = -std::numeric_limits<float>::max();
    }

    for (int i=0; i<model->nfaces(); i++) {
        Shaders shader;
        std::vector<int> face = model->face(i); 
        for (int j=0; j<3; j++) {
            shader.iface = i;
            shader.nvert = j;
            Vec3f v = model->vert(face[j]);
            shader.worldCoords[j]  = v;
            shader.screenCoords[j] = homo2vertices(viewport_ * projectionDivision(projection_ * view_ * model_ * local2homo(v)));
            shader.uv[j] = model->uv(i, j);
            shader.normal[j] = model->normal(i, j).normalize();
        }
        triangle(shader, zBuffer, image);
    }
 
    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image.write_tga_file("gouraudShader.tga");
    delete model;
    return 0;
}
