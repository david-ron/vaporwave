//
// COMP 371 Assignment Framework
//
// Created by Nicolas Bergeron on 8/7/14.
// Updated by Gary Chang on 14/1/15
//
// Copyright (c) 2014-2019 Concordia University. All rights reserved.
//

#include "Animation.h"
#include "Renderer.h"
#include "World.h"
#include "WorldBlock.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/common.hpp>
#include<glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
//#include <GL/glew.h>  
#include <GLFW/glfw3.h>

using namespace glm;
using namespace std;

AnimationKey::AnimationKey() : Model()
{
}

AnimationKey::~AnimationKey()
{
}

void AnimationKey::Update(float dt)
{
	Model::Update(dt);
}

void AnimationKey::Draw()
{
	Model::Draw();
}

bool AnimationKey::ParseLine(const std::vector<ci_string> &token)
{
	if (token.empty())
	{
		return true;
	}
	else
	{
		return Model::ParseLine(token);
	}
}

Animation::Animation() 
	: mName(""), mCurrentTime(0.0f), mDuration(0.0f), mVBO(0), mVAO(0)
{
}

Animation::~Animation()
{
}

void Animation::CreateVertexBuffer()
{
    // This is just to display lines between the animation keys
    for (int i=0; i<(int)mKey.size(); ++i)
    {
        Vertex v;
        v.position = mKey[i].mPosition;
        mVertexBuffer.push_back(v);
    }
    
	// Create a vertex array
	glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);

	// Upload Vertex Buffer to the GPU, keep a reference to it (mVertexBufferID)
	glGenBuffers(1, &mVBO);
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*mVertexBuffer.size(), &(mVertexBuffer[0]), GL_STATIC_DRAW);
    
    // Create a vertex array
    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);
    
    // Upload Vertex Buffer to the GPU, keep a reference to it (mVertexBufferID)
    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*mVertexBuffer.size(), &(mVertexBuffer[0]), GL_STATIC_DRAW);
    
    // 1st attribute buffer : vertex Positions
    glVertexAttribPointer(0,
                          3,                // size
                          GL_FLOAT,        // type
                          GL_FALSE,        // normalized?
                          sizeof(Vertex), // stride
                          (void*)0        // array buffer offset
                          );
    glEnableVertexAttribArray(0);

}

void Animation::Update(float dt)
{
	mCurrentTime += dt;

    while(mCurrentTime > mDuration)
    {
        mCurrentTime -= mDuration;
    }
}

void Animation::Draw()
{
	// @TODO 3 - Draw lines between animation key positions
	//
	// Steps:
	// - Draw the Vertex Buffer
	// - The Shader used to draw the Animation is PathLines, only requires to send the positions of the key positions
	// - You need to bind the vertex buffer created in CreateVertexBuffer() as the input of the vertex shader
	// - Also set the WorldTransform uniform variable in the vertex shader
	// - You Should use GL_LINE_LOOP as a primitive to draw lines (not triangles)
	// - You can look at CubeModel::Draw() for a good example!
    //
    // Notes:
    // The shader is bound in WorldBlock.cpp and the ViewProjection Matrix uniform is set there...
	// The Model View Projection transforms are computed in the Vertex Shader

	mat4 worldMatrix(1.0f);

	glBindVertexArray(mVAO);
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);

	GLuint WorldMatrixLocation = glGetUniformLocation(Renderer::GetShaderProgramID(), "WorldTransform");
	glUniformMatrix4fv(WorldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);

	// Draw the loop !
	glDrawArrays(GL_LINE_LOOP, 0, mVertexBuffer.size());

}


void Animation::Load(ci_istringstream& iss)
{
	ci_string line;

	// Parse model line by line
	while(std::getline(iss, line))
	{
		// Splitting line into tokens
		ci_istringstream strstr(line);
		istream_iterator<ci_string, char, ci_char_traits> it(strstr);
		istream_iterator<ci_string, char, ci_char_traits> end;
		vector<ci_string> token(it, end);

		if (ParseLine(token) == false)
		{
			fprintf(stderr, "Error loading scene file... token:  %s!", token[0].c_str());
			getchar();
			exit(-1);
		}
	}
}


bool Animation::ParseLine(const std::vector<ci_string> &token)
{
    if (token.empty())
    {
        return true;
    }
	else if (token[0] == "name")
	{
		assert(token.size() > 2);
		assert(token[1] == "=");

		mName = token[2];
		return true;
	}
	else if (token[0] == "key")
	{
		assert(token.size() > 5);
        assert(token[1] == "=");
        assert(token[3] == "time");
        assert(token[4] == "=");

		ci_string name = token[2];

		//AnimationKey *key = World::getWorldInstance()->getWorldBlock()->FindAnimationKey(name);
		AnimationKey *key = World::getWorldInstance()->FindAnimationKey(name);
        
		assert(key != nullptr);
		AddKey(key, (float) atof(token[5].c_str()));
		return true;
	}
	return false;
}

void Animation::AddKey(AnimationKey* key, float time)
{
    assert(time >= mDuration);
	mKey.push_back(*key);
    mKeyTime.push_back(time);
    mDuration = time;
}

ci_string Animation::GetName() const
{
	return mName;
}

glm::mat4 Animation::GetAnimationWorldMatrix() const
{
    // @TODO 4 - Find the 2 keys to interpolate the transformation (before and after current time)
    //           Interpolate the position, scaling and rotation separately
    //           Finally concatenate the interpolated transforms into a single
    //           world transform and return it.
    
	int keySize = mKey.size();//Amount of key points in this animation
	int keyTimeSize = mKeyTime.size();//Amount of key time in this animation
	float time = glfwGetTime();//Current overall time
	float periodTime = fmod(time,mDuration); // time with in one period

	// index of the key in front of the current position (end key)
	int fIndex;
	for (fIndex = 1; fIndex < keySize; fIndex++)
		if (periodTime <= mKeyTime[fIndex])
			break;
	if (fIndex == keyTimeSize) fIndex = 0;
	// index of the key at the back of the current position () 
	int bIndex = (fIndex == 0) ? keySize - 1 : fIndex - 1;


	//To find the interpolant
	float subPeriodTime = mKeyTime[bIndex + 1] - mKeyTime[bIndex];
	float interpolant = (periodTime - mKeyTime[bIndex]) / subPeriodTime;

	//new parameters ( linear interpolation)
	glm::vec3 newPosition = glm::mix(mKey[bIndex].mPosition, mKey[fIndex].mPosition,interpolant);
	glm::vec3 newScaling = glm::mix(mKey[bIndex].mScaling , mKey[fIndex].mScaling,interpolant);
	glm::vec3 newRotationAxis = glm::mix(mKey[bIndex].mRotationAxis , mKey[fIndex].mRotationAxis,interpolant);
	float     newRotationAngleInDegrees = glm::mix(mKey[bIndex].mRotationAngleInDegrees, mKey[fIndex].mRotationAngleInDegrees,interpolant);

	//rotation, scaling & translation
	glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(newRotationAngleInDegrees), newRotationAxis);
	glm::mat4 scalingMatrix = glm::scale(glm::mat4(1.0f), newScaling);
	glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), newPosition);

	//manual way to find translation matrix (testing)
	//glm::mat4 translationMatrix = mat4(	1.0f, 0.0f, 0.0f, 0.0f, 
	//									0.0f, 1.0f, 0.0f, 0.0f, 
	//									0.0f, 0.0f, 1.0f, 0.0f, 
	//									newPosition[0], newPosition[1], newPosition[2], 1.0f);
	//glm::mat4 scalingMatrix = mat4(	newScaling[0], 0.0f, 0.0f, 0.0f,
	//								0.0f, newScaling[1], 0.0f, 0.0f,
	//								0.0f, 0.0f, newScaling[2], 0.0f,
	//								0.0f, 0.0f, 0.0f, 1.0f);

	



	//Animation world matrix
	mat4 worldMatrix = translationMatrix * scalingMatrix * rotationMatrix;


    return worldMatrix;
}

// testing
//quat qb = angleAxis(mKey[bIndex].mRotationAngleInDegrees, mKey[bIndex].mRotationAxis);
//quat qf = angleAxis(mKey[fIndex].mRotationAngleInDegrees, mKey[fIndex].mRotationAxis);
//quat q = slerp(qb, qf, interpolant);
//glm::mat4 rotationMatrix = glm::toMat4(q);