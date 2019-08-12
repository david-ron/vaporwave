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


// This is used for sorting the list of Billboards
bool CompareBillboardAlongZ::operator()(const Billboard* a, const Billboard* b)
{
    //mat4 viewMatrix = World::getWorldInstance()->getWorldBlock()->GetCurrentCamera()->GetViewMatrix();
	mat4 viewMatrix = World::getWorldInstance()->GetCurrentCamera()->GetViewMatrix();
    return ((viewMatrix*vec4(a->position, 1.0f)).z < (viewMatrix*vec4(b->position, 1.0f)).z);
}


BillboardList::BillboardList(unsigned int maxNumBillboards, int textureID)
: mTextureID(textureID), mMaxNumBillboards(maxNumBillboards)
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

BillboardList::~BillboardList()
{
	// Free the GPU from the Vertex Buffer
	glDeleteBuffers(1, &mVBO);
	glDeleteVertexArrays(1, &mVAO);

	mVertexBuffer.resize(0);
	mBillboardList.resize(0);
}

void BillboardList::AddBillboard(Billboard* b)
{
    mBillboardList.push_back(b);
    
    assert(mBillboardList.size() <= mVertexBuffer.size() / 6.0f);
}

void BillboardList::RemoveBillboard(Billboard* b)
{
    list<Billboard*>::iterator it = find(mBillboardList.begin(), mBillboardList.end(), b);
    assert((*it != nullptr));
    mBillboardList.remove(*it);
}

void BillboardList::Update(float dt)
{
    // Sort billboards according to their depth, the functor CompareBillboardAlongZ does the magic!
    CompareBillboardAlongZ comp;
    mBillboardList.sort(comp);
    
    // Set Data in Vertex Buffer
    unsigned long firstVertexIndex = 0;

    // Maybe the view matrix will be useful to align the billboards
    //const Camera* cam = World::getWorldInstance()->getWorldBlock()->GetCurrentCamera();
	const Camera* cam = World::getWorldInstance()->GetCurrentCamera();
    mat4 viewMatrix = cam->GetViewMatrix();
	
	
	
	
    
    for (list<Billboard*>::iterator it = mBillboardList.begin(); it != mBillboardList.end(); ++it)
    {
        const Billboard* b = *it;

		vec3 right = vec3(viewMatrix[0][0], viewMatrix[1][0], viewMatrix[2][0]);
		vec3 up = vec3(viewMatrix[0][1], viewMatrix[1][1], viewMatrix[2][1]);
		vec3 LookAt = vec3(viewMatrix[0][2], viewMatrix[1][2], viewMatrix[2][2]);

		vec3 mRight = normalize( rotate(right, radians(b->angle), LookAt))*0.5f*b->size.x;
		vec3 mUp = normalize( rotate(up, radians(b->angle), LookAt))*0.5f*b->size.y;



        // Colors
        mVertexBuffer[firstVertexIndex].color = mVertexBuffer[firstVertexIndex + 1].color = mVertexBuffer[firstVertexIndex +2].color = mVertexBuffer[firstVertexIndex + 3].color = mVertexBuffer[firstVertexIndex + 4].color = mVertexBuffer[firstVertexIndex + 5].color = b->color;

        
        // @TODO 5 - Align billboards with Camera plane
        //
        // For each billboard, update each vertex position and normals
        // Currently, positions are aligned with the X-Y plane, billboards must face the camera
        //
        // You must update the positions and normals for the 6 vertices below
        
        
        // Normals
        //mVertexBuffer[firstVertexIndex].normal = mVertexBuffer[firstVertexIndex + 1].normal = mVertexBuffer[firstVertexIndex +2].normal = mVertexBuffer[firstVertexIndex + 3].normal = mVertexBuffer[firstVertexIndex + 4].normal = mVertexBuffer[firstVertexIndex + 5].normal = vec3(0.0f, 0.0f, 1.0f); // wrong...
		
		
		mVertexBuffer[firstVertexIndex].normal = mVertexBuffer[firstVertexIndex + 1].normal = mVertexBuffer[firstVertexIndex + 2].normal = mVertexBuffer[firstVertexIndex + 3].normal = mVertexBuffer[firstVertexIndex + 4].normal = mVertexBuffer[firstVertexIndex + 5].normal = -LookAt;

        // First triangle
        // Top left
		//mVertexBuffer[firstVertexIndex].position.x = b->position.x - 0.5f*b->size.x;
		//mVertexBuffer[firstVertexIndex].position.y = b->position.y + 0.5f*b->size.y;
		//mVertexBuffer[firstVertexIndex].position.z = b->position.z;
		mVertexBuffer[firstVertexIndex].position = b->position - mRight + mUp;

        // Bottom Left
        //mVertexBuffer[firstVertexIndex + 1].position.x = b->position.x - 0.5f*b->size.x;
        //mVertexBuffer[firstVertexIndex + 1].position.y = b->position.y - 0.5f*b->size.y;
        //mVertexBuffer[firstVertexIndex + 1].position.z = b->position.z;
		mVertexBuffer[firstVertexIndex + 1].position = b->position - mRight - mUp;

        
        // Top Right
        //mVertexBuffer[firstVertexIndex + 2].position.x = b->position.x + 0.5f*b->size.x;
        //mVertexBuffer[firstVertexIndex + 2].position.y = b->position.y + 0.5f*b->size.y;
        //mVertexBuffer[firstVertexIndex + 2].position.z = b->position.z;
		mVertexBuffer[firstVertexIndex + 2].position = b->position + mRight + mUp;
        
        // Second Triangle
        // Top Right
        //mVertexBuffer[firstVertexIndex + 3].position.x = b->position.x + 0.5f*b->size.x;
        //mVertexBuffer[firstVertexIndex + 3].position.y = b->position.y + 0.5f*b->size.y;
        //mVertexBuffer[firstVertexIndex + 3].position.z = b->position.z;
		mVertexBuffer[firstVertexIndex + 3].position = b->position + mRight + mUp;
        
        // Bottom Left
        //mVertexBuffer[firstVertexIndex + 4].position.x = b->position.x - 0.5f*b->size.x;
        //mVertexBuffer[firstVertexIndex + 4].position.y = b->position.y - 0.5f*b->size.y;
        //mVertexBuffer[firstVertexIndex + 4].position.z = b->position.z;
		mVertexBuffer[firstVertexIndex + 4].position = b->position - mRight - mUp;
        
        // Bottom Right
        //mVertexBuffer[firstVertexIndex + 5].position.x = b->position.x + 0.5f*b->size.x;
        //mVertexBuffer[firstVertexIndex + 5].position.y = b->position.y - 0.5f*b->size.y;
        //mVertexBuffer[firstVertexIndex + 5].position.z = b->position.z;
		mVertexBuffer[firstVertexIndex + 5].position = b->position + mRight - mUp;
        
        // do not touch this...
        firstVertexIndex += 6;
    }
    
    Renderer::CheckForErrors();
    
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, 6*sizeof(BillboardVertex)*mBillboardList.size(), (void*)&mVertexBuffer[0]);
}

//void BillboardList::Draw(glm::mat4 offsetMatrix)
void BillboardList::Draw(glm::mat4 offsetMatrix)
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

    
    glBindTexture(GL_TEXTURE_2D, mTextureID);
    glUniform1i(textureLocation, 0);				// Set our Texture sampler to user Texture Unit 0

    
    Renderer::CheckForErrors();

	GLuint ViewMatrixID = glGetUniformLocation(Renderer::GetShaderProgramID(), "ViewTransform");
	// viewTransform
	//mat4 View = World::getWorldInstance()->getWorldBlock()->GetCurrentCamera()->GetViewMatrix();
	mat4 View = World::getWorldInstance()->GetCurrentCamera()->GetViewMatrix();
	glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &View[0][0]);


	GLuint LightAttenuationID = glGetUniformLocation(Renderer::GetShaderProgramID(), "lightAttenuation");
	glUniform3f(LightAttenuationID, 0.0f, 0.0f, 1.0f);

	//Lighting 
	int enabled;
	vec4 lPosition;// = lightSource.getPositions();
	vec3 lColor;// = lightSource.getColors();
	
	//WorldBlock* mWorld = World::getWorldInstance()->getWorldBlock();
	World* mWorld = World::getWorldInstance();
	int lightSize = mWorld->getLightSize();
	LightSource lightSource;/// = mWorld->getLightSourceAt(0);

	int lSize = World::getWorldInstance()->getLightSize();
	GLuint LightSizeID = glGetUniformLocation(Renderer::GetShaderProgramID(), "lightSize");
	glUniform1i(LightSizeID, lSize);


	vec4 LightPositions[8];
	vec3 LightColor[8];

	for (int i = 0; i < lSize; i++) {
		LightPositions[i] = World::getWorldInstance()->getLightSourceAt(i).getPosition();
		LightColor[i] = World::getWorldInstance()->getLightSourceAt(i).getColor();
	}

	GLuint LightPositionID = glGetUniformLocation(Renderer::GetShaderProgramID(), "lPosition");
	GLuint LightColorID = glGetUniformLocation(Renderer::GetShaderProgramID(), "lColor");

	glUniform4fv(LightPositionID, lSize, value_ptr(LightPositions[0]));
	glUniform3fv(LightColorID, lSize, value_ptr(LightColor[0]));

	// Light 1
	enabled = 0;
	GLuint L1EnableID = glGetUniformLocation(Renderer::GetShaderProgramID(), "L1Enable");
	GLuint L1PositionID = glGetUniformLocation(Renderer::GetShaderProgramID(), "L1Position");
	GLuint L1ColorID = glGetUniformLocation(Renderer::GetShaderProgramID(), "L1Color");

	if (lightSize > 0) {
		enabled = 1;
		lightSource = mWorld->getLightSourceAt(0);
		lPosition = lightSource.getPosition();
		lColor = lightSource.getColor();
	}

	glUniform1i(L1EnableID, enabled);
	glUniform4f(L1PositionID, lPosition.x, lPosition.y, lPosition.z, lPosition.w);
	glUniform3f(L1ColorID, lColor.x, lColor.y, lColor.z);
	
	// Light 2
	enabled = 0;

	GLuint L2EnableID = glGetUniformLocation(Renderer::GetShaderProgramID(), "L2Enable");
	GLuint L2PositionID = glGetUniformLocation(Renderer::GetShaderProgramID(), "L2Position");
	GLuint L2ColorID = glGetUniformLocation(Renderer::GetShaderProgramID(), "L2Color");

	if (lightSize > 1) {
		enabled = 1;
		lightSource = mWorld->getLightSourceAt(1);
		lPosition = lightSource.getPosition();
		lColor = lightSource.getColor();
	}
	glUniform1i(L2EnableID, enabled);
	glUniform4f(L2PositionID, lPosition.x, lPosition.y, lPosition.z, lPosition.w);
	glUniform3f(L2ColorID, lColor.x, lColor.y, lColor.z);

	// Light 3
	enabled = 0;

	GLuint L3EnableID = glGetUniformLocation(Renderer::GetShaderProgramID(), "L3Enable");
	GLuint L3PositionID = glGetUniformLocation(Renderer::GetShaderProgramID(), "L3Position");
	GLuint L3ColorID = glGetUniformLocation(Renderer::GetShaderProgramID(), "L3Color");

	if (lightSize > 2) {
		enabled = 1;
		lightSource = mWorld->getLightSourceAt(2);
		lPosition = lightSource.getPosition();
		lColor = lightSource.getColor();
	}

	glUniform1i(L3EnableID, enabled);
	glUniform4f(L3PositionID, lPosition.x, lPosition.y, lPosition.z, lPosition.w);
	glUniform3f(L3ColorID, lColor.x, lColor.y, lColor.z);

	// Light 4
	enabled = 0;

	GLuint L4EnableID = glGetUniformLocation(Renderer::GetShaderProgramID(), "L4Enable");
	GLuint L4PositionID = glGetUniformLocation(Renderer::GetShaderProgramID(), "L4Position");
	GLuint L4ColorID = glGetUniformLocation(Renderer::GetShaderProgramID(), "L4Color");

	if (lightSize > 3) {
		enabled = 1;
		lightSource = mWorld->getLightSourceAt(3);
		lPosition = lightSource.getPosition();
		lColor = lightSource.getColor();
	}
	glUniform1i(L4EnableID, enabled);
	glUniform4f(L4PositionID, lPosition.x, lPosition.y, lPosition.z, lPosition.w);
	glUniform3f(L4ColorID, lColor.x, lColor.y, lColor.z);

	// Light 5
	enabled = 0;

	GLuint L5EnableID = glGetUniformLocation(Renderer::GetShaderProgramID(), "L5Enable");
	GLuint L5PositionID = glGetUniformLocation(Renderer::GetShaderProgramID(), "L5Position");
	GLuint L5ColorID = glGetUniformLocation(Renderer::GetShaderProgramID(), "L5Color");

	if (lightSize > 4) {
		enabled = 1;
		lightSource = mWorld->getLightSourceAt(4);
		lPosition = lightSource.getPosition();
		lColor = lightSource.getColor();
	}
	glUniform1i(L5EnableID, enabled);
	glUniform4f(L5PositionID, lPosition.x, lPosition.y, lPosition.z, lPosition.w);
	glUniform3f(L5ColorID, lColor.x, lColor.y, lColor.z);

	// Light 6
	enabled = 0;

	GLuint L6EnableID = glGetUniformLocation(Renderer::GetShaderProgramID(), "L6Enable");
	GLuint L6PositionID = glGetUniformLocation(Renderer::GetShaderProgramID(), "L6Position");
	GLuint L6ColorID = glGetUniformLocation(Renderer::GetShaderProgramID(), "L6Color");

	if (lightSize > 5) {
		enabled = 1;
		lightSource = mWorld->getLightSourceAt(5);
		lPosition = lightSource.getPosition();
		lColor = lightSource.getColor();
	}
	glUniform1i(L6EnableID, enabled);
	glUniform4f(L6PositionID, lPosition.x, lPosition.y, lPosition.z, lPosition.w);
	glUniform3f(L6ColorID, lColor.x, lColor.y, lColor.z);


	// Light 7
	enabled = 0;

	GLuint L7EnableID = glGetUniformLocation(Renderer::GetShaderProgramID(), "L7Enable");
	GLuint L7PositionID = glGetUniformLocation(Renderer::GetShaderProgramID(), "L7Position");
	GLuint L7ColorID = glGetUniformLocation(Renderer::GetShaderProgramID(), "L7Color");

	if (lightSize > 6) {
		enabled = 1;
		lightSource = mWorld->getLightSourceAt(6);
		lPosition = lightSource.getPosition();
		lColor = lightSource.getColor();
	}
	glUniform1i(L7EnableID, enabled);
	glUniform4f(L7PositionID, lPosition.x, lPosition.y, lPosition.z, lPosition.w);
	glUniform3f(L7ColorID, lColor.x, lColor.y, lColor.z);

	// Light 8
	enabled = 0;

	GLuint L8EnableID = glGetUniformLocation(Renderer::GetShaderProgramID(), "L8Enable");
	GLuint L8PositionID = glGetUniformLocation(Renderer::GetShaderProgramID(), "L8Position");
	GLuint L8ColorID = glGetUniformLocation(Renderer::GetShaderProgramID(), "L8Color");

	if (lightSize > 7) {
		enabled = 1;
		lightSource = mWorld->getLightSourceAt(7);
		lPosition = lightSource.getPosition();
		lColor = lightSource.getColor();
	}
	glUniform1i(L8EnableID, enabled);
	glUniform4f(L8PositionID, lPosition.x, lPosition.y, lPosition.z, lPosition.w);
	glUniform3f(L8ColorID, lColor.x, lColor.y, lColor.z);



    // This looks for the MVP Uniform variable in the Vertex Program
    GLuint VPMatrixLocation = glGetUniformLocation(Renderer::GetShaderProgramID(), "ViewProjectionTransform");
    
    // Send the view projection constants to the shader
    //const Camera* currentCamera = World::getWorldInstance()->getWorldBlock()->GetCurrentCamera();
	const Camera* currentCamera = World::getWorldInstance()->GetCurrentCamera();
    mat4 VP = currentCamera->GetViewProjectionMatrix();
    glUniformMatrix4fv(VPMatrixLocation, 1, GL_FALSE, &VP[0][0]);

    // Draw the Vertex Buffer
    // Note this draws a unit Cube
    // The Model View Projection transforms are computed in the Vertex Shader
    glBindVertexArray(mVAO);
    
    GLuint WorldMatrixLocation = glGetUniformLocation(Renderer::GetShaderProgramID(), "WorldTransform");

    // Billboard position are all relative to the origin
    //mat4 worldMatrix(1.0f);
	mat4 worldMatrix = offsetMatrix;
    glUniformMatrix4fv(WorldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
    
    // Draw the triangles !
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei) mBillboardList.size()*6); // 6 vertices by billboard
    
    Renderer::CheckForErrors();
    
    //Renderer::SetShader(oldShader);
}
