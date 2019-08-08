//
// COMP 371 Assignment Framework
//
// Created by Nicolas Bergeron on 8/7/14.
//
// Copyright (c) 2014-2019 Concordia University. All rights reserved.
//

#pragma once

#include "Camera.h"
using namespace std;
using namespace glm;

class ThirdPersonCamera : public Camera
{
public:
	ThirdPersonCamera(glm::vec3 position, glm::vec3 lookAt);
	virtual ~ThirdPersonCamera();

	virtual void Update(float dt);

	virtual glm::mat4 GetViewMatrix() const;
	vec3 getLookAt();
	vec3 getSideVector();
	

private:
	glm::vec3 mPosition;	
	float mHorizontalAngle; // horizontal angle
	float mVerticalAngle;   // vertical angle

	float mSpeed;			// WorldBlock units per second
	const float speedUpRate = 4.0f;
	float mAngularSpeed;    // Degrees per pixel

	glm::vec3 mLookAt;
	glm::vec3 mSideVector;


};
