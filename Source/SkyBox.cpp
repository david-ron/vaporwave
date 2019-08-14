//
//  SkyBox.cpp
//  COMP371_Framework
//
//  Created by David Ronci on 2019-08-10.
//  Copyright © 2019 Concordia. All rights reserved.
//

#include "SkyBox.hpp"
#include "Renderer.h"
#include <iostream>
#include <vector>
#include <FreeImageIO.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

SkyBox::SkyBox(glm::vec3 size){

	// phase out for unit square objloaded
    float skyboxVertices[] = {
        // positions
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        
        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,
        
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        
        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,
        
        -1.0f,  1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,
        
        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f,  1.0f
    };
    
    glGenVertexArrays(1, &mVAO);
    glGenBuffers(1, &mVBO);
    glBindVertexArray(mVAO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
#if defined(PLATFORM_OSX)
    std::string skyboxprefix = "Textures/";
#else
    std::string skyboxprefix = "../Assets/Textures/";
#endif
  
    faces.push_back(skyboxprefix + "purplevalley_rt.tga");
    faces.push_back(skyboxprefix + "purplevalley_lf.tga");
    faces.push_back(skyboxprefix + "purplevalley_up.tga");
    faces.push_back(skyboxprefix + "purplevalley_dn.tga" );
    faces.push_back(skyboxprefix + "purplevalley_bk.tga" );
    faces.push_back(skyboxprefix + "purplevalley_ft.tga" );
   
    loadCubemap(faces);
    
    glBindVertexArray(0);
}

void SkyBox::loadCubemap(std::vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, useless;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &useless, 0);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
			0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		stbi_image_free(data);	
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	cubeMapId= textureID;
}
SkyBox::~SkyBox()
{
    // Free the GPU from the Vertex Buffer
    glDeleteBuffers(1, &mVBO);
    glDeleteVertexArrays(1, &mVAO);
}


void SkyBox::Draw(glm::mat4 offsetMatrix){


	//glDisable(GL_DEPTH_TEST); can be used to isolate the skybox in the future (maybe if i implement ui...)
	glDepthFunc(GL_LEQUAL);
	GLuint WorldMatrixLocation = glGetUniformLocation(Renderer::GetShaderProgramID(), "WorldTransform");
	glm::mat4 WorldMatrix = offsetMatrix * GetWorldMatrix();
	glUniformMatrix4fv(WorldMatrixLocation, 1, GL_FALSE, &WorldMatrix[0][0]);
    // skybox cube
    glBindVertexArray(mVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapId);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);


	//glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

	
}
bool SkyBox::ParseLine(const std::vector<ci_string> &token) {
	if (token.empty())
	{
		return true;
	}
	else
	{
		return Model::ParseLine(token);
	}
}
void SkyBox::Update(float dt) {

	Model::Update(dt);
 }

// not actually used for skybox but is in ainherited thing from model D:
void SkyBox::getCornerPoint(std::vector<glm::vec3>& input) {
	for (int i = 0; i < 8; i++)
		input.push_back(CornerPoint[i]);
}