//
// COMP 371 Assignment Framework
//
// Created by Nicolas Bergeron on 8/7/14.
// Updated by Gary Chang on 14/1/15
//
// Copyright (c) 2014-2019 Concordia University. All rights reserved.
//

#include "Terrain/Terrain.h"
#include "WorldBlock.h"
#include "Renderer.h"
#include "ParsingHelper.h"
//#include "Lights.h"

#include "StaticCamera.h"
#include "FirstPersonCamera.h"

#include "World.h"
#include "CubeModel.h"
#include "SphereModel.h"
#include "Animation.h"
#include "Billboard.h"
#include <GLFW/glfw3.h>
#include "EventManager.h"
#include "TextureLoader.h"
#include "Billboard.h"

#include "ParticleDescriptor.h"
#include "ParticleEmitter.h"
#include "ParticleSystem.h"

#include "LightSource.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <random>
#include <glm/gtc/type_ptr.hpp>


using namespace std;
using namespace glm;

//WorldBlock* WorldBlock::instance;
const int WorldBlock::buildingSizeRange[2] = {15,25};

WorldBlock::WorldBlock(vec2 coor)
{
	WB_Coordinate[0] = coor.x;
	WB_Coordinate[1] = coor.y;

	WB_OffsetMatrix = glm::translate(mat4(1.0f), vec3(coor[0]*World::WorldBlockSize, 0.0, coor[1]*World::WorldBlockSize));

	std::default_random_engine generator;

	float MidRange = buildingSizeRange[0] + (buildingSizeRange[1] - buildingSizeRange[0]) / 2.0;
	float c = (buildingSizeRange[1] - buildingSizeRange[0]) / 3.0;
	std::normal_distribution<double> distribution(MidRange, c);
	

	BuildingAmo = 0;
	while (BuildingAmo< buildingSizeRange[0] || BuildingAmo > buildingSizeRange[1])
	{
		BuildingAmo = round(distribution(generator));
	}
	
	assert(BuildingAmo > buildingSizeRange[0] && BuildingAmo < buildingSizeRange[1]);

	
	mBuildings = new Buildings(BuildingAmo);

    
#if defined(PLATFORM_OSX)
//    int billboardTextureID = TextureLoader::LoadTexture("Textures/BillboardTest.bmp");
    int billboardTextureID = TextureLoader::LoadTexture("Textures/Particle.png");
#else
//    int billboardTextureID = TextureLoader::LoadTexture("../Assets/Textures/BillboardTest.bmp");
    int billboardTextureID = TextureLoader::LoadTexture("../Assets/Textures/Particle.png");
#endif
    assert(billboardTextureID != 0);

    mpBillboardList = new BillboardList(2048, billboardTextureID);


	isLightSphere = false;
}

WorldBlock::~WorldBlock()
{
	// Models
	for (vector<Model*>::iterator it = mModel.begin(); it < mModel.end(); ++it)
	{
		delete *it;
	}

	mModel.clear();

	for (vector<Animation*>::iterator it = mAnimation.begin(); it < mAnimation.end(); ++it)
	{
		delete *it;
	}

	mAnimation.clear();

	for (vector<AnimationKey*>::iterator it = mAnimationKey.begin(); it < mAnimationKey.end(); ++it)
	{
		delete *it;
	}

	mAnimationKey.clear();

	//// Camera
	//for (vector<Camera*>::iterator it = mCamera.begin(); it < mCamera.end(); ++it)
	//{
	//	delete *it;
	//}
	//mCamera.clear();
    
    for (vector<ParticleSystem*>::iterator it = mParticleSystemList.begin(); it < mParticleSystemList.end(); ++it)
    {
        delete *it;
    }
    mParticleSystemList.clear();
    
    for (vector<ParticleDescriptor*>::iterator it = mParticleDescriptorList.begin(); it < mParticleDescriptorList.end(); ++it)
    {
        delete *it;
    }
    mParticleDescriptorList.clear();

    
	delete mpBillboardList;
}

//WorldBlock* WorldBlock::GetInstance()
//{
//    return instance;
//}

void WorldBlock::Update(float dt)
{


    // Update animation and keys
    for (vector<Animation*>::iterator it = mAnimation.begin(); it < mAnimation.end(); ++it)
    {
        (*it)->Update(dt);
    }
    
    for (vector<AnimationKey*>::iterator it = mAnimationKey.begin(); it < mAnimationKey.end(); ++it)
    {
        (*it)->Update(dt);
    }


	// Update current Camera
	World::getWorldInstance()->GetCurrentCamera()->Update(dt);

	// Update models
	for (vector<Model*>::iterator it = mModel.begin(); it < mModel.end(); ++it)
	{
		(*it)->Update(dt);
	}
    
    // Update billboards
    
    for (vector<ParticleSystem*>::iterator it = mParticleSystemList.begin(); it != mParticleSystemList.end(); ++it)
    {
        (*it)->Update(dt);
    }
    
    mpBillboardList->Update(dt);

}



void WorldBlock::DrawCurrentShader() {
	Renderer::CheckForErrors();
	
	// This looks for the MVP Uniform variable in the Vertex Program
	GLuint VPMatrixLocation = glGetUniformLocation(Renderer::GetShaderProgramID(), "ViewProjectionTransform");
	// Get a handle for our Transformation Matrices uniform
	//GLuint WorldMatrixID = glGetUniformLocation(Renderer::GetShaderProgramID(), "WorldTransform");
	GLuint ViewMatrixID = glGetUniformLocation(Renderer::GetShaderProgramID(), "ViewTransform");
	GLuint ProjMatrixID = glGetUniformLocation(Renderer::GetShaderProgramID(), "ProjectonTransform");



	//// Get a handle for Material Attributes uniform
	//GLuint MaterialID = glGetUniformLocation(Renderer::GetShaderProgramID(), "materialCoefficients");

	// Send the view projection constants to the shader
	// VP
	//mat4 VP = mCamera[mCurrentCamera]->GetViewProjectionMatrix();
	mat4 VP = World::getWorldInstance()->GetCurrentCamera()->GetViewProjectionMatrix();
	glUniformMatrix4fv(VPMatrixLocation, 1, GL_FALSE, &VP[0][0]);

	// viewTransform
	//mat4 View = mCamera[mCurrentCamera]->GetViewMatrix();
	mat4 View = World::getWorldInstance()->GetCurrentCamera()->GetViewMatrix();
	glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &View[0][0]);
	// projectionMatrix
	//mat4 Projection = mCamera[mCurrentCamera]->GetProjectionMatrix();
	mat4 Projection = World::getWorldInstance()->GetCurrentCamera()->GetProjectionMatrix();
	glUniformMatrix4fv(ProjMatrixID, 1, GL_FALSE, &Projection[0][0]);


	GLuint LightAttenuationID = glGetUniformLocation(Renderer::GetShaderProgramID(), "lightAttenuation");
	glUniform3f(LightAttenuationID, 0.0f, 0.0f, 1.0f);


	int lSize = lightSource.size();
	GLuint LightSizeID = glGetUniformLocation(Renderer::GetShaderProgramID(), "lightSize");
	glUniform1i(LightSizeID, lSize);

	vec4 LightPositions[8];
	vec3 LightColor[8];

	for (int i = 0; i < lSize; i++) {
		LightPositions[i] = lightSource[i]->getPosition();
		LightColor[i] = lightSource[i]->getColor();
	}

	GLuint LightPositionID = glGetUniformLocation(Renderer::GetShaderProgramID(), "lPosition");
	GLuint LightColorID = glGetUniformLocation(Renderer::GetShaderProgramID(), "lColor");

	glUniform4fv(LightPositionID,lSize,value_ptr(LightPositions[0]));
	glUniform3fv(LightColorID,lSize,value_ptr(LightColor[0]));


	vec4 mProperties;
	// Draw models
	for (vector<Model*>::iterator it = mModel.begin(); it < mModel.end(); ++it)
	{
		if (isLightSphere && (*it)->GetName() == "\"Sphere\"")
			continue;
		
		mProperties = (*it)->getProperties();
		GLuint materialCoefficientsID = glGetUniformLocation(Renderer::GetShaderProgramID(), "materialCoefficients");
		glUniform4f(materialCoefficientsID, mProperties.x, mProperties.y, mProperties.z, mProperties.w);

		if ((*it)->GetName() != "\"Building\"" && (*it)->GetName() != "\"fly\"") {
			
			(*it)->Draw(WB_OffsetMatrix);
		}
		else if ((*it)->GetName() == "\"fly\"") {
			cout << glfwGetTime() << "  ";
			vec3 vColor;
			if (fmod(glfwGetTime(),60) > 30) {
				vColor = vec3(0.0, 0.18, 0.39);
			}
			else {
				float dimmer = fmod(glfwGetTime(), 10) / 10;
				float lightBrightness;
				if (1 - dimmer > 0.6)
					lightBrightness = (float)1 - dimmer;
				else
					lightBrightness = (0.4) + dimmer;
				vColor = vec3(lightBrightness);
			}
			
			GLuint mVertexColorID = glGetUniformLocation(Renderer::GetShaderProgramID(), "mVertexColor");
			glUniform3f(mVertexColorID, vColor.x, vColor.y, vColor.z);
			GLuint mVertexColorEnableID = glGetUniformLocation(Renderer::GetShaderProgramID(), "mVertexColorEnable");
			glUniform1i(mVertexColorEnableID, 1);
			GLuint mLightDirectionIgnoreID = glGetUniformLocation(Renderer::GetShaderProgramID(), "mLightDirectionIgnore");
			glUniform1i(mLightDirectionIgnoreID, 1);

			(*it)->Draw(WB_OffsetMatrix);
			glUniform1i(mVertexColorEnableID, 0);
			glUniform1i(mLightDirectionIgnoreID, 0);
		}
		else
		
			for (int i = 0; i < BuildingAmo; i++) {

				mat4 offSet = mBuildings->getBuildingOffsetMatrixAt(i);

				vec3 vColor;
				float temp = offSet[0][0];
				temp -= floor(temp);
				temp *= 100;

				if (temp < 33)
					vColor = vec3(0.345, 0.08, 0.58);
				else if (temp < 66)
					vColor = vec3(0.81, 0.188, 0.72);
				else
					vColor = vec3(1, 0.45, 0.8);
				


				GLuint mVertexColorID = glGetUniformLocation(Renderer::GetShaderProgramID(), "mVertexColor");
				glUniform3f(mVertexColorID, vColor.x, vColor.y, vColor.z);
				GLuint mVertexColorEnableID = glGetUniformLocation(Renderer::GetShaderProgramID(), "mVertexColorEnable");
				glUniform1i(mVertexColorEnableID, 1);

				(*it)->Draw(WB_OffsetMatrix * mBuildings->getBuildingOffsetMatrixAt(i));

				//GLuint mVertexColorEnableID = glGetUniformLocation(Renderer::GetShaderProgramID(), "mVertexColorEnable");
				glUniform1i(mVertexColorEnableID, 0);
			}
		
	}

	Renderer::CheckForErrors();
}

void WorldBlock::DrawCurrentLightSources() {

	if (!isLightSphere) return;
	Renderer::CheckForErrors();

	// This looks for the MVP Uniform variable in the Vertex Program
	GLuint VPMatrixLocation = glGetUniformLocation(Renderer::GetShaderProgramID(), "ViewProjectionTransform");
	// Get a handle for our Transformation Matrices uniform
	//GLuint WorldMatrixID = glGetUniformLocation(Renderer::GetShaderProgramID(), "WorldTransform");
	GLuint ViewMatrixID = glGetUniformLocation(Renderer::GetShaderProgramID(), "ViewTransform");
	GLuint ProjMatrixID = glGetUniformLocation(Renderer::GetShaderProgramID(), "ProjectonTransform");



	//// Get a handle for Material Attributes uniform
	//GLuint MaterialID = glGetUniformLocation(Renderer::GetShaderProgramID(), "materialCoefficients");

	// Send the view projection constants to the shader
	// VP
	//mat4 VP = mCamera[mCurrentCamera]->GetViewProjectionMatrix();
	mat4 VP = World::getWorldInstance()->GetCurrentCamera()->GetViewProjectionMatrix();
	glUniformMatrix4fv(VPMatrixLocation, 1, GL_FALSE, &VP[0][0]);

	// viewTransform
	//mat4 View = mCamera[mCurrentCamera]->GetViewMatrix();
	mat4 View = World::getWorldInstance()->GetCurrentCamera()->GetViewMatrix();
	glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &View[0][0]);
	// projectionMatrix
	//mat4 Projection = mCamera[mCurrentCamera]->GetProjectionMatrix();
	mat4 Projection = World::getWorldInstance()->GetCurrentCamera()->GetProjectionMatrix();
	glUniformMatrix4fv(ProjMatrixID, 1, GL_FALSE, &Projection[0][0]);


	GLuint LightAttenuationID = glGetUniformLocation(Renderer::GetShaderProgramID(), "lightAttenuation");
	glUniform3f(LightAttenuationID, 0.0f, 0.0f, 1.0f);

	GLuint LightPositionID = glGetUniformLocation(Renderer::GetShaderProgramID(), "lPosition");
	GLuint LightColorID = glGetUniformLocation(Renderer::GetShaderProgramID(), "lColor");

	
	vec3 lPosition = WB_OffsetMatrix * vec4(mModel[SphereIndex]->GetPosition(), 1.0f);
	glUniform3f(LightPositionID, lPosition.x, lPosition.y, lPosition.z);
	glUniform3f(LightColorID, 1.0f, 1.0f, 1.0f);
	
	mModel[SphereIndex]->Draw(WB_OffsetMatrix);

	//vec4 mProperties;
	//// Draw models
	//for (vector<Model*>::iterator it = mModel.begin(); it < mModel.end(); ++it)
	//{
	//	if (isLightSphere && (*it)->GetName() != "\"Sphere\"")
	//		continue;

	//	mProperties = (*it)->getProperties();
	//	GLuint materialCoefficientsID = glGetUniformLocation(Renderer::GetShaderProgramID(), "materialCoefficients");
	//	glUniform4f(materialCoefficientsID, mProperties.x, mProperties.y, mProperties.z, mProperties.w);
	//	if ((*it)->GetName() != "\"Building\"")
	//		(*it)->Draw(WB_OffsetMatrix);
	//	else
	//		for (int i = 0; i < BuildingAmo; i++) {
	//			(*it)->Draw(WB_OffsetMatrix * mBuildings->getBuildingOffsetMatrixAt(i));
	//		}
	//}

	Renderer::CheckForErrors();
}


void WorldBlock::DrawPathLinesShader() {
	Renderer::CheckForErrors();
	glUniformMatrix4fv(glGetUniformLocation(Renderer::GetShaderProgramID(), "WBOffsetMatrix"), 1, GL_FALSE, &WB_OffsetMatrix[0][0]);

	// Send the view projection constants to the shader
	GLuint VPMatrixLocation = glGetUniformLocation(Renderer::GetShaderProgramID(), "ViewProjectionTransform");
	mat4 VP = World::getWorldInstance()->GetCurrentCamera()->GetViewProjectionMatrix();
	glUniformMatrix4fv(VPMatrixLocation, 1, GL_FALSE, &VP[0][0]);

	for (vector<Animation*>::iterator it = mAnimation.begin(); it < mAnimation.end(); ++it)
	{
		mat4 VP = World::getWorldInstance()->GetCurrentCamera()->GetViewProjectionMatrix();
		//mCamera[mCurrentCamera]->GetViewProjectionMatrix();
		glUniformMatrix4fv(VPMatrixLocation, 1, GL_FALSE, &VP[0][0]);

		(*it)->Draw();
	}

	for (vector<AnimationKey*>::iterator it = mAnimationKey.begin(); it < mAnimationKey.end(); ++it)
	{
		//mat4 VP = mCamera[mCurrentCamera]->GetViewProjectionMatrix();
		mat4 VP = World::getWorldInstance()->GetCurrentCamera()->GetViewProjectionMatrix();
		glUniformMatrix4fv(VPMatrixLocation, 1, GL_FALSE, &VP[0][0]);

		(*it)->Draw(WB_OffsetMatrix);
	}
	Renderer::CheckForErrors();
}


void WorldBlock::DrawTextureShader() {
	Renderer::CheckForErrors();
	mpBillboardList->Draw(WB_OffsetMatrix);
	Renderer::CheckForErrors();
}


LightSource const WorldBlock::getLightSourceAt(int index)  {
	return *(lightSource[index]);
}



Animation* WorldBlock::FindAnimation(ci_string animName)
{
    for(std::vector<Animation*>::iterator it = mAnimation.begin(); it < mAnimation.end(); ++it)
    {
        if((*it)->GetName() == animName)
        {
            return *it;
        }
    }
    return nullptr;
}

AnimationKey* WorldBlock::FindAnimationKey(ci_string keyName)
{
    for(std::vector<AnimationKey*>::iterator it = mAnimationKey.begin(); it < mAnimationKey.end(); ++it)
    {
        if((*it)->GetName() == keyName)
        {
            return *it;
        }
    }
    return nullptr;
}

//const Camera* WorldBlock::GetCurrentCamera() const
//{
//     return mCamera[mCurrentCamera];
//}

void WorldBlock::AddBillboard(Billboard* b)
{
    mpBillboardList->AddBillboard(b);
}

void WorldBlock::RemoveBillboard(Billboard* b)
{
    mpBillboardList->RemoveBillboard(b);
}

void WorldBlock::AddParticleSystem(ParticleSystem* particleSystem)
{
    mParticleSystemList.push_back(particleSystem);
}

void WorldBlock::RemoveParticleSystem(ParticleSystem* particleSystem)
{
    vector<ParticleSystem*>::iterator it = std::find(mParticleSystemList.begin(), mParticleSystemList.end(), particleSystem);
    mParticleSystemList.erase(it);
}

void WorldBlock::AddParticleDescriptor(ParticleDescriptor* particleDescriptor)
{
    mParticleDescriptorList.push_back(particleDescriptor);
}

ParticleDescriptor* WorldBlock::FindParticleDescriptor(ci_string name)
{
    for(std::vector<ParticleDescriptor*>::iterator it = mParticleDescriptorList.begin(); it < mParticleDescriptorList.end(); ++it)
    {
        if((*it)->GetName() == name)
        {
            return *it;
        }
    }
    return nullptr;
}

void WorldBlock::setModel(std::vector<Model*> mModel) {
	for (int i = 0; i < mModel.size(); i++) {
		this->mModel.push_back(mModel[i]);
	}
}
void WorldBlock::setAnimation(std::vector<Animation*> mAnimation) {
	for (int i = 0; i < mAnimation.size(); i++) {
		this->mAnimation.push_back(mAnimation[i]);
	}
}
void WorldBlock::setAnimationKey(std::vector<AnimationKey*> mAnimationKey) {
	for (int i = 0; i < mAnimationKey.size(); i++) {
		this->mAnimationKey.push_back(mAnimationKey[i]);
	}
}

//void WorldBlock::setCamera(std::vector<Camera*> mCamera) {
//	for (int i = 0; i < mCamera.size(); i++) {
//		this->mCamera.push_back(mCamera[i]);
//	}
//}

void WorldBlock::setParticleSystemList(std::vector<ParticleSystem*> mParticleSystemList) {
	for (int i = 0; i < mParticleSystemList.size(); i++) {
		this->mParticleSystemList.push_back(mParticleSystemList[i]);
	}
}
void WorldBlock::setParticleDescriptorList(std::vector<ParticleDescriptor*> mParticleDescriptorList) {
	for (int i = 0; i < mParticleDescriptorList.size(); i++) {
		this->mParticleDescriptorList.push_back(mParticleDescriptorList[i]);
	}
}
void WorldBlock::setCurrentCamera(unsigned int mCurrentCamera) {
	this->mCurrentCamera = mCurrentCamera;
}
void WorldBlock::setLightSource(std::vector<LightSource*> lightSource) {
	for (int i = 0; i < lightSource.size(); i++) {
		this->lightSource.push_back(lightSource[i]);
	}
}

void WorldBlock::setBillboardList(BillboardList* mpBillboardList) {
	this->mpBillboardList = mpBillboardList;
}


void WorldBlock::getBuildingsWorldMatrix(vector<mat4>& input) {
	int oldSize = input.size();

	for (int i = 0; i < BuildingAmo; i++) {
		input.push_back(WB_OffsetMatrix * mBuildings->getBuildingOffsetMatrixAt(i));
	}

	int newSize = input.size();
	assert(newSize - oldSize == BuildingAmo);
}

