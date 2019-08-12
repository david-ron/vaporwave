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
ThirdPersonCamera::ThirdPersonCamera(glm::vec3 position, glm::vec3 lookAt) :  Camera(), mPosition(position), mLookAt(lookAt), mHorizontalAngle(90.0f), mVerticalAngle(0.0f), mSpeed(5.0f), mAngularSpeed(2.5f), mCamLookat(vec3(0.0f))
{

}

ThirdPersonCamera::~ThirdPersonCamera()
{
}

void ThirdPersonCamera::Update(float dt)
{
	
	/*if (!leftKeyPressed) {
		
	}*/

	if (!leftKeyPressed) {
		mCamHorizontalAngle = 0;
		mCamVerticalAngle = 0;
	}


	mLookAt = World::getWorldInstance()->getMClookAt();
	mPosition = World::getWorldInstance()->getMCposition();
	mSideVector = World::getWorldInstance()->getMCsideVector();

	mHorizontalAngle = World::getWorldInstance()->getHorizontalAngle();
	mVerticalAngle = World::getWorldInstance()->getVerticalAngle();

	float mH = mHorizontalAngle + mCamHorizontalAngle;
	float mV = mVerticalAngle + mCamVerticalAngle;


	mV= std::max(-85.0f, std::min(85.0f, mV));

	if (mH> 360)
	{
		mH-= 360;
	}
	else if (mH< -360)
	{
		mH+= 360;
	}

	float theta = radians(mH);
	float phi = radians(mV);

	mCamLookat = vec3(cosf(phi)*cosf(theta), sinf(phi), -cosf(phi)*sinf(theta));

	//newPosition = mPosition - mLookAt * 20.0f;
	newPosition = mPosition - mCamLookat * 20.0f;
}

void ThirdPersonCamera::addExtraCamAngle(float H, float V) {
	mCamHorizontalAngle -= H;
	mCamVerticalAngle -= V;

	//cout << mCamVerticalAngle << endl;
	//if (mCamHorizontalAngle > 360)
	//{
	//	mCamHorizontalAngle -= 360;
	//}
	//else if (mCamHorizontalAngle < -360)
	//{
	//	mCamHorizontalAngle+= 360;
	//}
}



glm::vec3 ThirdPersonCamera::getLookAt() { return mLookAt; }
glm::vec3 ThirdPersonCamera::getSideVector() { return mSideVector; }

glm::mat4 ThirdPersonCamera::GetViewMatrix() const
{

	return glm::lookAt(	newPosition , mPosition , vec3(0.0f, 1.0f, 0.0f));
	
}
