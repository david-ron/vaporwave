//
// COMP 371 Assignment Framework
//
// Created by Nicolas Bergeron on 8/7/14.
//
// Copyright (c) 2014-2019 Concordia University. All rights reserved.
//

#pragma once
#include <math.h>
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
	void setLookAt(vec3 LookAt) { mLookAt = LookAt; };
	void setSideVector(vec3 SideVector) { mSideVector = SideVector; };
	void setPostion(vec3 position) { mPosition = position; };
	float getmHorizontalAngle() { return mHorizontalAngle; };
	float getmVerticalAngle() { return mVerticalAngle; };
	void setmHorizontalAngle(float HorizontalAngle) { mHorizontalAngle = HorizontalAngle; };
	void setmVerticalAngle(float VerticalAngle) { mVerticalAngle = VerticalAngle; };
	void setleftKeyPressed(bool keyPressed) { leftKeyPressed = keyPressed; };
private:
	glm::vec3 mPosition;	
	float mHorizontalAngle; // horizontal angle
	float mVerticalAngle;   // vertical angle

	float mSpeed;			// WorldBlock units per second
	const float speedUpRate = 4.0f;
	float mAngularSpeed;    // Degrees per pixel

	glm::vec3 mLookAt;
	glm::vec3 mSideVector;

	float radius=10.0f; //radius of the sphere
	float theta = radians(mHorizontalAngle);
	float phi = radians(mVerticalAngle);
	vec3 newPosition;

	bool leftKeyPressed;


};
