#include "MaterialLoader.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>

bool MaterialLoader::loadMaterial(const char * path ,glm::vec4 mtl)
{
	FILE * file;
	file = fopen(path, "r");
	if (!file) {
		printf("Impossible to open the file ! Are you in the right path ?\n");
		printf(path);
		return false;
	}
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

