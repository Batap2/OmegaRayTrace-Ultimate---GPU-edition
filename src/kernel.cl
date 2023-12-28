#include "src/OpenCL_src/Vec3.cl"
#include "src/OpenCL_src/Material.cl"
#include "src/OpenCL_src/Ray.cl"
#include "src/OpenCL_src/HitData.cl"
#include "src/OpenCL_src/Sphere.cl"
#include "src/OpenCL_src/Triangle.cl"
#include "src/OpenCL_src/Plane.cl"
#include "src/OpenCL_src/Light.cl"
#include "src/OpenCL_src/Scene.cl"




//Fonction principale du kernel -> Rendu par raytracing 'une image de la scène
__kernel void render(__global float* fb, int max_x, int max_y,  __global float* cameraData,__global float* vertices,__global unsigned int* indices, int numMesh,__global unsigned int* split_meshes,__global unsigned int* split_meshes_tri,__global float* materials, __global float* SkyColor)
{
	int i = get_global_id(0);
	int j = get_global_id(1);

	// TODO : importe la skycolor
	skyColor = (Vec3){SkyColor[0],SkyColor[1],SkyColor[2]};

	Vec3 cameraPos = (Vec3){cameraData[0], cameraData[1], cameraData[2]};
	Vec3 cameraDirection = (Vec3){cameraData[3+0], cameraData[3+1], cameraData[3+2]};
	Vec3 cameraRight = (Vec3){cameraData[6+0], cameraData[6+1], cameraData[6+2]};
	Vec3 cameraUp = cross(cameraDirection, cameraRight);
	float FOV = 60.;

    float aspectRatio = (float)max_x / (float)max_y;
    float tanFOV = tan(0.5 * radians(FOV));

    if ((i < max_x) && (j < max_y)) {
        int pixel_index = j * max_x * 3 + i * 3;


		// Calcul des coordonnées du rayon
		Ray ray;
		float u = (2.0 * ((float)i + 0.5) / (float)max_x - 1.0) * aspectRatio *tanFOV;
		float v = (1.0 - 2.0 * ((float)j + 0.5) / (float)max_y)* tanFOV;

		ray.direction = normalize(add(scale(cameraRight, u), add(scale(cameraUp, v), cameraDirection)));
		ray.origin = cameraPos;

		Scene scene;

		scene.numSpheres = 1;
		int x = 0;
		scene.spheres[0].center = (Vec3){0.6f,-0.7f,1.0f};
		scene.spheres[0].radius = 0.3f;


		scene.numPlanes = 0;
		scene.planes[0].center = (Vec3){0.0, 5.0, 0.0};
		scene.planes[0].normal = (Vec3){0.0, 1.0, 0.0};


		scene.numTriangles = 0;
		unsigned int offset_vertex = 0;
		unsigned int offset_index = 0;
		int mesh_num = numMesh;

		for(int mesh_id = 0; mesh_id < mesh_num; mesh_id++)
		{
			unsigned int vertex_nbr = split_meshes[mesh_id];
			unsigned int tri_nbr = split_meshes_tri[mesh_id];
			unsigned int index_nbr = tri_nbr *3;

            Material current_mat;
            unsigned int mat_id = mesh_id*13;
            current_mat.ambiant_color = (Vec3){materials[mat_id],materials[mat_id+1],materials[mat_id+2]};
            current_mat.diffuse_color = (Vec3){materials[mat_id+3],materials[mat_id+4],materials[mat_id+5]};
            current_mat.specular_color = (Vec3){materials[mat_id+6],materials[mat_id+7],materials[mat_id+8]};
            current_mat.shininess = materials[mat_id+9];
            current_mat.metallic = materials[mat_id+10];
            current_mat.roughness = materials[mat_id+11];
            current_mat.ao = materials[mat_id+12];

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
				scene.triangles[current_tri_id].vertex1 = s0;
				scene.triangles[current_tri_id].vertex2 = s1;
				scene.triangles[current_tri_id].vertex3 = s2;
                scene.triangles[current_tri_id].mat = current_mat;
				scene.numTriangles++;
			}
			offset_index += index_nbr;
			offset_vertex += vertex_nbr;
		}

		mainScene = &scene;


		testMat.diffuse_color = (Vec3){1.0f,0.2f,0.2f};
		testMat.metallic = 0.0f;
		testMat.roughness = 0.5f;

		testMat2.diffuse_color = (Vec3){0.5f,1.0f,0.2f};
		testMat2.metallic = 0.0f;
		testMat2.roughness = 0.5f;

		addLight((Vec3){-0.75f,1.0f,1.2f}, (Vec3){1.0f,0.8f,0.55f}, 0.1f);
		//addLight((Vec3){0.75f,1.0f,1.2f}, (Vec3){0.85f,0.95f,1.0f}, 0.1f);


		
		int bounce = 0;
	
		Vec3 out_color = computeColor(&ray, cameraPos, bounce);

		fb[pixel_index + 0] = out_color.x;
		fb[pixel_index + 1] = out_color.y;
		fb[pixel_index + 2] = out_color.z;

		
    }
}
