#pragma once

#include "../Renderer.h"

//struct HeightMapType //change to vec3 later 
//{
//	HeightMapType() {};
//	~HeightMapType() {};
//	glm::vec3 position;
//};



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

	

	


	//glm::vec3 HeightMapType[225 * 225];

	//to be changed

	//void ShutdownBuffers();
	//void RenderBuffers(ID3D11DeviceContext*);


	unsigned int mVAO;
	unsigned int mVBO;

	// to be changed
	int m_vertexCount, m_indexCount;
	int m_terrainHeight, m_terrainWidth;
	float m_heightScale;
	char* m_terrainFilename;
	static glm::vec3* m_heightMap;
	ModelType* m_terrainModel;


};
