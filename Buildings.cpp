#include "Buildings.h"
#include "EventManager.h"
#include <random>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

vec3 Buildings::BuildingDefaultSize = vec3(2,10,2);
float Buildings::ShapeEscapeChance = 0.1;

mat4 Buildings::getBuildingOffsetMatrixAt(int index) {
	mat4 t = glm::translate(mat4(1.0f), mPosition[index]);
	mat4 r = glm::rotate(mat4(1.0f), glm::radians(mRotation), vec3(0,1,0));
	mat4 s = glm::scale(mat4(1.0f), mScaling[index]);
	return t * r * s;

}

void Buildings::setBuildingDefaultSize(vec3 size) {
	BuildingDefaultSize = size;
}

Buildings::Buildings(int BuildingAmo)
{
	this->BuildingAmo = BuildingAmo;
	cBuildingAmo = 0;

	mRotation = EventManager::GetRandomFloat(0, 360);

	// select the shape to start with
	int shape = EventManager::GetRandomInt(0, 1);
	// generate the buildings;
	while (cBuildingAmo < BuildingAmo) {
		if (shape) {
			WallShape();
			shape = 0;
		}
		else {
			ClusterShape();
			shape = 1;
		}
	}

}

void Buildings::ClusterShape() {
	std::default_random_engine generator;
	generator.seed(EventManager::GetRandomInt());
	std::normal_distribution<float> ScalingGenerator(1.2, 0.3);

	// check if there are enough building
	if (cBuildingAmo >= BuildingAmo) return;
	// center of the cluster
	mPosition.push_back(vec3(EventManager::GetRandomFloat(-45, 45), 0, EventManager::GetRandomFloat(-45, 45)));
	float temp[3];
	for (int i = 0; i < 3; i++) {
		temp[i] = ScalingGenerator(generator);
		if (temp[i] <= 0.3)
			i--;
	}
	mScaling.push_back(vec3(temp[0], temp[1], temp[2]) * 0.5f);
	mPosition[cBuildingAmo].y = BuildingDefaultSize.y * mScaling[cBuildingAmo].y / 2;
	cBuildingAmo++;

	float dIndex = 0.3;
	float yScalingIndex = 1;
	int centerBuildingIndex = cBuildingAmo - 1;
	while (cBuildingAmo < BuildingAmo) {
		// there is a chance that 
		if (EventManager::GetRandomFloat(0, 1) < ShapeEscapeChance) return;

		std::normal_distribution<float> xGenerator(mPosition[centerBuildingIndex].x, BuildingDefaultSize.x*dIndex);
		std::normal_distribution<float> yGenerator(mPosition[centerBuildingIndex].y, BuildingDefaultSize.y*dIndex);

		vec3 tempPosition, tempScaling;

		do {
			tempPosition = vec3(xGenerator(generator), 0, yGenerator(generator));
			float temp[3];
			for (int i = 0; i < 3; i++) { 
				temp[i] = ScalingGenerator(generator); 
				if (temp[i] <= 0.3)
					i--;
			}
			tempScaling = vec3(temp[0], temp[1] * yScalingIndex, temp[2]);
			tempPosition.y = BuildingDefaultSize.y * tempScaling.y / 2;
			
		} while (!isAcceptable(tempPosition, tempScaling));


		mPosition.push_back(tempPosition);
		mScaling.push_back(tempScaling * 0.5f);
		cBuildingAmo++;

		yScalingIndex -= 0.07;
		dIndex += 0.2;

	}

}

void Buildings::WallShape() {
	std::default_random_engine generator;
	generator.seed(EventManager::GetRandomInt());
	std::normal_distribution<float> ScalingGenerator(1.2, 0.3);

	// check if there are enough building
	if (cBuildingAmo >= BuildingAmo) return;
	// center of the cluster
	mPosition.push_back(vec3(EventManager::GetRandomFloat(-45, 45), 0, EventManager::GetRandomFloat(-45, 45)));
	float temp[3];
	for (int i = 0; i < 3; i++) {
		temp[i] = ScalingGenerator(generator);
		if (temp[i] <= 0.3)
			i--;
	}
	mScaling.push_back(vec3(temp[0], temp[1], temp[2]) * 0.5f);
	mPosition[cBuildingAmo].y = BuildingDefaultSize.y * mScaling[cBuildingAmo].y / 2;
	cBuildingAmo++;

	float dIndex = 0.3;
	float yScalingIndex = 1;
	int centerBuildingIndex = cBuildingAmo - 1;
	while (cBuildingAmo < BuildingAmo) {
		// there is a chance that 
		if (EventManager::GetRandomFloat(0, 1) < ShapeEscapeChance) return;

		std::normal_distribution<float> xGenerator(mPosition[centerBuildingIndex].x, BuildingDefaultSize.x*dIndex);
		std::normal_distribution<float> yGenerator(mPosition[centerBuildingIndex].y, BuildingDefaultSize.y*dIndex);

		vec3 tempPosition, tempScaling;

		do {
			tempPosition = vec3(xGenerator(generator), 0, yGenerator(generator));
			float temp[3];
			for (int i = 0; i < 3; i++) {
				temp[i] = ScalingGenerator(generator);
				if (temp[i] <= 0.3)
					i--;
			}
			tempScaling = vec3(temp[0], temp[1] * yScalingIndex, temp[2]);
			tempPosition.y = BuildingDefaultSize.y * tempScaling.y / 2;

		} while (!isAcceptable(tempPosition, tempScaling));


		mPosition.push_back(tempPosition);
		mScaling.push_back(tempScaling * 0.5f);
		cBuildingAmo++;

		yScalingIndex -= 0.07;
		dIndex += 0.2;

	}
}

bool Buildings::isAcceptable(vec3 aPosition, vec3 aScaling) {
	
	vec3 bPosition, bScaling;
	                                                                  
	for (int i = 0; i < cBuildingAmo; i++) {
		bPosition = mPosition[i];
		bScaling = mScaling[i];

		float xDiff = abs(aPosition.x - bPosition.x);
		float yDiff = abs(aPosition.y - bPosition.y);

		if (xDiff < 1.2*(aScaling.x + bScaling.x) && yDiff < 1.2*(aScaling.y + bScaling.y))
			return false;
	}

	return true;
}

Buildings::~Buildings()
{
}
