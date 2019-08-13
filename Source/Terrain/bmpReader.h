#pragma once

#include "../Renderer.h"


class bmpReader
{

public:
	
	static bmpReader* getInstance();
	glm::vec3* LoadBitmapHeightMap(std::string m_terrainFilename);
	

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

	
	int m_vertexCount, m_indexCount;
	int m_terrainHeight, m_terrainWidth;
	float m_heightScale;
	char* m_terrainFilename;
	static glm::vec3* m_heightMap;
	ModelType* m_terrainModel;


};
