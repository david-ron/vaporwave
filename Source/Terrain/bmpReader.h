#pragma once

#include "../Renderer.h"

class bmpReader
{
private:
	struct VertexType
	{
		glm::vec3 position;
		glm::vec4 color;
		glm::vec3 normal;
	};

	struct HeightMapType //change to vec3 later 
	{
		glm::vec3 position;
	};

	struct ModelType
	{
		glm::vec3 position, normal;
	};

public:
	bmpReader();

	//to be changed
	//TerrainClass(const TerrainClass&);
	//~TerrainClass();

	//bool Initialize(ID3D11Device*, char*);
	//void Shutdown();
	//bool Render(ID3D11DeviceContext*);
	//int GetIndexCount();

private:
	bool LoadBitmapHeightMap();
	void SetTerrainCoordinates();
	bool BuildTerrainModel(); 
	void ShutdownHeightMap();

	bool InitializeBuffers();
	void ShutdownTerrainModel();




	//to be changed

	//void ShutdownBuffers();
	//void RenderBuffers(ID3D11DeviceContext*);

private:
	unsigned int mVAO;
	unsigned int mVBO;

	// to be changed
	int m_vertexCount, m_indexCount;
	int m_terrainHeight, m_terrainWidth;
	float m_heightScale;
	char* m_terrainFilename;
	HeightMapType* m_heightMap;
	ModelType* m_terrainModel;


};
