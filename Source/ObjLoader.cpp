//
//  ObjLoader.cpp
//  COMP371_Framework
//
//  Created by David Ronci on 2019-08-09.
//  Copyright © 2019 Concordia. All rights reserved.
//

#include "ObjLoader.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>

bool ObjLoader::loadOBJ(
             const char * path,
             std::vector<glm::vec3> & out_vertices,
             std::vector<glm::vec3> & out_normals,
             std::vector<glm::vec2> & out_uvs,
             glm::vec3 & max,
             glm::vec3 & min,
			 glm::vec4 & mtl) {
    
    std::vector<int> vertexIndices, uvIndices, normalIndices;
    std::vector<glm::vec3> temp_vertices;
    std::vector<glm::vec2> temp_uvs;
    std::vector<glm::vec3> temp_normals;
	std::vector<glm::vec4> temp_mtl;

    max = glm::vec3(-INFINITY);
    min = glm::vec3(INFINITY);
	char MtlFile[128];
    FILE * file;
    file = fopen(path, "r");
    if (!file) {
        printf("Impossible to open the file ! Are you in the right path ?\n");
        printf(path);
        return false;
    }
    
    while (1) {
        
        char lineHeader[128];
        // read the first word of the line
        int res = fscanf(file, "%s", lineHeader);
        if (res == EOF)
            break; // EOF = End Of File. Quit the loop.
        
        // else : parse lineHeader
        
        if (strcmp(lineHeader, "v") == 0) {
            glm::vec3 vertex;
            fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
            temp_vertices.push_back(vertex);

			if (vertex.x > max.x)
				max.x = vertex.x;
			if (vertex.y > max.y)
				max.y = vertex.y;
			if (vertex.z > max.z)
				max.z = vertex.z;
			if (vertex.x < min.x)
				min.x = vertex.x;
			if (vertex.y < min.y)
				min.y = vertex.y;
			if (vertex.z < min.z)
				min.z = vertex.z;
        }
		else if (strcmp(lineHeader, "mtllib") == 0) {
			//fgets(line, 128, file);
			fscanf(file, "%s\n", MtlFile);
		}
        else if (strcmp(lineHeader, "vt") == 0) {
            glm::vec2 uv;
            fscanf(file, "%f %f\n", &uv.x, &uv.y);
            uv.y = -uv.y; // Invert V coordinate since we will only use DDS texture, which are inverted. Remove if you want to use TGA or BMP loaders.
            temp_uvs.push_back(uv);
        }
        else if (strcmp(lineHeader, "vn") == 0) {
            glm::vec3 normal;
            fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
            temp_normals.push_back(normal);

        }
        else if (strcmp(lineHeader, "f") == 0) {
            int vertexIndex[3], uvIndex[3], normalIndex[3];
            bool uv = true;
            bool norm = true;
            char line[128];
            fgets(line, 128, file);
            //vertex, uv, norm
            int matches = sscanf(line, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
            if (matches != 9) {
                //vertex, norm
                matches = sscanf(line, "%d//%d %d//%d %d//%d\n", &vertexIndex[0], &normalIndex[0], &vertexIndex[1], &normalIndex[1], &vertexIndex[2], &normalIndex[2]);
                if (matches != 6) {
                    //vertex, uv
                    matches = sscanf(line, "%d/%d %d/%d %d/%d\n", &vertexIndex[0], &uvIndex[0], &vertexIndex[1], &uvIndex[1], &vertexIndex[2], &uvIndex[2]);
                    if (matches != 6) {
                        //vertex
                        matches = sscanf(line, "%d %d %d\n", &vertexIndex[0], &vertexIndex[1], &vertexIndex[2]);
                        if (matches != 6) {
                            printf("File can't be read by our simple parser. 'f' format expected: d/d/d d/d/d d/d/d || d/d d/d d/d || d//d d//d d//d\n");
                            printf("Character at %ld", ftell(file));
                            return false;
                        }
                        uv, norm = false;
                    }
                    else {
                        norm = false;
                    }
                }
                else {
                    uv = false;
                }
            }
            vertexIndices.push_back(vertexIndex[0]);
            vertexIndices.push_back(vertexIndex[1]);
            vertexIndices.push_back(vertexIndex[2]);
            if (norm) {
                normalIndices.push_back(normalIndex[0]);
                normalIndices.push_back(normalIndex[1]);
                normalIndices.push_back(normalIndex[2]);
            }
            if (uv) {
                uvIndices.push_back(uvIndex[0]);
                uvIndices.push_back(uvIndex[1]);
                uvIndices.push_back(uvIndex[2]);
            }
        }
        else {
            char clear[1000];
            fgets(clear, 1000, file);
        }
        
    }

    for (unsigned int i = 0; i < vertexIndices.size(); i++) {
        if (uvIndices.size() != 0) {
            if (i < uvIndices.size()) {
                unsigned int uvIndex = abs(uvIndices[i]);
                glm::vec2 uv = temp_uvs[uvIndex - 1];
                out_uvs.push_back(uv);
            }
        }
        if (normalIndices.size() != 0) {
            if (i < normalIndices.size()) {
                unsigned int normalIndex = abs(normalIndices[i]);
                glm::vec3 normal = temp_normals[normalIndex - 1];
                out_normals.push_back(normal);
            }
        }
        
        unsigned int vertexIndex = abs(vertexIndices[i]);
        glm::vec3 vertex = temp_vertices[vertexIndex - 1];
        out_vertices.push_back(vertex);
    }
	//ka kd ks have rgb values but we will just use the first value
	// they are all the same in our case... its just to read them all in
	;
	file = fopen((std::string("../Assets/Models/")+std::string(MtlFile)).c_str(), "r");
	glm::vec3 temp_ka;
	glm::vec3 temp_kd;
	glm::vec3 temp_ks;
	glm::vec3 temp_ke;
	while (1) {

		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break;
		if (strcmp(lineHeader, "Ka") == 0) {
			fscanf(file, "%f %f %f\n", &temp_ka.x, &temp_ka.y, &temp_ka.z);
		}
		else if (strcmp(lineHeader, "Kd") == 0) {
			fscanf(file, "%f %f\n", &temp_kd.x, &temp_kd.y, &temp_kd.z);
		}
		else if (strcmp(lineHeader, "Ks") == 0) {
			fscanf(file, "%f %f %f\n", &temp_ks.x, &temp_ks.y, &temp_ks.z);
		}
		else if (strcmp(lineHeader, "Ke") == 0) {
			fscanf(file, "%f %f %f\n", &temp_ke.x, &temp_ke.y, &temp_ke.z);
		}
	}
	mtl.x = temp_ka.x;
	mtl.y = temp_kd.x;
	mtl.z = temp_ks.x;
	mtl.w = temp_ks.x;
    return true;
}

