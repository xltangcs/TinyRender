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

        }

        ~Shaders()
        {

        };

        float flatShader(TGAColor& color, Vec3f baryCoord)
        {
            float intensity = 0.0;
            Vec3f fragment_normal = (worldCoords[2]-worldCoords[0])^(worldCoords[1]-worldCoords[0]); 
            fragment_normal.normalize();
            intensity = fragment_normal * light_dir; 
            color = TGAColor(255* intensity, 255* intensity, 255* intensity, 255);
            return intensity;
        }

        float gouraudShader(TGAColor& color, Vec3f baryCoord)
        {
            Vec2f uvp = uv[0] * baryCoord.x + uv[1] * baryCoord.y + uv[2] * baryCoord.z;
            TGAColor c = model->diffuse(uvp);

            Vec3f vertIntensity;
            for(int i=0; i<3; i++)
            {
                vertIntensity[i] = std::max(normal[i].normalize() * light_dir, 0.f);
            }
            vertIntensity.normalize();
            float intensity = vertIntensity * baryCoord;
            

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

            color = c * intensity;
            return intensity;
        }
};

