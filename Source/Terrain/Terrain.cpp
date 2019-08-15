#include "Terrain.h"
#include <windows.h>
#include <glm/gtx/normal.hpp>
#include "bmpReader.h"
#include "../World.h"

using namespace glm;
using namespace std;

Terrain::Terrain()
{
	m_terrainHeight = 225;
	m_terrainWidth = 225;

	m_heightScale = 12.0;
	m_terrainModel = 0;

	mVAO = 0;
	mVBO = 0;

	m_terrainFilename = "../Assets/Textures/terrain.bmp";
	//m_terrainFilename = "../Assets/Textures/flower.bmp";

	m_heightMap = bmpReader::getInstance()->LoadBitmapHeightMap(m_terrainFilename);

	
	SetTerrainCoordinates();
	BuildTerrainModel();

	
	vec3 color = vec3(1.0f, 1.0f, 1.0f); 
					
	m_indexCount = m_vertexCount;

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

	// Draw the triangles !

	glDrawArrays(GL_TRIANGLES, 0, m_vertexCount); // 36 vertices: 3 * 2 * 6 (3 per triangle, 2 triangles per face, 6 faces)
	
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



void Terrain::SetTerrainCoordinates()
{
	int i, j, index;

	// Loop through all the elements in the height map array and adjust their coordinates correctly.
	for (j = 0; j < m_terrainHeight; j++)
	{
		for (i = 0; i < m_terrainWidth; i++)
		{
			index = (m_terrainWidth * j) + i;

			// Set the X and Z coordinates.
			m_heightMap[index].x = (float)i;
			m_heightMap[index].z = -(float)j;

			// Move the terrain depth into the positive range.  For example from (0, -256) to (256, 0).
			m_heightMap[index].z += (float)(m_terrainHeight - 1);

			// Scale the height.
			m_heightMap[index].y /= m_heightScale;
			m_heightMap[index].y *= 5;
		}
	}


	return;
}

bool Terrain::BuildTerrainModel()
{
	int i, j, index, index1, index2, index3, index4;

	// Calculate the number of vertices in the 3D terrain model.
	m_vertexCount = (m_terrainHeight - 1) * (m_terrainWidth - 1) * 6;

	// Create the 3D terrain model array.
	m_terrainModel = new Vertex[m_vertexCount]; // modeltype is a struct of float x,y,z

	if (!m_terrainModel)
	{
		return false;
	}

	index = 0;
	
	int it = 0;
	int step = 8;
	vector<vec3> testPostion; 
	for (j = 0; j < (m_terrainHeight - 1); j++)
	{
		if(j%step == 0)
			it += step;
		for (i = 0; i < (m_terrainWidth - 1); i++)
		{
			
			// Get the indexes to the four points of the quad.
			index1 = (m_terrainWidth * j) + i;          // Upper left.
			index2 = (m_terrainWidth * j) + (i + 1);      // Upper right.
			index3 = (m_terrainWidth * (j + 1)) + i;      // Bottom left.
			index4 = (m_terrainWidth * (j + 1)) + (i + 1);  // Bottom right.

			glm::vec3 normalTriangle1 = glm::triangleNormal(m_heightMap[index1],
				m_heightMap[index2], m_heightMap[index3]);

			glm::vec3 normalTriangle2 = glm::triangleNormal(m_heightMap[index3],
				m_heightMap[index2], m_heightMap[index4]);

			glm::vec3 color;
			
			if(it%(2*step)<step)
				color = glm::vec3(0.7f, 0.0f, 0.9f);
			else
				color = glm::vec3(0.7f, 0.7f, 1.0f);

			if (it % step == 0 || j % step == 0)
				color = glm::vec3(0.4, 0.8, 0.4);

			it++;
			// Now create two triangles for that quad.
			// Triangle 1 - Upper left.

			m_terrainModel[index].position = m_heightMap[index1] - vec3(World::WorldBlockSize / 2, 0, World::WorldBlockSize / 2);
			testPostion.push_back(m_terrainModel[index].position);
			m_terrainModel[index].normal = normalTriangle1;
			m_terrainModel[index].color = color;
			index++;

			// Triangle 1 - Upper right.
			m_terrainModel[index].position = m_heightMap[index2] - vec3(World::WorldBlockSize / 2, 0, World::WorldBlockSize / 2);
			testPostion.push_back(m_terrainModel[index].position);
			m_terrainModel[index].normal = normalTriangle1;
			m_terrainModel[index].color = color;
			index++;

			// Triangle 1 - Bottom left.
			m_terrainModel[index].position = m_heightMap[index3] - vec3(World::WorldBlockSize / 2, 0, World::WorldBlockSize / 2);
			testPostion.push_back(m_terrainModel[index].position);
			m_terrainModel[index].normal = normalTriangle1;
			m_terrainModel[index].color = color;
			index++;

			// Triangle 2 - Bottom left.
			m_terrainModel[index].position = m_heightMap[index3] - vec3(World::WorldBlockSize / 2, 0, World::WorldBlockSize / 2);
			testPostion.push_back(m_terrainModel[index].position);
			m_terrainModel[index].normal = normalTriangle2;
			m_terrainModel[index].color = color;
			index++;

			// Triangle 2 - Upper right.
			m_terrainModel[index].position = m_heightMap[index2] - vec3(World::WorldBlockSize / 2, 0, World::WorldBlockSize / 2);
			testPostion.push_back(m_terrainModel[index].position);
			m_terrainModel[index].normal = normalTriangle2;
			m_terrainModel[index].color = color;
			index++;

			// Triangle 2 - Bottom right.
			m_terrainModel[index].position = m_heightMap[index4] - vec3(World::WorldBlockSize / 2, 0, World::WorldBlockSize / 2);
			testPostion.push_back(m_terrainModel[index].position);
			m_terrainModel[index].normal = normalTriangle2;
			m_terrainModel[index].color = color;
			index++;
		}
	}

	//delete[] m_heightMap;

	glGenVertexArrays(1, &mVAO);
	glBindVertexArray(mVAO);

	// Upload Vertex Buffer to the GPU, keep a reference to it (mVertexBufferID)
	glGenBuffers(1, &mVBO);
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*(m_terrainWidth-1)*(m_terrainHeight-1)*6, m_terrainModel, GL_STATIC_DRAW);


	// 1st attribute buffer : vertex Positions
	glVertexAttribPointer(0,                // attribute. No particular reason for 0, but must match the layout in the shader.
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
		(void*)sizeof(vec3)    // Normal is Offseted by vec3 (see class Vertex)
	);
	glEnableVertexAttribArray(1);


	// 3rd attribute buffer : vertex color
	glVertexAttribPointer(2,
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(Vertex),
		(void*)(2 * sizeof(vec3)) // Color is Offseted by 2 vec3 (see class Vertex)
	);
	glEnableVertexAttribArray(2);
	//glBindVertexArray(0); //disconnects VAO

	//m_vertexCount = 36;
	return true;
}




void Terrain::ShutdownTerrainModel()
{
	// Release the terrain model data.
	if (m_terrainModel)
	{
		delete[] m_terrainModel;
		m_terrainModel = 0;
	}

	return;
}


void Terrain::getHightAndNormal(const vec3 coor, float& hight, vec3& normal) {
	float minDistance = INFINITY;
	float cDistance;
	float cHight;
	vec3 cNormal;

	vec2 mcPosition = vec2(coor.x, coor.z);
	vec2 vPosition;

	for (int i = 0; i < m_vertexCount; i++) {
		// change the coordinate to 2d vec2
		vPosition = vec2(m_terrainModel[i].position.x, m_terrainModel[i].position.z);
		// if the current vertex is further then what have already found
		cDistance = length(vPosition - mcPosition);
		if (cDistance >= minDistance)
			continue;// we dont care

		minDistance = cDistance;
		cHight = m_terrainModel[i].position.y;
		cNormal = m_terrainModel[i].normal;
 	}



	hight = cHight;
	normal = cNormal;

	//cout << "(" << coor.x << " ," << coor.z << ") :" << cHight << endl;
}