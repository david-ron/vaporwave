#include "Buildings.h"
#include "EventManager.h"
#include <random>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

vec3 Buildings::BuildingDefaultSize = vec3(2,10,2);
float Buildings::ShapeEscapeChance = 0.1;

mat4 Buildings::getBuildingOffsetMatrixAt(int index) {
	mat4 t = glm::translate(mat4(1.0f), mPosition[index]);
	
	mat4 s = glm::scale(mat4(1.0f), mScaling[index]);
	return t * s;

}

void Buildings::setBuildingDefaultSize(vec3 size) {
	BuildingDefaultSize = size;
}

Buildings::Buildings(int BuildingAmo)
{
	this->BuildingAmo = BuildingAmo;
	cBuildingAmo = 0;

	// select the shape to start with
	int shape = EventManager::GetRandomInt(0, 1);
	// generate the buildings;
	while (cBuildingAmo < BuildingAmo) {
		//if (shape) {
		//	WallShape();
		//	shape = 0;
		//}
		//else {
			ClusterShape();
			shape = 1;
		//}
	}

}

void Buildings::ClusterShape() {
	std::default_random_engine generator;
	generator.seed(EventManager::GetRandomInt());
	std::normal_distribution<float> ScalingGenerator(1.0, 0.5);

	// check if there are enough building
	if (cBuildingAmo >= BuildingAmo) return;
	// center of the cluster
	mPosition.push_back(vec3(EventManager::GetRandomFloat(-45, 45), 0, EventManager::GetRandomFloat(-45, 45)));
	float temp[3];
	for (int i = 0; i < 3; i++) {
		temp[i] = ScalingGenerator(generator);
		//if (temp[i] <= 0)
		//	i--;
	}
	mScaling.push_back(vec3(temp[0], temp[1], temp[2]));
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

		mPosition.push_back(vec3(xGenerator(generator), 0, yGenerator(generator)));

		float temp[3];
		for (int i = 0; i < 3; i++) {temp[i] = ScalingGenerator(generator);}
		mScaling.push_back(vec3(temp[0], temp[1] * yScalingIndex, temp[2]));
		mPosition[cBuildingAmo].y = BuildingDefaultSize.y * mScaling[cBuildingAmo].y / 2;
		cBuildingAmo++;

		yScalingIndex -= 0.1;
		dIndex += 0.1;

	}

}

void Buildings::WallShape() {
	
}

bool Buildings::isAcceptable(vec3 position, vec3 scaling) {
	bool result = true;

	for (int i = 0; i < cBuildingAmo; i++) {
		
	}

	return result;
}

Buildings::~Buildings()
{
}
