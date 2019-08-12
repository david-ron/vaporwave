#pragma once

#include <vector>
#include <glm/glm.hpp>

#include <stdio.h>

#include <glm/glm.hpp>
#include <iostream>
#include "ParsingHelper.h"

using namespace std;
using namespace glm;

class LightSource
{
public:
	LightSource();
	~LightSource();
	

	glm::vec4 getPosition() const { return position; }
	glm::vec3 getColor() const { return color; }
	//glm::vec3 getAttenuation() const { return lightAttenuation; }

	void Load(ci_istringstream& iss);
	bool ParseLine(std::vector<ci_string> token);

	void setPostion(glm::vec4 position);
	void setColor(glm::vec3 color);
	//void setAttentuation(glm::vec3 attenuation);

private:
	ci_string name;
	vec4 position;
	vec3 color;
	vec3 lightAttenuation;

};

