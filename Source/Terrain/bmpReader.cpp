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

vec3* bmpReader::LoadBMP(std::string bmpFile, int &twidth, int &theight)
{
	int size, step, index;
	step = 0;
	FILE* filePtr;
	unsigned long long count;
	BITMAPFILEHEADER fileHeader;
	BITMAPINFOHEADER infoHeader;
	unsigned char* image;
	unsigned char height;

	fopen_s(&filePtr, bmpFile.c_str(), "rb");
	fread(&fileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr); 
	fread(&infoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);

	terrainHeight = infoHeader.biHeight;
	terrainWidth = infoHeader.biWidth;
	theight = terrainHeight;
	twidth = terrainWidth;

	size = terrainHeight * ((terrainWidth * 3) + 1);
	image = new unsigned char[size];
	fseek(filePtr, fileHeader.bfOffBits, SEEK_SET); 
	fread(image, 1, size, filePtr);
	fclose(filePtr);

	heightMap = new glm::vec3[terrainWidth * terrainHeight];

	for (int i = 0; i < terrainHeight; i++)
	{
		for (int j = 0; j < terrainWidth; j++)
		{
			height = image[step];
			// makes the height of the terrain lower
			heightMap[(terrainWidth - 1 - i) * (terrainHeight)+j].y = (float)height / 6;
			step += 3;
		}
		step++;
	}

	delete[] image;

	return	heightMap;
}

bmpReader* bmpReader::getInstance() {
	if (Instance == nullptr)
		Instance = new bmpReader();
	return Instance;

}
