#pragma once

#include "../Renderer.h"

class bmpReader
{

public:

	static bmpReader* getInstance();
	glm::vec3* LoadBitMap(std::string bmpFile, int &twidth, int &theight);


private:

	struct ModelType
	{
		glm::vec3 position, normal;
	};

	static bmpReader * Instance;

	bmpReader();

	struct VertexType
	{
		glm::vec3 position;
		glm::vec4 color;
		glm::vec3 normal;
	};

	unsigned int mVAO;
	unsigned int mVBO;

	int vertexAmount;
	int terrainHeight, terrainWidth;
	float scaleHeight;
	char* bmpFile;
	static glm::vec3* heightMap;
	ModelType* terrain;
};
