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
#include "Billboard.h"
#include "LightSource.h"
#include "Buildings.h"

#include <vector>

//class myLights;
class Camera;
class Model;
class Animation;
class AnimationKey;
class ParticleSystem;
class ParticleDescriptor;
class LightSource;
class SkyBox;
using namespace std;
using namespace glm;

class WorldBlock
{
public:
	WorldBlock(vec2);
	~WorldBlock();
	
    //static WorldBlock* GetInstance();

	void Update(float dt);
	//void Draw();
	void DrawCurrentShader();
	void DrawCurrentLightSources();
	void DrawPathLinesShader();
	void DrawTextureShader();


	//void LoadScene(const char * scene_path);
    Animation* FindAnimation(ci_string animName);
    AnimationKey* FindAnimationKey(ci_string keyName);
    ParticleDescriptor* FindParticleDescriptor(ci_string name);
	//std::vector<LightSource*> getLightSource();
	const LightSource getLightSourceAt(int);
	const int getLightSize() { return lightSource.size(); }
	vec2 getWorldBlockCoor() { return vec2(WB_Coordinate[0], WB_Coordinate[1]); }
	Buildings* getBuildings() { return mBuildings; }
	void getBuildingsWorldMatrix(vector<mat4>&);
	mat4 getWBOffsetMatrix() { return WB_OffsetMatrix; }
	bool IsLightSphere() { return isLightSphere; }


    //const Camera* GetCurrentCamera() const;
    void AddBillboard(Billboard* b);
    void RemoveBillboard(Billboard* b);
    void AddParticleSystem(ParticleSystem* particleSystem);
    void RemoveParticleSystem(ParticleSystem* particleSystem);
    void AddParticleDescriptor(ParticleDescriptor* particleDescriptor);

	void setModel(std::vector<Model*> mModel);
	void setAnimation(std::vector<Animation*> mAnimation);
	void setAnimationKey(std::vector<AnimationKey*> mAnimationKey);
	//void setCamera(std::vector<Camera*> mCamera);
	void setParticleSystemList(std::vector<ParticleSystem*> mParticleSystemList);
	void setParticleDescriptorList(std::vector<ParticleDescriptor*> mParticleDescriptorList);
	void setCurrentCamera(unsigned int mCurrentCamera);
	void setLightSource(std::vector<LightSource*> lightSource);
	void setBillboardList(BillboardList* mpBillboardList);
	void setOnThis(bool o) { onThis = o; }

	void setIsLightSphere(bool LS) { isLightSphere = LS; }
	void setSphereIndex(int i) { SphereIndex = i; }

    
private:
    
	//Terrain * terrain;
	std::vector<Model*> mModel;
	int SphereIndex;
    std::vector<Animation*> mAnimation;
    std::vector<AnimationKey*> mAnimationKey;
	//std::vector<Camera*> mCamera;
    std::vector<ParticleSystem*> mParticleSystemList;
    std::vector<ParticleDescriptor*> mParticleDescriptorList;
	unsigned int mCurrentCamera;
	std::vector<LightSource*> lightSource;

    BillboardList* mpBillboardList;

	int WB_Coordinate[2];
	mat4 WB_OffsetMatrix;

	static int const buildingSizeRange[2];
	int BuildingAmo;
	//vector<mat4> buildingOffsetMatrix;
	Buildings* mBuildings;

	//to tell whether object is on a worldBlock
	bool onThis = false;

	bool isLightSphere;
};
