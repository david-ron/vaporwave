#pragma once

#include <vector>
#include <list>
#include <GL/gl.h>
#include <glm/glm.hpp>
#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <stdlib.h>
#include <GLFW/glfw3.h>

struct Pixel
{	//position contains, (x,y(intensity),z)
	glm::vec3 position;
	glm::vec4 color;
};

//need a list of Pixel that will represent all the pixels in the terrainbmp.







class Terrain
{
public:
	//loading image based off textureloader
	static int LoadTerrain(const char * imagepath); 

private:

};

