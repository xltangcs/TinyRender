#include <iostream>
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"

extern Model *model;
extern Vec3f light_dir;
class Shaders{
    public:
        int iface;
        int nvert;
        Vec3f screenCoords[3]; 
        Vec3f worldCoords[3];
        Vec2f uv[3];
        Vec3f normal[3];

        Vec3f varying_intensity;
        mat<2, 3, float> varying_uv;

        Shaders()
        {
            iface = 0;
            nvert = 0;

            screenCoords[0] = Vec3f(0.0f, 0.0f, 0.0f);
	        screenCoords[1] = Vec3f(0.0f, 0.0f, 0.0f);
	        screenCoords[2] = Vec3f(0.0f, 0.0f, 0.0f);

            worldCoords[0] = Vec3f(0.0f, 0.0f, 0.0f);
	        worldCoords[1] = Vec3f(0.0f, 0.0f, 0.0f);
	        worldCoords[2] = Vec3f(0.0f, 0.0f, 0.0f);

            uv[0] = Vec2f(0.0, 0.0);
            uv[1] = Vec2f(0.0, 0.0);
            uv[2] = Vec2f(0.0, 0.0);

	        normal[0] = Vec3f(0.0f, 0.0f, 0.0f);
	        normal[1] = Vec3f(0.0f, 0.0f, 0.0f);
	        normal[2] = Vec3f(0.0f, 0.0f, 0.0f);

            varying_intensity = Vec3f(0.0f, 0.0f, 0.0f);

        }

        ~Shaders()
        {

        };

        void vertex(int iface, int nthvert)
        {
            varying_uv.set_col(nthvert, model->uv(iface, nthvert));
            varying_intensity[nthvert] = std::max(0.f, model->normal(iface, nthvert)*(light_dir.normalize())); // get diffuse lighting intensity
        }
        

        bool FlatShader(TGAColor& color, Vec3f baryCoord)
        {
            Vec2f uv = varying_uv * baryCoord; //插值后的uv坐标
            TGAColor c = model->diffuse(uv); //获取uv贴图
            Vec3f fragment_normal = cross((worldCoords[2]-worldCoords[0]), (worldCoords[1]-worldCoords[0])).normalize();
            float intensity = fragment_normal * light_dir; 
            color = c * intensity;
            return false;
        }

        bool GouraudShader(TGAColor& color, Vec3f baryCoord)
        {
            Vec2f uv = varying_uv * baryCoord; //插值后的uv坐标
            TGAColor c = model->diffuse(uv); //获取uv贴图
            float intensity = varying_intensity * baryCoord; //插值后的光照强度
            color = c * intensity; //该点应有的颜色：纹理*光强
            return false;
            

            // float intensity1 = 0.0;
            // Vec3f fragment_normal = (worldCoords[2]-worldCoords[0])^(worldCoords[1]-worldCoords[0]); 
            // fragment_normal.normalize();
            // intensity1 = fragment_normal * light_dir; 

            // if (baryCoord.x < 0 || baryCoord.y < 0 || baryCoord.z < 0 )
            //     return 0.5;
            // if(intensity1 !=  intensity && intensity1 > 0 && intensity > 0)
            // {
            //     std::cout<<"intensity = "<< intensity <<std::endl;
            //     std::cout<<"intensity1 = "<< intensity1 <<std::endl;
            // }
        }

        bool ToonShader(TGAColor& color, Vec3f baryCoord)
        {
           float intensity = varying_intensity * baryCoord;
            if (intensity>.85) intensity = 1;
            else if (intensity>.60) intensity = .80;
            else if (intensity>.45) intensity = .60;
            else if (intensity>.30) intensity = .45;
            else if (intensity>.15) intensity = .30;
            color = TGAColor(255, 155, 0)*intensity;
            return false;
        }
};