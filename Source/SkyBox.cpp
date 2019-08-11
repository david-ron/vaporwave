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


SkyBox::SkyBox(){
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
    
    unsigned int mVAO, mVBO;
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
    faces.push_back(skyboxprefix + "purplevalley_lt.tga");
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
    
    int width, height;
    FREE_IMAGE_FORMAT format;
    FIBITMAP* image;
    FIBITMAP* image32bits;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        format = FreeImage_GetFileType(faces[i].c_str(), 0);
        image = FreeImage_Load(format, faces[i].c_str());
        image32bits = FreeImage_ConvertTo32Bits(image);
        width = FreeImage_GetWidth(image32bits);
        height = FreeImage_GetHeight(image32bits);
       // unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
 
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE,  (void*)FreeImage_GetBits(image32bits));
            //stbi_image_free(data);
        // Free images
        FreeImage_Unload(image);
        FreeImage_Unload(image32bits);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    
    cubeMapId = textureID;
    

}
SkyBox::~SkyBox()
{
    // Free the GPU from the Vertex Buffer
    glDeleteBuffers(1, &mVBO);
    glDeleteVertexArrays(1, &mVAO);
}


void SkyBox::Draw(){
    
    // skybox cube
    glBindVertexArray(mVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapId);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS);
}
