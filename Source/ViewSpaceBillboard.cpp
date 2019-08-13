//
// COMP 371 Assignment Framework
//
// Created by Nicolas Bergeron on 15/7/15.
//
// Copyright (c) 2014-2019 Concordia University. All rights reserved.
//

#include "Billboard.h"
#include "Renderer.h"
#include "World.h"
#include "WorldBlock.h"
#include "Camera.h"
#include "StaticCamera.h"
#include "ViewSpaceBillboard.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/common.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glm/gtx/string_cast.hpp>
#include <iostream>

#include "LightSource.h"
#include <vector>

using namespace std;
using namespace std;
using namespace glm;




ViewSpaceBillboard::ViewSpaceBillboard(unsigned int maxNumBillboards)
: mMaxNumBillboards(maxNumBillboards)
{
	// light source info
	LightSource lightSource = LightSource();

    // Pre-allocate Vertex Buffer - 6 vertices by billboard (2 triangles)
    mVertexBuffer.resize(maxNumBillboards * 6);
    
    // Initialize vertices to white, with UVs taking all the texture
    for (int i=0; i<(int)mVertexBuffer.size(); ++i)
    {
        // White vertex
        mVertexBuffer[i].color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
        
        // Normal on Z axis
        mVertexBuffer[i].normal = vec3(0.0f, 0.0f, 1.0f);
        
        // Texture coordinates
        switch(i%6)
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
    // Note the vertex buffer will change over time, we use GL_DYNAMIC_DRAW
    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, mVertexBuffer.size() * sizeof(BillboardVertex), &mVertexBuffer[0], GL_DYNAMIC_DRAW);
    Renderer::CheckForErrors();
    
    // 1st attribute buffer : vertex Positions
    glVertexAttribPointer(0,                // attribute. No particular reason for 0, but must match the layout in the shader.
                          3,                // size
                          GL_FLOAT,        // type
                          GL_FALSE,        // normalized?
                          sizeof(BillboardVertex), // stride
                          (void*)0        // array buffer offset
                          );
    glEnableVertexAttribArray(0);

    // 2nd attribute buffer : vertex normal
    glVertexAttribPointer(1,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(BillboardVertex),
                          (void*)sizeof(vec3)    // Normal is Offseted by vec3 (see class Vertex)
                          );
    glEnableVertexAttribArray(1);

    
    // 3rd attribute buffer : vertex color
    glVertexAttribPointer(2,
                          4,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(BillboardVertex),
                          (void*) (2* sizeof(vec3)) // Color is Offseted by 2 vec3 (see class Vertex)
                          );
    glEnableVertexAttribArray(2);

    // 3rd attribute buffer : texture coordinates
    glVertexAttribPointer(3,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(BillboardVertex),
                          (void*) (2* sizeof(vec3) + sizeof(vec4)) // texture coordinates are Offseted by 2 vec3 (see class Vertex) and a vec4
                          );
    glEnableVertexAttribArray(3);


}

ViewSpaceBillboard::~ViewSpaceBillboard()
{
	// Free the GPU from the Vertex Buffer
	glDeleteBuffers(1, &mVBO);
	glDeleteVertexArrays(1, &mVAO);

	mVertexBuffer.resize(0);
	mBillboardList.resize(0);
}

void ViewSpaceBillboard::AddBillboard(Billboard* b)
{
    mBillboardList.push_back(b);
    
    assert(mBillboardList.size() <= mVertexBuffer.size() / 6.0f);
}

void ViewSpaceBillboard::RemoveBillboard(Billboard* b)
{
    list<Billboard*>::iterator it = find(mBillboardList.begin(), mBillboardList.end(), b);
    assert((*it != nullptr));
    mBillboardList.remove(*it);
}

void ViewSpaceBillboard::Update(float dt)
{

    Renderer::CheckForErrors();
    
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, 6*sizeof(BillboardVertex)*mBillboardList.size(), (void*)&mVertexBuffer[0]);
}

//void BillboardList::Draw(glm::mat4 offsetMatrix)
void ViewSpaceBillboard::Draw()
{
    Renderer::CheckForErrors();

    
    //// Set current shader to be the Textured Shader
    //ShaderType oldShader = (ShaderType)Renderer::GetCurrentShader();
    //
    //Renderer::SetShader(SHADER_TEXTURED);
    //glUseProgram(Renderer::GetShaderProgramID());

    //Renderer::CheckForErrors();

    
    GLuint textureLocation = glGetUniformLocation(Renderer::GetShaderProgramID(), "mySamplerTexture");
    glActiveTexture(GL_TEXTURE0);

    Renderer::CheckForErrors();

    
    glBindTexture(GL_TEXTURE_2D, mTextureID[0]);
    glUniform1i(textureLocation, 1);				// Set our Texture sampler to user Texture Unit 0

    
    Renderer::CheckForErrors();

	GLuint ViewMatrixID = glGetUniformLocation(Renderer::GetShaderProgramID(), "ViewTransform");
	// viewTransform
	//mat4 View = World::getWorldInstance()->getWorldBlock()->GetCurrentCamera()->GetViewMatrix();
	mat4 View = World::getWorldInstance()->GetCurrentCamera()->GetViewMatrix();
	glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &View[0][0]);


	vec4 lPosition;// = lightSource.getPositions();
	vec3 lColor;// = lightSource.getColors();
	
	//WorldBlock* mWorld = World::getWorldInstance()->getWorldBlock();
	World* mWorld = World::getWorldInstance();




    // This looks for the MVP Uniform variable in the Vertex Program
    //GLuint VPMatrixLocation = glGetUniformLocation(Renderer::GetShaderProgramID(), "ViewProjectionTransform");
	GLuint PMatrixLocation = glGetUniformLocation(Renderer::GetShaderProgramID(), "ProjectionTransform");
    
    // Send the view projection constants to the shader
    //const Camera* currentCamera = World::getWorldInstance()->getWorldBlock()->GetCurrentCamera();
	const Camera* currentCamera = World::getWorldInstance()->GetCurrentCamera();
    mat4 VP = currentCamera->GetViewProjectionMatrix();
	//mat4 P = currentCamera->GetProjectionMatrix();
    glUniformMatrix4fv(PMatrixLocation, 1, GL_FALSE, &VP[0][0]);

    // Draw the Vertex Buffer
    // Note this draws a unit Cube
    // The Model View Projection transforms are computed in the Vertex Shader
    glBindVertexArray(mVAO);
    
    GLuint WorldMatrixLocation = glGetUniformLocation(Renderer::GetShaderProgramID(), "WorldTransform");

    // Billboard position are all relative to the origin
    //mat4 worldMatrix(1.0f);
	mat4 worldMatrix = mat4(1.0f);// offsetMatrix;

    glUniformMatrix4fv(WorldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
    
    // Draw the triangles !
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei) mBillboardList.size()*6); // 6 vertices by billboard
    
    Renderer::CheckForErrors();
    
    //Renderer::SetShader(oldShader);
}
