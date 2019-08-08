#include "bmpReader.h"
#include <windows.h>
#include <glm/gtx/normal.hpp>
using namespace glm;
using namespace std;

bmpReader::bmpReader()
{
	m_terrainHeight = 225;
	m_terrainWidth = 225;
	m_heightScale = 12.0;
	m_terrainFilename = 0;
	m_heightMap = 0;
	m_terrainModel = 0;
	mVAO = 0;
	mVBO = 0;

	// Initialize the terrain height map with the data from the bitmap file.
	LoadBitmapHeightMap();

	// Setup the X and Z coordinates for the height map as well as scale the terrain height by the height scale value.
	SetTerrainCoordinates();

	// Now build the 3D model of the terrain.
	BuildTerrainModel();

	// We can now release the height map since it is no longer needed in memory once the 3D terrain model has been built.
	ShutdownHeightMap();

	// Load the rendering buffers with the terrain data.
	InitializeBuffers();

	// Release the terrain model now that the rendering buffers have been loaded.
	ShutdownTerrainModel();
}


bool bmpReader::LoadBitmapHeightMap()
{
	int error, imageSize, i, j, k, index;
	FILE* filePtr;
	unsigned long long count;
	BITMAPFILEHEADER bitmapFileHeader;
	BITMAPINFOHEADER bitmapInfoHeader;
	unsigned char* bitmapImage;
	unsigned char height;

	// Start by creating the array structure to hold the height map data.
	m_heightMap = new HeightMapType[m_terrainWidth * m_terrainHeight];
	if (!m_heightMap)
	{
		return false;
	}

	// Open the bitmap map file in binary.
	error = fopen_s(&filePtr, m_terrainFilename, "rb");
	if (error != 0)
	{
		return false;
	}

	// Read in the bitmap file header.
	count = fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);
	if (count != 1)
	{
		return false;
	}

	// Read in the bitmap info header.
	count = fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);
	if (count != 1)
	{
		return false;
	}

	// Make sure the height map dimensions are the same as the terrain dimensions for easy 1 to 1 mapping.
	if ((bitmapInfoHeader.biHeight != m_terrainHeight) || (bitmapInfoHeader.biWidth != m_terrainWidth))
	{
		return false;
	}

	// Calculate the size of the bitmap image data.  
	// Since we use non-divide by 2 dimensions (eg. 257x257) we need to add an extra byte to each line.
	imageSize = m_terrainHeight * ((m_terrainWidth * 3) + 1);

	// Allocate memory for the bitmap image data.
	bitmapImage = new unsigned char[imageSize];
	if (!bitmapImage)
	{
		return false;
	}

	// Move to the beginning of the bitmap data.
	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

	// Read in the bitmap image data.
	count = fread(bitmapImage, 1, imageSize, filePtr);
	if (count != imageSize)
	{
		return false;
	}

	// Close the file.
	error = fclose(filePtr);
	if (error != 0)
	{
		return false;
	}

	// Initialize the position in the image data buffer.
	k = 0;

	// Read the image data into the height map array.
	for (j = 0; j < m_terrainHeight; j++)
	{
		for (i = 0; i < m_terrainWidth; i++)
		{
			// Bitmaps are upside down so load bottom to top into the height map array.
			index = (m_terrainWidth * (m_terrainHeight - 1 - j)) + i;

			// Get the grey scale pixel value from the bitmap image data at this location.
			height = bitmapImage[k];

			// Store the pixel value as the height at this point in the height map array.
			m_heightMap[index].position.y = (float)height;

			// Increment the bitmap image data index.
			k += 3;
		}

		// Compensate for the extra byte at end of each line in non-divide by 2 bitmaps (eg. 257x257).
		k++;
	}

	// Release the bitmap image data now that the height map array has been loaded.
	delete[] bitmapImage;
	bitmapImage = 0;

	// Release the terrain filename now that is has been read in.
	delete[] m_terrainFilename;
	m_terrainFilename = 0;

	return true;
}


void bmpReader::SetTerrainCoordinates()
{
	int i, j, index;

	// Loop through all the elements in the height map array and adjust their coordinates correctly.
	for (j = 0; j < m_terrainHeight; j++)
	{
		for (i = 0; i < m_terrainWidth; i++)
		{
			index = (m_terrainWidth * j) + i;

			// Set the X and Z coordinates.
			m_heightMap[index].position.x= (float)i;
			m_heightMap[index].position.z = -(float)j;

			// Move the terrain depth into the positive range.  For example from (0, -256) to (256, 0).
			m_heightMap[index].position.z += (float)(m_terrainHeight - 1);

			// Scale the height.
			m_heightMap[index].position.y /= m_heightScale;
		}
	}

	return;
}

//BuildTerrainModel is the function that takes the points in the height map 
//array and creates a 3D polygon mesh from them.It loops through the height 
//map array and grabs four points at a time and creates two triangles for those four points.
//The final 3D terrain model is stored in the m_terrainModel array.

bool bmpReader::BuildTerrainModel()
{
	int i, j, index, index1, index2, index3, index4;

	// Calculate the number of vertices in the 3D terrain model.
	m_vertexCount = (m_terrainHeight - 1) * (m_terrainWidth - 1) * 6;

	// Create the 3D terrain model array.
	m_terrainModel = new ModelType[m_vertexCount]; // modeltype is a struct of float x,y,z

	if (!m_terrainModel)
	{
		return false;
	}

	// Initialize the index into the height map array.
	index = 0;

	// Load the 3D terrain model with the height map terrain data.
	// We will be creating 2 triangles for each of the four points in a quad.
	for (j = 0; j < (m_terrainHeight - 1); j++)
	{
		for (i = 0; i < (m_terrainWidth - 1); i++)
		{
			// Get the indexes to the four points of the quad.
			index1 = (m_terrainWidth * j) + i;          // Upper left.
			index2 = (m_terrainWidth * j) + (i + 1);      // Upper right.
			index3 = (m_terrainWidth * (j + 1)) + i;      // Bottom left.
			index4 = (m_terrainWidth * (j + 1)) + (i + 1);  // Bottom right.

			//TODO . make j absolute? add color.

			glm::vec3 normalTriangle1 = glm::triangleNormal(m_heightMap[index1].position,
				m_heightMap[index2].position, m_heightMap[index3].position);
			glm::vec3 normalTriangle2 = glm::triangleNormal(m_heightMap[index3].position,
				m_heightMap[index2].position, m_heightMap[index4].position);

			// Now create two triangles for that quad.
			// Triangle 1 - Upper left.
			m_terrainModel[index].position = m_heightMap[index1].position;
			m_terrainModel[index].normal = normalTriangle1;
			index++;

			// Triangle 1 - Upper right.
			m_terrainModel[index].position = m_heightMap[index2].position;
			m_terrainModel[index].normal = normalTriangle1;
			index++;

			// Triangle 1 - Bottom left.
			m_terrainModel[index].position = m_heightMap[index3].position;
			m_terrainModel[index].normal = normalTriangle1;
			index++;

			// Triangle 2 - Bottom left.
			m_terrainModel[index].position = m_heightMap[index3].position;
			m_terrainModel[index].normal = normalTriangle2;
			index++;

			// Triangle 2 - Upper right.
			m_terrainModel[index].position = m_heightMap[index2].position;
			m_terrainModel[index].normal = normalTriangle2;
			index++;

			// Triangle 2 - Bottom right.
			m_terrainModel[index].position = m_heightMap[index4].position;
			m_terrainModel[index].normal = normalTriangle2;
			index++;
		}
	}

	return true;
}


void bmpReader::ShutdownHeightMap()
{
	// Release the height map array.
	if (m_heightMap)
	{
		delete[] m_heightMap;
		m_heightMap = 0;
	}
	return;
}


bool bmpReader::InitializeBuffers() //modified to match cubemodel.cpp
{
	VertexType* vertices = new VertexType[m_vertexCount];; //contains position vec3 andd color vec4
	vec4 color = vec4(1.0f, 1.0f, 1.0f, 1.0f);

	// Calculate the number of vertices in the terrain.
	m_vertexCount = (m_terrainWidth - 1) * (m_terrainHeight - 1) * 6;

	// Set the index count to the same as the vertex count.
	m_indexCount = m_vertexCount;

	unsigned long* indices = new unsigned long[m_indexCount];

	for (int i = 0; i < m_vertexCount; i++)
	{
		vertices[i].position = m_terrainModel[i].position;
		vertices[i].color = color;
		vertices[i].normal = m_terrainModel[i].normal;
		indices[i] = i;
	}

	if (!vertices||!indices)
	{
		return false;
	}

	// Create a vertex array
	glGenVertexArrays(1, &mVAO);
	glBindVertexArray(mVAO);

	// Upload Vertex Buffer to the GPU, keep a reference to it (mVertexBufferID)
	glGenBuffers(1, &mVBO);
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);


	// 1st attribute buffer : vertex Positions
	glVertexAttribPointer(0,                // attribute. No particular reason for 0, but must match the layout in the shader.
		3,                // size
		GL_FLOAT,        // type
		GL_FALSE,        // normalized?
		sizeof(ModelType), // stride
		(void*)0        // array buffer offset
	);
	glEnableVertexAttribArray(0);

	// 2nd attribute buffer : vertex normal
	glVertexAttribPointer(1,
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(vertices),
		(void*)sizeof(vec3)    // Normal is Offseted by vec3 (see class Vertex)
	);
	glEnableVertexAttribArray(1);


	// 3rd attribute buffer : vertex color
	glVertexAttribPointer(2,
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(vertices),
		(void*)(2 * sizeof(vec3)) // Color is Offseted by 2 vec3 (see class Vertex)
	);
	glEnableVertexAttribArray(2);

	return false;

}

void bmpReader::ShutdownTerrainModel()
{
	// Release the terrain model data.
	if (m_terrainModel)
	{
		delete[] m_terrainModel;
		m_terrainModel = 0;
	}

	return;
}






