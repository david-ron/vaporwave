//
// COMP 371 Assignment Framework
//
// Created by Nicolas Bergeron on 8/7/14.
// Updated by Gary Chang on 14/1/15
//
// Copyright (c) 2014-2019 Concordia University. All rights reserved.
//

#pragma once

#include "ParsingHelper.h"

#include <vector>
#include "objLoader.hpp"
#include <glm/glm.hpp>

class Animation;

class Model
{
public:
	Model();
	virtual ~Model();

	virtual void Update(float dt) = 0;
	virtual void Draw(glm::mat4 offsetMatrix) = 0;
	virtual bool isCollided() { return false; }

	void Load(ci_istringstream& iss);

	virtual glm::mat4 GetWorldMatrix() const;

	void SetPosition(glm::vec3 position);
	void SetScaling(glm::vec3 scaling);
	void SetRotation(glm::vec3 axis, float angleDegrees);

	glm::vec3 GetPosition() const		{ return mPosition; }
	glm::vec3 GetScaling() const		{ return mScaling; }
	glm::vec3 GetRotationAxis() const	{ return mRotationAxis; }
	float     GetRotationAngle() const	{ return mRotationAngleInDegrees; }
    ci_string GetName()                 { return mName; }
	glm::vec4 getProperties() { return properties; }
    virtual void getCornerPoint(std::vector<glm::vec3>& input){ for (int i = 0; i < 8; i++)
        input.push_back(CornerPoint[i]);};
protected:
	virtual bool ParseLine(const std::vector<ci_string> &token) = 0;

	ci_string mName; // The model name is mainly for debugging
	glm::vec3 mPosition;
	glm::vec3 mScaling;
	glm::vec3 mRotationAxis;
	float     mRotationAngleInDegrees;
    virtual void makeSimpleColor (std::vector<glm::vec3> &colors){colors.push_back(glm::vec3(0.7f,0.0f,0.9f));};
    // Makes the model follow a list of Animation Keys so it's world transform changes over time
    Animation* mAnimation;
	glm::vec4 properties;
    std::vector<glm::vec3> CornerPoint;
    friend class Animation;
};
