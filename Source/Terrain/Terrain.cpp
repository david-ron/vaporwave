#include "Terrain.h"
#include <windows.h>
#include <glm/gtx/normal.hpp>
#include "bmpReader.h"
#include "../World.h"

using namespace glm;
using namespace std;

Terrain::Terrain()
{
	terrainHeight = 0;
	terrainWidth = 0;

	scaleHeight = 12.0;
	terrain = 0;

	mVAO = 0;
	mVBO = 0;

	bmpFile = "../Assets/Textures/terrain.bmp";
	heightMap = bmpReader::getInstance()->LoadBMP(bmpFile, terrainWidth, terrainHeight);
	SetTerrainPosition();
	CreateTerrain();
}

Terrain::~Terrain()
{
	glDeleteBuffers(1, &mVBO);
	glDeleteVertexArrays(1, &mVAO);
}

void Terrain::Update(float dt)
{
	Model::Update(dt);
}

void Terrain::Draw(glm::mat4 offsetMatrix)
{
	glBindVertexArray(mVAO);
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	GLuint WorldMatrixLocation = glGetUniformLocation(Renderer::GetShaderProgramID(), "WorldTransform");
	glm::mat4 WorldMatrix = offsetMatrix;
	glUniformMatrix4fv(WorldMatrixLocation, 1, GL_FALSE, &WorldMatrix[0][0]);
	GLuint MaterialID = glGetUniformLocation(Renderer::GetShaderProgramID(), "materialCoefficients");
	glUniform4f(MaterialID, 0.2f, 0.8f, 0.2f, 50);
	GLuint IsTerrainID = glGetUniformLocation(Renderer::GetShaderProgramID(), "isTerrain");
	glUniform1i(IsTerrainID, 1);
	glDrawArrays(GL_TRIANGLES, 0, vertexAmount); 

}

bool Terrain::ParseLine(const std::vector<ci_string>& token)
{
	if (token.empty())
	{
		return true;
	}
	else
	{
		return Model::ParseLine(token);
	}
}

void Terrain::SetTerrainPosition()
{
	int index;

	for (int i = 0; i < terrainHeight; i++)
	{
		for (int j = 0; j < terrainWidth; j++)
		{
			index = (terrainWidth * i) + j;

			heightMap[index].x = (float)j;
			heightMap[index].z = -(float)i;

			heightMap[index].z += (float)(terrainHeight - 1);

			heightMap[index].y /= scaleHeight;
			heightMap[index].y *= 5;
		}
	}
}

void Terrain::CreateTerrain()
{
	int it, step, index, upperLeft, upperRight, bottomLeft, bottomRight;
	vertexAmount = (terrainHeight - 1) * (terrainWidth - 1) * 6;
	terrain = new Vertex[vertexAmount]; // modeltype is a struct of float x,y,z
	glm::vec3 color;
	index = 0;
	it = 0;
	step = 8;
	

	for (int i = 0; i < (terrainHeight - 1); i++)
	{
		if (i%step == 0) {
			it += step;
		}

		for (int j = 0; j < (terrainWidth - 1); j++)
		{
			upperLeft = (terrainWidth * i) + j;         
			upperRight = (terrainWidth * i) + (j + 1);      
			bottomLeft = (terrainWidth * (i + 1)) + j;     
			bottomRight = (terrainWidth * (i + 1)) + (j + 1); 

			glm::vec3 normalTriangle1 = glm::triangleNormal(heightMap[upperLeft],
				heightMap[upperRight], heightMap[bottomLeft]);

			glm::vec3 normalTriangle2 = glm::triangleNormal(heightMap[bottomLeft],
				heightMap[upperRight], heightMap[bottomRight]);

			if (it % (2 * step) < step) {
				color = glm::vec3(0.7f, 0.0f, 0.9f);
			}else{
				color = glm::vec3(0.7f, 0.7f, 1.0f);
			}

			it++;

			terrain[index].position = heightMap[upperLeft] - vec3(World::WorldBlockSize / 2, 0, World::WorldBlockSize / 2);
			terrain[index].normal = normalTriangle1;
			terrain[index].color = color;
			index++;

			terrain[index].position = heightMap[upperRight] - vec3(World::WorldBlockSize / 2, 0, World::WorldBlockSize / 2);
			terrain[index].normal = normalTriangle1;
			terrain[index].color = color;
			index++;

			terrain[index].position = heightMap[bottomLeft] - vec3(World::WorldBlockSize / 2, 0, World::WorldBlockSize / 2);
			terrain[index].normal = normalTriangle1;
			terrain[index].color = color;
			index++;

			terrain[index].position = heightMap[bottomLeft] - vec3(World::WorldBlockSize / 2, 0, World::WorldBlockSize / 2);
			terrain[index].normal = normalTriangle2;
			terrain[index].color = color;
			index++;

			terrain[index].position = heightMap[upperRight] - vec3(World::WorldBlockSize / 2, 0, World::WorldBlockSize / 2);
			terrain[index].normal = normalTriangle2;
			terrain[index].color = color;
			index++;

			terrain[index].position = heightMap[bottomRight] - vec3(World::WorldBlockSize / 2, 0, World::WorldBlockSize / 2);
			terrain[index].normal = normalTriangle2;
			terrain[index].color = color;
			index++;
		}
		
	}
	delete[] heightMap;
	glGenVertexArrays(1, &mVAO);
	glBindVertexArray(mVAO);
	glGenBuffers(1, &mVBO);
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*(terrainWidth - 1)*(terrainHeight - 1) * 6, terrain, GL_STATIC_DRAW);


	// 1st attribute buffer : vertex Positions
	glVertexAttribPointer(0,                
		3,                // size
		GL_FLOAT,        // type
		GL_FALSE,        // normalized?
		sizeof(Vertex), // stride
		(void*)0        // array buffer offset
	);
	glEnableVertexAttribArray(0);

	// 2nd attribute buffer : vertex normal
	glVertexAttribPointer(1,
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(Vertex),
		(void*)sizeof(vec3)    
	);
	glEnableVertexAttribArray(1);


	// 3rd attribute buffer : vertex color
	glVertexAttribPointer(2,
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(Vertex),
		(void*)(2 * sizeof(vec3)) 
	);
	glEnableVertexAttribArray(2);

}

void Terrain::getHightAndNormal(const vec3 coor, float& hight, vec3& normal) {
	float minDistance = INFINITY;
	float cDistance;
	float cHight;
	vec3 cNormal;

	vec2 mcPosition = vec2(coor.x, coor.z);
	vec2 vPosition;

	for (int i = 0; i < vertexAmount; i++) {
		// change the coordinate to 2d vec2
		vPosition = vec2(terrain[i].position.x, terrain[i].position.z);
		// if the current vertex is further then what have already found
		cDistance = length(vPosition - mcPosition);
		if (cDistance >= minDistance)
			continue;// we dont care

		minDistance = cDistance;
		cHight = terrain[i].position.y;
		cNormal = terrain[i].normal;
	}

	hight = cHight;
	normal = cNormal;
}