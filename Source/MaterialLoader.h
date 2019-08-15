#pragma once
#include <glm/glm.hpp>
#include <cstring>
#include <vector>
#include <string>
class MaterialLoader
{
public:
	//Currently only used inside objLoader as an addition to the load obj

	static bool loadMaterial(const char * path, glm::vec4 material);
};

