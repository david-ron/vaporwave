#pragma once

#include <glm/glm.hpp>
#include <vector>

using namespace glm;
using namespace std;

class Buildings
{
public:
	Buildings(int BuildingAmo);
	~Buildings();

	static void setBuildingDefaultSize(vec3 size);
	mat4 getBuildingOffsetMatrixAt(int index);

private:
	static vec3 BuildingDefaultSize;
	static float ShapeEscapeChance;

	int BuildingAmo;
	int cBuildingAmo;
	vector<vec3> mPosition;
	vector<vec3> mScaling;
	float mRotation;

	void ClusterShape();
	void WallShape();
	bool isAcceptable(vec3, vec3);
};

