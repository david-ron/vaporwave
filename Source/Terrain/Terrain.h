#pragma once

#include "../Renderer.h"
#include "../Model.h"

class Terrain : public Model
{
public:
	Terrain();
	virtual ~Terrain();

	void Update(float dt);
	void Draw(glm::mat4 offsetMatrix);


protected:
	virtual bool ParseLine(const std::vector<ci_string> &token);

private:


	//glm::vec3 HeightMapType[225*225] ;

	struct Vertex //from model
	{
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec3 color;
	};

	struct VertexType
	{
		glm::vec3 position;
		glm::vec3 color; //was vec4
		glm::vec3 normal;
	};

	//struct HeightMapType //change to vec3 later 
	//{
	//	glm::vec3 position;
	//};

	struct ModelType
	{
		glm::vec3 position, normal, color;
	};

	void SetTerrainCoordinates();
	bool BuildTerrainModel();
	void ShutdownTerrainModel();

	unsigned int mVAO;
	unsigned int mVBO;
	//unsigned int vertexCount;

	// to be changed
	unsigned int m_vertexCount, m_indexCount;
	int m_terrainHeight, m_terrainWidth;
	float m_heightScale;
	glm::vec3* m_heightMap;
	char* m_terrainFilename;
	Vertex* m_terrainModel;


};

