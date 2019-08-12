//
// COMP 371 Assignment Framework
//
// Created by Nicolas Bergeron on 8/7/14.
//
// Copyright (c) 2014-2019 Concordia University. All rights reserved.
//

#include "ThirdPersonCamera.h"
#include "EventManager.h"
#include "World.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include <GLFW/glfw3.h>
#include <algorithm>
#include <iostream>



using namespace glm;
using namespace std;
ThirdPersonCamera::ThirdPersonCamera(glm::vec3 position, glm::vec3 lookAt) :  Camera(), mPosition(position), mLookAt(lookAt), mHorizontalAngle(90.0f), mVerticalAngle(0.0f), mSpeed(5.0f), mAngularSpeed(2.5f)
{

}

ThirdPersonCamera::~ThirdPersonCamera()
{
}

void ThirdPersonCamera::Update(float dt)
{
	
	/*if (!leftKeyPressed) {
		
	}*/


	mLookAt = World::getWorldInstance()->getMClookAt();
	mPosition = World::getWorldInstance()->getMCposition();
	mSideVector = World::getWorldInstance()->getMCsideVector();


	newPosition = mPosition -vec3(radius*cosf(phi)*cosf(theta), 
									radius*sinf(phi), 
									-radius * cosf(phi)*sinf(theta));

	newPosition = mPosition - mLookAt * 20.0f;
}



glm::vec3 ThirdPersonCamera::getLookAt() { return mLookAt; }
glm::vec3 ThirdPersonCamera::getSideVector() { return mSideVector; }

glm::mat4 ThirdPersonCamera::GetViewMatrix() const
{

	return glm::lookAt(	newPosition , mPosition , vec3(0.0f, 1.0f, 0.0f));
	
}
