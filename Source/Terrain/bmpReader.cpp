#include "bmpReader.h"
#include <windows.h>
#include <glm/gtx/normal.hpp>
#include <string>
using namespace glm;
using namespace std;
#include <iostream>

bmpReader* bmpReader::Instance = nullptr;
glm::vec3* bmpReader::heightMap;

bmpReader::bmpReader()
{
	terrainHeight = 0;
	terrainWidth = 0;
	heightMap = 0;
}


vec3* bmpReader::LoadBitMap(std::string bmpFile, int &twidth, int &theight)
{
	int error, imageSize, k, index;
	FILE* filePtr;
	unsigned long long count;
	BITMAPFILEHEADER bitmapFileHeader;
	BITMAPINFOHEADER bitmapInfoHeader;
	unsigned char* bitmapImage;
	unsigned char height;

	fopen_s(&filePtr, bmpFile.c_str(), "rb");
	fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);
	fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);

	terrainHeight = bitmapInfoHeader.biHeight;
	terrainWidth = bitmapInfoHeader.biWidth;
	theight = terrainHeight;
	twidth = terrainWidth;

	imageSize = terrainHeight * ((terrainWidth * 3) + 1);
	bitmapImage = new unsigned char[imageSize];
	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);
	fread(bitmapImage, 1, imageSize, filePtr);
	fclose(filePtr);

	// Initialize the position in the image data buffer.
	k = 0;

	heightMap = new glm::vec3[terrainWidth * terrainHeight];
	// Read the image data into the height map array.
	for (int j = 0; j < terrainHeight; j++)
	{
		for (int i = 0; i < terrainWidth; i++)
		{
			index = (terrainWidth * (terrainHeight - 1 - j)) + i;
			height = bitmapImage[k];
			heightMap[index].y = (float)height / 6;
			k += 3;
		}
		k++;
	}
	delete[] bitmapImage;
	bitmapImage = 0;
	return	heightMap;
}



bmpReader* bmpReader::getInstance() {
	if (Instance == nullptr)
		Instance = new bmpReader();
	return Instance;

}
