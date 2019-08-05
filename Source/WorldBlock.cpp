//
// COMP 371 Assignment Framework
//
// Created by Nicolas Bergeron on 8/7/14.
// Updated by Gary Chang on 14/1/15
//
// Copyright (c) 2014-2019 Concordia University. All rights reserved.
//

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

#include "ParticleDescriptor.h"
#include "ParticleEmitter.h"
#include "ParticleSystem.h"

#include "LightSource.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


using namespace std;
using namespace glm;

//WorldBlock* WorldBlock::instance;


WorldBlock::WorldBlock(vec2 coor)
{
	WB_Coordinate[0] = coor.x;
	WB_Coordinate[1] = coor.y;

	WB_OffsetMatrix = glm::translate(mat4(1.0f), vec3(coor[0]*World::WorldBlockSize, 0.0, coor[1]*World::WorldBlockSize));
	//mat4 offsetMatrix = translate(mat4(1.0), vec3(100.0, 0.0, 0.0));


	//lightSource = LightSource();

	// Setup Camera
	mCamera.push_back(new FirstPersonCamera(vec3(3.0f, 5.0f, 20.0f)));
	mCamera.push_back(new StaticCamera(vec3(3.0f, 30.0f, 5.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f)));
	mCamera.push_back(new StaticCamera(vec3(0.5f,  0.5f, 5.0f), vec3(0.0f, 0.5f, 0.0f), vec3(0.0f, 1.0f, 0.0f)));
	mCurrentCamera = 0;

    
#if defined(PLATFORM_OSX)
//    int billboardTextureID = TextureLoader::LoadTexture("Textures/BillboardTest.bmp");
    int billboardTextureID = TextureLoader::LoadTexture("Textures/Particle.png");
#else
//    int billboardTextureID = TextureLoader::LoadTexture("../Assets/Textures/BillboardTest.bmp");
    int billboardTextureID = TextureLoader::LoadTexture("../Assets/Textures/Particle.png");
#endif
    assert(billboardTextureID != 0);

    mpBillboardList = new BillboardList(2048, billboardTextureID);

    
    // TODO - You can un-comment out these 2 temporary billboards and particle system
    // That can help you debug billboards, you can set the billboard texture to billboardTest.png
    /*    Billboard *b = new Billboard();
     b->size  = glm::vec2(2.0, 2.0);
     b->position = glm::vec3(0.0, 3.0, 0.0);
     b->color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
     
     Billboard *b2 = new Billboard();
     b2->size  = glm::vec2(2.0, 2.0);
     b2->position = glm::vec3(0.0, 3.0, 1.0);
     b2->color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);

     mpBillboardList->AddBillboard(b);
     mpBillboardList->AddBillboard(b2);
     */    // TMP
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

	// Camera
	for (vector<Camera*>::iterator it = mCamera.begin(); it < mCamera.end(); ++it)
	{
		delete *it;
	}
	mCamera.clear();
    
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
	// User Inputs
	// 0 1 2 to change the Camera
	if (glfwGetKey(EventManager::GetWindow(), GLFW_KEY_1 ) == GLFW_PRESS)
	{
		mCurrentCamera = 0;
	}
	else if (glfwGetKey(EventManager::GetWindow(), GLFW_KEY_2 ) == GLFW_PRESS)
	{
		if (mCamera.size() > 1)
		{
			mCurrentCamera = 1;
		}
	}
	else if (glfwGetKey(EventManager::GetWindow(), GLFW_KEY_3 ) == GLFW_PRESS)
	{
		if (mCamera.size() > 2)
		{
			mCurrentCamera = 2;
		}
	}

	// Spacebar to change the shader
	if (glfwGetKey(EventManager::GetWindow(), GLFW_KEY_0 ) == GLFW_PRESS)
	{
		Renderer::SetShader(SHADER_SOLID_COLOR);
	}
	else if (glfwGetKey(EventManager::GetWindow(), GLFW_KEY_9 ) == GLFW_PRESS)
	{
		Renderer::SetShader(SHADER_BLUE);
	}

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
	mCamera[mCurrentCamera]->Update(dt);

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

void WorldBlock::Draw()
{
	Renderer::BeginFrame();
	
	// Set shader to use
	glUseProgram(Renderer::GetShaderProgramID());

	//mat4 offsetMatrix = WB_OffsetMatrix;
	//mat4 offsetMatrix = translate(mat4(1.0), vec3(100.0, 0.0, 0.0));
	//glUniformMatrix4fv(glGetUniformLocation(Renderer::GetShaderProgramID(), "WBOffsetMatrix"), 1, GL_FALSE, &offsetMatrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(Renderer::GetShaderProgramID(), "WBOffsetMatrix"), 1, GL_FALSE, &WB_OffsetMatrix[0][0]);

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
	mat4 VP = mCamera[mCurrentCamera]->GetViewProjectionMatrix();
	glUniformMatrix4fv(VPMatrixLocation, 1, GL_FALSE, &VP[0][0]);

	// viewTransform
	mat4 View = mCamera[mCurrentCamera]->GetViewMatrix();
	glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &View[0][0]);
	// projectionMatrix
	mat4 Projection = mCamera[mCurrentCamera]->GetProjectionMatrix();
	glUniformMatrix4fv(ProjMatrixID, 1, GL_FALSE, &Projection[0][0]);


	GLuint LightAttenuationID = glGetUniformLocation(Renderer::GetShaderProgramID(), "lightAttenuation");
	glUniform3f(LightAttenuationID, 0.0f, 0.0f, 1.0f);

	
	// glUniform3f(LightColorID, lightColor.r, lightColor.g, lightColor.b);
	//glUniform3f(LightColorID, 1.0f, 1.0f, 1.0f);
	// glUniform3f(LightAttenuationID, lightKc, lightKl, lightKq);


	//lighting
	//int size = lightSource.getSize();
	int enabled;
	vec4 lPosition;// = lightSource.getPositions();
	vec3 lColor;// = lightSource.getColors();

	

	// Light 1
	enabled = 0;
	GLuint L1EnableID = glGetUniformLocation(Renderer::GetShaderProgramID(), "L1Enable");
	GLuint L1PositionID = glGetUniformLocation(Renderer::GetShaderProgramID(), "L1Position");
	GLuint L1ColorID = glGetUniformLocation(Renderer::GetShaderProgramID(), "L1Color");

	if (lightSource.size() > 0) {
		enabled = 1;
		lPosition = lightSource[0]->getPosition();
		lColor = lightSource[0]->getColor();
	}
	
	//glUniform4f(L1PositionID, 10.f,2.0f,0.0f,1);
	glUniform1i(L1EnableID, enabled);
	glUniform4f(L1PositionID, lPosition.x, lPosition.y, lPosition.z, lPosition.w);
	glUniform3f(L1ColorID, lColor.x, lColor.y, lColor.z);

	// Light 2
	enabled = 0;

	GLuint L2EnableID = glGetUniformLocation(Renderer::GetShaderProgramID(), "L2Enable");
	GLuint L2PositionID = glGetUniformLocation(Renderer::GetShaderProgramID(), "L2Position");
	GLuint L2ColorID = glGetUniformLocation(Renderer::GetShaderProgramID(), "L2Color");

	if (lightSource.size() > 1) {
		enabled = 1;
		lPosition = lightSource[1]->getPosition();
		lColor = lightSource[1]->getColor();
	}
	glUniform1i(L2EnableID, enabled);
	glUniform4f(L2PositionID, lPosition.x, lPosition.y, lPosition.z, lPosition.w);
	glUniform3f(L2ColorID, lColor.x, lColor.y, lColor.z);


	// Light 3
	enabled = 0;

	GLuint L3EnableID = glGetUniformLocation(Renderer::GetShaderProgramID(), "L3Enable");
	GLuint L3PositionID = glGetUniformLocation(Renderer::GetShaderProgramID(), "L3Position");
	GLuint L3ColorID = glGetUniformLocation(Renderer::GetShaderProgramID(), "L3Color");

	if (lightSource.size() > 2) {
		enabled = 1;
		lPosition = lightSource[2]->getPosition();
		lColor = lightSource[2]->getColor();
	}

	glUniform1i(L3EnableID, enabled);
	glUniform4f(L3PositionID, lPosition.x, lPosition.y, lPosition.z, lPosition.w);
	glUniform3f(L3ColorID, lColor.x, lColor.y, lColor.z);

	// Light 4
	enabled = 0;

	GLuint L4EnableID = glGetUniformLocation(Renderer::GetShaderProgramID(), "L4Enable");
	GLuint L4PositionID = glGetUniformLocation(Renderer::GetShaderProgramID(), "L4Position");
	GLuint L4ColorID = glGetUniformLocation(Renderer::GetShaderProgramID(), "L4Color");

	if (lightSource.size() > 3) {
		enabled = 1;
		lPosition = lightSource[3]->getPosition();
		lColor = lightSource[3]->getColor();
	}
	glUniform1i(L4EnableID, enabled);
	glUniform4f(L4PositionID, lPosition.x, lPosition.y, lPosition.z, lPosition.w);
	glUniform3f(L4ColorID, lColor.x, lColor.y, lColor.z);

	// Light 5
	enabled = 0;

	GLuint L5EnableID = glGetUniformLocation(Renderer::GetShaderProgramID(), "L5Enable");
	GLuint L5PositionID = glGetUniformLocation(Renderer::GetShaderProgramID(), "L5Position");
	GLuint L5ColorID = glGetUniformLocation(Renderer::GetShaderProgramID(), "L5Color");

	if (lightSource.size() > 4) {
		enabled = 1;
		lPosition = lightSource[4]->getPosition();
		lColor = lightSource[4]->getColor();
	}
	glUniform1i(L5EnableID, enabled);
	glUniform4f(L5PositionID, lPosition.x, lPosition.y, lPosition.z, lPosition.w);
	glUniform3f(L5ColorID, lColor.x, lColor.y, lColor.z);

	// Light 6
	enabled = 0;

	GLuint L6EnableID = glGetUniformLocation(Renderer::GetShaderProgramID(), "L6Enable");
	GLuint L6PositionID = glGetUniformLocation(Renderer::GetShaderProgramID(), "L6Position");
	GLuint L6ColorID = glGetUniformLocation(Renderer::GetShaderProgramID(), "L6Color");

	if (lightSource.size() > 5) {
		enabled = 1;
		lPosition = lightSource[5]->getPosition();
		lColor = lightSource[5]->getColor();
	}
	glUniform1i(L6EnableID, enabled);
	glUniform4f(L6PositionID, lPosition.x, lPosition.y, lPosition.z, lPosition.w);
	glUniform3f(L6ColorID, lColor.x, lColor.y, lColor.z);


	// Light 7
	enabled = 0;

	GLuint L7EnableID = glGetUniformLocation(Renderer::GetShaderProgramID(), "L7Enable");
	GLuint L7PositionID = glGetUniformLocation(Renderer::GetShaderProgramID(), "L7Position");
	GLuint L7ColorID = glGetUniformLocation(Renderer::GetShaderProgramID(), "L7Color");

	if (lightSource.size() > 6) {
		enabled = 1;
		lPosition = lightSource[6]->getPosition();
		lColor = lightSource[6]->getColor();
	}
	glUniform1i(L7EnableID, enabled);
	glUniform4f(L7PositionID, lPosition.x, lPosition.y, lPosition.z, lPosition.w);
	glUniform3f(L7ColorID, lColor.x, lColor.y, lColor.z);

	// Light 8
	enabled = 0;

	GLuint L8EnableID = glGetUniformLocation(Renderer::GetShaderProgramID(), "L8Enable");
	GLuint L8PositionID = glGetUniformLocation(Renderer::GetShaderProgramID(), "L8Position");
	GLuint L8ColorID = glGetUniformLocation(Renderer::GetShaderProgramID(), "L8Color");

	if (lightSource.size() > 7) {
		enabled = 1;
		lPosition = lightSource[7]->getPosition();
		lColor = lightSource[7]->getColor();
	}
	glUniform1i(L8EnableID, enabled);
	glUniform4f(L8PositionID, lPosition.x, lPosition.y, lPosition.z, lPosition.w);
	glUniform3f(L8ColorID, lColor.x, lColor.y, lColor.z);

	vec4 mProperties;
	// Draw models
	for (vector<Model*>::iterator it = mModel.begin(); it < mModel.end(); ++it)
	{
		mProperties = (*it)->getProperties();
		GLuint materialCoefficientsID = glGetUniformLocation(Renderer::GetShaderProgramID(), "materialCoefficients");
		glUniform4f(materialCoefficientsID, mProperties.x, mProperties.y, mProperties.z, mProperties.w);

		(*it)->Draw();
	}

	// Draw Path Lines
	
	// Set Shader for path lines
	unsigned int prevShader = Renderer::GetCurrentShader();
	Renderer::SetShader(SHADER_PATH_LINES);
	glUseProgram(Renderer::GetShaderProgramID());
	glUniformMatrix4fv(glGetUniformLocation(Renderer::GetShaderProgramID(), "WBOffsetMatrix"), 1, GL_FALSE, &WB_OffsetMatrix[0][0]);

	// Send the view projection constants to the shader
	VPMatrixLocation = glGetUniformLocation(Renderer::GetShaderProgramID(), "ViewProjectionTransform");
	glUniformMatrix4fv(VPMatrixLocation, 1, GL_FALSE, &VP[0][0]);

	for (vector<Animation*>::iterator it = mAnimation.begin(); it < mAnimation.end(); ++it)
	{
		mat4 VP = mCamera[mCurrentCamera]->GetViewProjectionMatrix();
		glUniformMatrix4fv(VPMatrixLocation, 1, GL_FALSE, &VP[0][0]);

		(*it)->Draw();
	}

	for (vector<AnimationKey*>::iterator it = mAnimationKey.begin(); it < mAnimationKey.end(); ++it)
	{
		mat4 VP = mCamera[mCurrentCamera]->GetViewProjectionMatrix();
		glUniformMatrix4fv(VPMatrixLocation, 1, GL_FALSE, &VP[0][0]);

		(*it)->Draw();
	}

    Renderer::CheckForErrors();
    
    // Draw Billboards
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Set current shader to be the Textured Shader
	Renderer::CheckForErrors();
	ShaderType oldShader = (ShaderType)Renderer::GetCurrentShader();

	Renderer::SetShader(SHADER_TEXTURED);
	glUseProgram(Renderer::GetShaderProgramID());
	Renderer::CheckForErrors();

	glUniformMatrix4fv(glGetUniformLocation(Renderer::GetShaderProgramID(), "WBOffsetMatrix"), 1, GL_FALSE, &WB_OffsetMatrix[0][0]);

    mpBillboardList->Draw();

	Renderer::CheckForErrors();
	Renderer::SetShader(oldShader);
	Renderer::CheckForErrors();
    
	glDisable(GL_BLEND);


	// Restore previous shader
	Renderer::SetShader((ShaderType) prevShader);

	Renderer::EndFrame();
}

LightSource const WorldBlock::getLightSourceAt(int index)  {
	return *(lightSource[index]);
}


void WorldBlock::LoadScene(const char * scene_path)
{
	// Using case-insensitive strings and streams for easier parsing
	ci_ifstream input;
	input.open(scene_path, ios::in);

	// Invalid file
	if(input.fail() )
	{	 
		fprintf(stderr, "Error loading file: %s\n", scene_path);
		getchar();
		exit(-1);
	}

	ci_string item;
	while( std::getline( input, item, '[' ) )   
	{
        ci_istringstream iss( item );

		ci_string result;
		if( std::getline( iss, result, ']') )
		{
			if( result == "cube" )
			{
				// Box attributes
				CubeModel* cube = new CubeModel();
				cube->Load(iss);
				mModel.push_back(cube);
			}
            else if( result == "sphere" )
            {
                SphereModel* sphere = new SphereModel();
                sphere->Load(iss);
                mModel.push_back(sphere);
            }
			else if ( result == "animationkey" )
			{
				AnimationKey* key = new AnimationKey();
				key->Load(iss);
				mAnimationKey.push_back(key);
			}
			else if (result == "animation")
			{
				Animation* anim = new Animation();
				anim->Load(iss);
				mAnimation.push_back(anim);
			}
            else if (result == "particledescriptor")
            {
                ParticleDescriptor* psd = new ParticleDescriptor();
                psd->Load(iss);
                AddParticleDescriptor(psd);
            }
			else if ( result.empty() == false && result[0] == '#')
			{
				// this is a comment line
			}
			else if (result == "light") {
				LightSource* light = new LightSource();
				light->Load(iss);
				lightSource.push_back(light);
			}
			else
			{
				fprintf(stderr, "Error loading scene file... !");
				getchar();
				exit(-1);
			}
	    }
	}
	input.close();

	// Set Animation vertex buffers
	for (vector<Animation*>::iterator it = mAnimation.begin(); it < mAnimation.end(); ++it)
	{
		// Draw model
		(*it)->CreateVertexBuffer();
	}
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

const Camera* WorldBlock::GetCurrentCamera() const
{
     return mCamera[mCurrentCamera];
}

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
void WorldBlock::setCamera(std::vector<Camera*> mCamera) {
	for (int i = 0; i < mCamera.size(); i++) {
		this->mCamera.push_back(mCamera[i]);
	}
}
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
