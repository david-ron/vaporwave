#include "Pixel.h"
#include "../Renderer.h"
#include "../World.h"
#include "../WorldBlock.h"
#include "../Camera.h"
#include "../StaticCamera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/common.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include "../LightSource.h"
#include <vector>

using namespace std;
using namespace std;
using namespace glm;

PixelList::PixelList(unsigned int maxNumPixels, int textureID)
: mTextureID(textureID), mMaxNumPixels(maxNumPixels)
{
		// light source info
		LightSource lightSource = LightSource();

		// Pre-allocate Vertex Buffer - 6 vertices by Pixel (2 triangles). think of a sqaure 'pixel'. divide it in 2 triangles.
		mVertexBuffer.resize(maxNumPixels * 6);

		// Initialize vertices to white, with UVs taking all the texture
		for (int i = 0; i < (int)mVertexBuffer.size(); ++i)
		{
			// White vertex
			mVertexBuffer[i].color = vec4(1.0f, 1.0f, 1.0f, 1.0f); //SETTING COLOR OF VERTEIXES

			// Normal on Z axis
			mVertexBuffer[i].normal = vec3(0.0f, 0.0f, 1.0f);

			// Texture coordinates
			switch (i % 6)
			{
			case 0: mVertexBuffer[i].textureCoordinate = vec2(0.0f, 1.0f); break;
			case 1: mVertexBuffer[i].textureCoordinate = vec2(0.0f, 0.0f); break;
			case 2: mVertexBuffer[i].textureCoordinate = vec2(1.0f, 1.0f); break;
			case 3: mVertexBuffer[i].textureCoordinate = vec2(1.0f, 1.0f); break;
			case 4: mVertexBuffer[i].textureCoordinate = vec2(0.0f, 0.0f); break;
			case 5: mVertexBuffer[i].textureCoordinate = vec2(1.0f, 0.0f); break;
			}
		}

		// Create a vertex array
		glGenVertexArrays(1, &mVAO);
		glBindVertexArray(mVAO);

		// Upload Vertex Buffer to the GPU, keep a reference to it (mVertexBufferID)
		glGenBuffers(1, &mVBO);
		glBindBuffer(GL_ARRAY_BUFFER, mVBO); 
		glBufferData(GL_ARRAY_BUFFER, mVertexBuffer.size() * sizeof(PixelVertex), &mVertexBuffer[0], GL_STATIC_DRAW);
		Renderer::CheckForErrors();

		// 1st attribute buffer : vertex Positions
		glVertexAttribPointer(0,                // attribute. No particular reason for 0, but must match the layout in the shader.
			3,                // size
			GL_FLOAT,        // type
			GL_FALSE,        // normalized?
			sizeof(PixelVertex), // stride
			(void*)0        // array buffer offset
		);
		glEnableVertexAttribArray(0);

		// 2nd attribute buffer : vertex normal
		glVertexAttribPointer(1,
			3,
			GL_FLOAT,
			GL_FALSE,
			sizeof(PixelVertex),
			(void*)sizeof(vec3)    // Normal is Offseted by vec3 (see class Vertex)
		);
		glEnableVertexAttribArray(1);


		// 3rd attribute buffer : vertex color
		glVertexAttribPointer(2,
			4,
			GL_FLOAT,
			GL_FALSE,
			sizeof(PixelVertex),
			(void*)(2 * sizeof(vec3)) // Color is Offseted by 2 vec3 (see class Vertex)
		);
		glEnableVertexAttribArray(2);

		// 3rd attribute buffer : texture coordinates
		glVertexAttribPointer(3,
			2,
			GL_FLOAT,
			GL_FALSE,
			sizeof(PixelVertex),
			(void*)(2 * sizeof(vec3) + sizeof(vec4)) // texture coordinates are Offseted by 2 vec3 (see class Vertex) and a vec4
		);
		glEnableVertexAttribArray(3);

















}







//do i need this?
PixelList::~PixelList()
{
	// Free the GPU from the Vertex Buffer
	glDeleteBuffers(1, &mVBO);
	glDeleteVertexArrays(1, &mVAO);

	mVertexBuffer.resize(0);
	mPixelList.resize(0);
}



void PixelList::AddPixel(PixelList * b)
{
}

void PixelList::RemovePixel(PixelList * b)
{
}

//i dont think i need this?
void PixelList::Update(float dt)
{
}

void PixelList::Draw(glm::mat4 offsetMatrix)
{
}
