#include "bmpReader.h"
#include <windows.h>
#include <glm/gtx/normal.hpp>
#include <string>
using namespace glm;
using namespace std;
#include <iostream>

bmpReader* bmpReader::Instance = nullptr;
glm::vec3* bmpReader::m_heightMap ;

bmpReader::bmpReader()
{

	m_terrainHeight = 225;
	m_terrainWidth = 225;
	m_heightMap = new glm::vec3[m_terrainWidth * m_terrainHeight];


}


vec3* bmpReader::LoadBitmapHeightMap(std::string m_terrainFilename )
{
	int error, imageSize, k, index;
	FILE* filePtr;
	unsigned long long count;
	BITMAPFILEHEADER bitmapFileHeader;
	BITMAPINFOHEADER bitmapInfoHeader;
	unsigned char* bitmapImage;
	unsigned char height;

	if (!m_heightMap)
	{
		return false;
	}

	// Open the bitmap map file in binary.
	error = fopen_s(&filePtr, m_terrainFilename.c_str(), "rb");
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

	cout << bitmapInfoHeader.biHeight;

	// Make sure the height map dimensions are the same as the terrain dimensions for easy 1 to 1 mapping.
	if ((bitmapInfoHeader.biHeight != m_terrainHeight) || (bitmapInfoHeader.biWidth != m_terrainWidth))
	{
		return false;
	}

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
	for (int j = 0; j < m_terrainHeight; j++)
	{
		for (int i = 0; i < m_terrainWidth; i++)
		{
			// Bitmaps are upside down so load bottom to top into the height map array.
			index = (m_terrainWidth * (m_terrainHeight - 1 - j)) + i;

			// Get the grey scale pixel value from the bitmap image data at this location.
			height = bitmapImage[k];

			// Store the pixel value as the height at this point in the height map array.
			m_heightMap[index].y = (float)height / 6;

			// Increment the bitmap image data index.
			k += 3;
		}

		// Compensate for the extra byte at end of each line in non-divide by 2 bitmaps (eg. 257x257).
		k++;
	}
	
	// Release the bitmap image data now that the height map array has been loaded.
	delete[] bitmapImage;
	bitmapImage = 0;
		
	return	m_heightMap;


}



bmpReader* bmpReader::getInstance() {
	if (Instance == nullptr)
		Instance = new bmpReader();
	return Instance;
	
}
