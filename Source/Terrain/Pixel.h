#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <list>

// A billboard has a position and a size in world units, and a texture
struct Pixel
{
	glm::vec3 position;
	glm::vec2 size;
	glm::vec4 color;
};

//struct HeightMapPosition {
//	float x, y, z;
//};
//
//struct ModelPosition {
//
//	float x, y, z;
//
//};


class PixelList
{
public:
	PixelList(unsigned int maxNumPixels, int textureID);
	~PixelList();

	void AddPixel(PixelList* b);
	void RemovePixel(PixelList* b);

	void Update(float dt);
	//void Draw(glm::mat4 offsetMatrix);
	void Draw(glm::mat4 offsetMatrix);

private:
	// Each vertex on a billboard
	struct PixelVertex
	{
		//after figuring out the mesh stuff. not sure if I will keept
		glm::vec3 position;
		
		glm::vec3 normal;

		//the y or height
		glm::vec1 intensity;

		// this should be hardcoded for the vertex color. something light
		glm::vec4 color;
		//x and 'z' (i guess we see it as x and y in usual 2d view)
		glm::vec2 textureCoordinate;
	};

	std::vector<PixelVertex> mVertexBuffer;
	std::list<Pixel*> mPixelList;

	int mTextureID;

	unsigned int mMaxNumPixels;

	unsigned int mVAO;
	unsigned int mVBO;
};
