#pragma once

#include <glm/glm.hpp>
#include <cstring>
#include <vector>
#include <string>


glm::vec3 calculateLookAt(float mHorizontalAngle, float mVerticalAngle)
{
	float theta = glm::radians(mHorizontalAngle);
	float phi = glm::radians(mVerticalAngle);
	return glm::vec3(cosf(phi)*cosf(theta), sinf(phi), -cosf(phi)*sinf(theta));
}
// not even used
glm::vec3 calculateSideVector(glm::vec3 LookAt, vec3 groundNormal = vec3(0.0f,1.0f,0.0f))
{
	groundNormal = vec3(0.0f, 1.0f, 0.0f);
	return glm::normalize(glm::cross(LookAt, groundNormal));
}
