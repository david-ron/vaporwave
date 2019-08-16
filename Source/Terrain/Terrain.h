#pragma once

#include "../Renderer.h"
#include "../Model.h"

using namespace std;
using namespace glm;

class Terrain : public Model
{
public:
	Terrain();
	virtual ~Terrain();

	void Update(float dt);
	void Draw(glm::mat4 offsetMatrix);

	void getHightAndNormal(const vec3 coor, float& hight, vec3& normal);

protected:
	virtual bool ParseLine(const std::vector<ci_string> &token);

private:
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


	void SetTerrainPosition();
	void CreateTerrain();


	unsigned int mVAO;
	unsigned int mVBO;



	unsigned int vertexAmount;
	int terrainHeight, terrainWidth;
	float scaleHeight;
	glm::vec3* heightMap;
	char* bmpFile;
	Vertex* terrain;


};

