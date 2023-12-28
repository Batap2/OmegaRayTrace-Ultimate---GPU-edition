#include "src/OpenCL_src/Vec3.cl"
#include "src/OpenCL_src/Material.cl"
#include "src/OpenCL_src/Ray.cl"
#include "src/OpenCL_src/HitData.cl"
#include "src/OpenCL_src/Sphere.cl"
#include "src/OpenCL_src/Triangle.cl"
#include "src/OpenCL_src/Plane.cl"
#include "src/OpenCL_src/Light.cl"
#include "src/OpenCL_src/Scene.cl"
#include "src/OpenCL_src/Camera.cl"

// GLOBAL VARIABLE

Camera mainCamera;
//Scene *mainScene;
//const int LIGHT_MAX_NUMBER = 32;
//Light lights[LIGHT_MAX_NUMBER];
//int lightNumber = 0;
// Vec3 skyColor;
//const int MESH_MAX_NUMBER = 1000;
//Material materials[MESH_MAX_NUMBER];

//To update the camera
__kernel void updateCamera(int max_x, int max_y,__global float* cameraData)
{
    int i = get_global_id(0);
    int j = get_global_id(1);

    if(i==0 && j == 0)
    {
        mainCamera.cameraPos = (Vec3){cameraData[0], cameraData[1], cameraData[2]};
        mainCamera.cameraDirection = (Vec3){cameraData[3+0], cameraData[3+1], cameraData[3+2]};
        mainCamera.cameraRight = (Vec3){cameraData[6+0], cameraData[6+1], cameraData[6+2]};
        mainCamera.cameraUp = cross(mainCamera.cameraDirection, mainCamera.cameraRight);
        mainCamera.FOV = 60.;
        mainCamera.aspectRatio = (float)max_x / (float)max_y;
        mainCamera.tanFOV = tan(0.5 * radians(mainCamera.FOV));
    }
}

//To update the skyColor
__kernel void updateSkyColor(int max_x, int max_y,__global float* SkyColor)
{
    int i = get_global_id(0);
    int j = get_global_id(1);

    if(i==0 && j == 1)
    {
        skyColor = (Vec3){SkyColor[0],SkyColor[1],SkyColor[2]};
    }
}

//To update materials
__kernel void updateMaterials(int max_x, int max_y, int mesh_nbr, __global float* materialsData) {
    int i = get_global_id(0);
    int j = get_global_id(1);

    if (i == 0 && j == 2) {
        for(int mesh_id =0; mesh_id < mesh_nbr; mesh_id++)
        {
            unsigned int mat_id = mesh_id*13;
            materials[mesh_id].ambiant_color = (Vec3){materialsData[mat_id],materialsData[mat_id+1],materialsData[mat_id+2]};
            materials[mesh_id].diffuse_color = (Vec3){materialsData[mat_id+3],materialsData[mat_id+4],materialsData[mat_id+5]};
            materials[mesh_id].specular_color = (Vec3){materialsData[mat_id+6],materialsData[mat_id+7],materialsData[mat_id+8]};
            materials[mesh_id].shininess = materialsData[mat_id+9];
            materials[mesh_id].metallic = materialsData[mat_id+10];
            materials[mesh_id].roughness = materialsData[mat_id+11];
            materials[mesh_id].ao = materialsData[mat_id+12];
        }
    }
}

//To load all data requiered to initialize the kernel
__kernel void loading(int max_x, int max_y,__global float* cameraData,__global float* SkyColor, int mesh_nbr,__global float* materialsData,__global float* vertices,__global unsigned int* indices,__global unsigned int* split_meshes,__global unsigned int* split_meshes_tri)
{
    int i = get_global_id(0);
    int j = get_global_id(1);

    //Loading camera
    if( i==0 && j == 0)
    {
        mainCamera.cameraPos = (Vec3){cameraData[0], cameraData[1], cameraData[2]};
        mainCamera.cameraDirection = (Vec3){cameraData[3+0], cameraData[3+1], cameraData[3+2]};
        mainCamera.cameraRight = (Vec3){cameraData[6+0], cameraData[6+1], cameraData[6+2]};
        mainCamera.cameraUp = cross(mainCamera.cameraDirection, mainCamera.cameraRight);
        mainCamera.FOV = 60.;
        mainCamera.aspectRatio = (float)max_x / (float)max_y;
        mainCamera.tanFOV = tan(0.5 * radians(mainCamera.FOV));
    }

    //Loading SkyColor
    if(i==1 && j ==0)
    {
        skyColor = (Vec3){SkyColor[0],SkyColor[1],SkyColor[2]};
    }

    //Loading Material
    if(i==2 && j==0)
    {
        for(int mesh_id =0; mesh_id < mesh_nbr; mesh_id++)
        {
            unsigned int mat_id = mesh_id*13;
            materials[mesh_id].ambiant_color = (Vec3){materialsData[mat_id],materialsData[mat_id+1],materialsData[mat_id+2]};
            materials[mesh_id].diffuse_color = (Vec3){materialsData[mat_id+3],materialsData[mat_id+4],materialsData[mat_id+5]};
            materials[mesh_id].specular_color = (Vec3){materialsData[mat_id+6],materialsData[mat_id+7],materialsData[mat_id+8]};
            materials[mesh_id].shininess = materialsData[mat_id+9];
            materials[mesh_id].metallic = materialsData[mat_id+10];
            materials[mesh_id].roughness = materialsData[mat_id+11];
            materials[mesh_id].ao = materialsData[mat_id+12];
        }
    }

    //Loading Scene - mostly Triangles
    if(i==2 && j==0)
    {

		mainScene2.numSpheres = 0;
        mainScene2.numPlanes = 0;
        mainScene2.numTriangles = 0;

        unsigned int offset_vertex = 0;
        unsigned int offset_index = 0;

        for(int mesh_id = 0; mesh_id < mesh_nbr; mesh_id++)
        {
            unsigned int vertex_nbr = split_meshes[mesh_id];
            unsigned int tri_nbr = split_meshes_tri[mesh_id];
            unsigned int index_nbr = tri_nbr *3;

            for(int tri = 0; tri < tri_nbr; tri++)
            {
                int current_id = offset_index + tri*3 ;
                int id0 = current_id;
                int id1 = current_id + 1;
                int id2 = current_id + 2;

                Vec3 s0 = (Vec3){vertices[(offset_vertex+indices[id0])*3+0],vertices[(offset_vertex+indices[id0])*3+1],vertices[(offset_vertex+indices[id0])*3+2]};
                Vec3 s1 = (Vec3){vertices[(offset_vertex+indices[id1])*3+0],vertices[(offset_vertex+indices[id1])*3+1],vertices[(offset_vertex+indices[id1])*3+2]};
                Vec3 s2 = (Vec3){vertices[(offset_vertex+indices[id2])*3+0],vertices[(offset_vertex+indices[id2])*3+1],vertices[(offset_vertex+indices[id2])*3+2]};

                int current_tri_id =tri + offset_index/3;
                mainScene2.triangles[current_tri_id].vertex1 = s0;
                mainScene2.triangles[current_tri_id].vertex2 = s1;
                mainScene2.triangles[current_tri_id].vertex3 = s2;
                mainScene2.triangles[current_tri_id].mat = mesh_id;
                mainScene2.numTriangles++;
            }
            offset_index += index_nbr;
            offset_vertex += vertex_nbr;
        }

    }

}


//Fonction principale du kernel -> Rendu par raytracing 'une image de la scène
__kernel void render(__global float* fb, int max_x, int max_y)
{
	int i = get_global_id(0);
	int j = get_global_id(1);

    if ((i < max_x) && (j < max_y)) {
        int pixel_index = j * max_x * 3 + i * 3;

		// Calcul des coordonnées du rayon
		Ray ray = getRayfromCamera(mainCamera,i,j,max_x,max_y);

		addLight((Vec3){-0.75f,1.0f,1.2f}, (Vec3){1.0f,0.8f,0.55f}, 0.1f);
		//addLight((Vec3){0.75f,1.0f,1.2f}, (Vec3){0.85f,0.95f,1.0f}, 0.1f);

		
		int bounce = 0;
	
		Vec3 out_color = computeColor(&ray, mainCamera.cameraPos, bounce);

		fb[pixel_index + 0] = out_color.x;
		fb[pixel_index + 1] = out_color.y;
		fb[pixel_index + 2] = out_color.z;

		
    }
}
