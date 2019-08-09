#include "World.h"

#include "StaticCamera.h"
#include "FirstPersonCamera.h"

#include "Renderer.h"
#include "CubeModel.h"
#include "SphereModel.h"
#include "Animation.h"
#include "Billboard.h"
#include <GLFW/glfw3.h>
#include "EventManager.h"
#include "TextureLoader.h"
#include "CubeObj.hpp"
#include "ParticleDescriptor.h"
#include "ParticleEmitter.h"
#include "ParticleSystem.h"

#include "LightSource.h"

World* World::worldInstance;
const float World::WorldBlockSize = 100;

void World::LoadScene(const char * scene_path) {
	//mWorldBlock->LoadScene(scene_path);

	// Using case-insensitive strings and streams for easier parsing
	ci_ifstream input;
	input.open(scene_path, ios::in);

	// Invalid file
	if (input.fail())
	{
		fprintf(stderr, "Error loading file: %s\n", scene_path);
		getchar();
		exit(-1);
	}

	ci_string item;
	while (std::getline(input, item, '['))
	{
		ci_istringstream iss(item);

		ci_string result;
		if (std::getline(iss, result, ']'))
		{
			if (result == "cube")
			{
				// Box attributes
				CubeModel* cube = new CubeModel();
				cube->Load(iss);
				mModel.push_back(cube);
			}
			else if (result == "sphere")
			{
				SphereModel* sphere = new SphereModel();
				sphere->Load(iss);
				mModel.push_back(sphere);
			}
			else if (result == "animationkey")
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
			else if (result.empty() == false && result[0] == '#')
			{
				// this is a comment line
			}
			else if (result == "light") {
				LightSource* light = new LightSource();
				light->Load(iss);
				lightSource.push_back(light);
			}
            else if (result == "object"){
                CubeObj* cube = new CubeObj();
                cube->Load(iss);
                mModel.push_back(cube);
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


	setupWorldBlock(mWorldBlock[0]);
	checkNeighbors();
}

void World::setupWorldBlock(WorldBlock* WB) {
	
	WB->setAnimationKey(mAnimationKey);
	WB->setAnimation(mAnimation);
	WB->setParticleSystemList(mParticleSystemList);
	WB->setParticleDescriptorList(mParticleDescriptorList);

	WB->setModel(mModel);
	
	//WB->setCamera(mCamera);
	//WB->setCurrentCamera(mCurrentCamera);
	WB->setLightSource(lightSource);
	WB->setBillboardList(mpBillboardList);

}

void World::Update(float dt) {

	// update charater 

	// Prevent from having the camera move only when the cursor is within the windows
	EventManager::DisableMouseCursor();


	// The Camera moves based on the User inputs
	// - You can access the mouse motion with EventManager::GetMouseMotionXY()
	// - For mapping A S D W, you can look in WorldBlock.cpp for an example of accessing key states
	// - Don't forget to use dt to control the speed of the camera motion

	// Mouse motion to get the variation in angle
	mHorizontalAngle -= EventManager::GetMouseMotionX() * mAngularSpeed * dt;
	mVerticalAngle -= EventManager::GetMouseMotionY() * mAngularSpeed * dt;

	// Clamp vertical angle to [-85, 85] degrees
	mVerticalAngle = std::max(-85.0f, std::min(85.0f, mVerticalAngle));
	if (mHorizontalAngle > 360)
	{
		mHorizontalAngle -= 360;
	}
	else if (mHorizontalAngle < -360)
	{
		mHorizontalAngle += 360;
	}

	float theta = radians(mHorizontalAngle);
	float phi = radians(mVerticalAngle);

	mcLookAt = vec3(cosf(phi)*cosf(theta), sinf(phi), -cosf(phi)*sinf(theta));

	mcSideVector = glm::cross(mcLookAt, vec3(0.0f, 1.0f, 0.0f));
	glm::normalize(mcSideVector);

	// A S D W for motion along the camera basis vectors
	// Forward
	if (glfwGetKey(EventManager::GetWindow(), GLFW_KEY_W) == GLFW_PRESS) 
	{
		vec3 direction = mcLookAt;

		if ( (mcPosition.y - mcRadius) <= 0 && dot(direction,vec3(0,1,0)) < 0.0f) {
			vec3 mSideVector = cross(vec3(0, 1, 0), direction);
			direction = cross(mSideVector, vec3(0, 1, 0));
			direction = normalize(direction);
		}

		if (glfwGetMouseButton(EventManager::GetWindow(), GLFW_MOUSE_BUTTON_1) == GLFW_PRESS)
			mcPosition += direction * dt * mCharacterDefaultSpeed * mCharacterSpeedUpRate;
		else
			mcPosition += direction * dt * mCharacterDefaultSpeed;
	}
	// Backward
	if (glfwGetKey(EventManager::GetWindow(), GLFW_KEY_S) == GLFW_PRESS)
	{
		vec3 direction = mcLookAt;

		

		if (glfwGetMouseButton(EventManager::GetWindow(), GLFW_MOUSE_BUTTON_1) == GLFW_PRESS)
			mcPosition -= direction * dt * mCharacterDefaultSpeed * mCharacterSpeedUpRate;
		else
			mcPosition -= direction * dt * mCharacterDefaultSpeed;
	}
	// To the left
	if (glfwGetKey(EventManager::GetWindow(), GLFW_KEY_D) == GLFW_PRESS)
	{
		if (glfwGetMouseButton(EventManager::GetWindow(), GLFW_MOUSE_BUTTON_1) == GLFW_PRESS)
			mcPosition += mcSideVector * dt * mCharacterDefaultSpeed * mCharacterSpeedUpRate;
		else
			mcPosition += mcSideVector * dt * mCharacterDefaultSpeed;
	}
	// To the right
	if (glfwGetKey(EventManager::GetWindow(), GLFW_KEY_A) == GLFW_PRESS)
	{
		if (glfwGetMouseButton(EventManager::GetWindow(), GLFW_MOUSE_BUTTON_1) == GLFW_PRESS)
			mcPosition -= mcSideVector * dt * mCharacterDefaultSpeed * mCharacterSpeedUpRate;
		else
			mcPosition -= mcSideVector * dt * mCharacterDefaultSpeed;
	}
	// Up
	if (glfwGetKey(EventManager::GetWindow(), GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		if (glfwGetMouseButton(EventManager::GetWindow(), GLFW_MOUSE_BUTTON_1) == GLFW_PRESS)
			mcPosition += vec3(0, 1, 0) * dt * mCharacterDefaultSpeed * mCharacterSpeedUpRate;
		else
			mcPosition += vec3(0, 1, 0) * dt * mCharacterDefaultSpeed;
	}
	// Back to intial
	if (glfwGetKey(EventManager::GetWindow(), GLFW_KEY_O) == GLFW_PRESS)
	{
		if (glfwGetKey(EventManager::GetWindow(), GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS ||
			glfwGetKey(EventManager::GetWindow(), GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
			mcPosition = mcPositionInitial;
	}


	// Update current Camera
	mCamera[mCurrentCamera]->Update(dt);


	// check for the center block
	int x = floor((mcPosition.x + 50) / 100);
	int z = floor((mcPosition.z + 50) / 100);
	vec2 newCenter = vec2(x, z);
	if (newCenter != CenterBlock) {
		CenterBlock = newCenter;
		checkNeighbors();
	}
	




	// User Inputs
	// 0 1 2 to change the Camera
	if (glfwGetKey(EventManager::GetWindow(), GLFW_KEY_1) == GLFW_PRESS)
	{
		mCurrentCamera = 0;
	}
	else if (glfwGetKey(EventManager::GetWindow(), GLFW_KEY_2) == GLFW_PRESS)
	{
		if (mCamera.size() > 1)
		{
			mCurrentCamera = 1;
		}
	}
	else if (glfwGetKey(EventManager::GetWindow(), GLFW_KEY_3) == GLFW_PRESS)
	{
		if (mCamera.size() > 2)
		{
			mCurrentCamera = 2;
		}
	}

	// Spacebar to change the shader
	if (glfwGetKey(EventManager::GetWindow(), GLFW_KEY_0) == GLFW_PRESS)
	{
		Renderer::SetShader(SHADER_SOLID_COLOR);
	}
	else if (glfwGetKey(EventManager::GetWindow(), GLFW_KEY_9) == GLFW_PRESS)
	{
		Renderer::SetShader(SHADER_BLUE);
	}

	for (vector<ParticleSystem*>::iterator it = mParticleSystemList.begin(); it != mParticleSystemList.end(); ++it)
	{
		(*it)->Update(dt);
	}

	mpBillboardList->Update(dt);

	mWorldBlock[0]->Update(dt);
	//mWorldBlock0->Update(dt);

}


void World::Draw() {

	glClearColor(0.1f, 0.13f, 0.2f,0.0f);
	//mWorldBlock0->Draw();
	//mWorldBlock1->Draw();

	//first shader
	Renderer::BeginFrame();
	// Set shader to use
	glUseProgram(Renderer::GetShaderProgramID());
	Renderer::CheckForErrors();

	for (int i = 0; i < 9; i++) {
		mWorldBlock[DisplayedWBIndex[i]]->DrawCurrentShader();
	}

	
	// path lines shader
	Renderer::CheckForErrors();
	unsigned int prevShader = Renderer::GetCurrentShader();
	Renderer::SetShader(SHADER_PATH_LINES);
	glUseProgram(Renderer::GetShaderProgramID());
	Renderer::CheckForErrors();

	for (int i = 0; i < 9; i++) {
		mWorldBlock[DisplayedWBIndex[i]]->DrawPathLinesShader();
	}



	Renderer::CheckForErrors();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	Renderer::CheckForErrors();

	ShaderType oldShader = (ShaderType)Renderer::GetCurrentShader();
	//Texture shader
	Renderer::SetShader(SHADER_TEXTURED);
	glUseProgram(Renderer::GetShaderProgramID());

	Renderer::CheckForErrors();

	for (int i = 0; i < 9; i++) {
		mWorldBlock[DisplayedWBIndex[i]]->DrawTextureShader();
	}

	//mWorldBlock0->DrawTextureShader();
	//mWorldBlock1->DrawTextureShader();
	//mWorldBlock2->DrawTextureShader();
	//mWorldBlock3->DrawTextureShader();
	//mWorldBlock4->DrawTextureShader();
	//mWorldBlock5->DrawTextureShader();
	//mWorldBlock6->DrawTextureShader();
	//mWorldBlock7->DrawTextureShader();
	//mWorldBlock8->DrawTextureShader();

	Renderer::CheckForErrors();
	Renderer::SetShader(oldShader);
	Renderer::CheckForErrors();

	glDisable(GL_BLEND);


	// Restore previous shader
	Renderer::SetShader((ShaderType)prevShader);

	Renderer::EndFrame();
}

//WorldBlock* World::getWorldBlock() const {
//	return mWorldBlock1;
//}

World* World::getWorldInstance() {
	if (worldInstance == NULL)
	{
		worldInstance = new World();
	}

	return worldInstance;
}



World::World() {

	mcPosition = vec3(3.0f, 5.0f, 20.0f);
	mcPositionInitial = mcPosition;
	mcLookAt = vec3(0.0f, 0.0f, -1.0f);

	CenterBlock = vec2(0, 0);
	// Neighbors = getNeighbors(CenterBlock);
	mWorldBlock.push_back(new WorldBlock(vec2(0, 0)));
	mWorldBlock[0]->setOnThis(true);
	oldCenterBlock = mWorldBlock[0];
	
	

	//mWorldBlock0 = new WorldBlock(vec2(0, 0));
	//mWorldBlock1 = new WorldBlock(vec2(1, 0));
	//mWorldBlock2 = new WorldBlock(vec2(-1, 0));
	//mWorldBlock3 = new WorldBlock(vec2(1, 1));
	//mWorldBlock4 = new WorldBlock(vec2(0, 1));
	//mWorldBlock5 = new WorldBlock(vec2(-1, 1));
	//mWorldBlock6 = new WorldBlock(vec2(-1, -1));
	//mWorldBlock7 = new WorldBlock(vec2(0, -1));
	//mWorldBlock8 = new WorldBlock(vec2(1, -1));

	//mWorldBlock.insert()



	// Setup Camera
	//mCamera.push_back(new FirstPersonCamera(vec3(3.0f, 5.0f, 20.0f)));
	mCamera.push_back(new FirstPersonCamera(mcPosition, mcLookAt));
	//mCamera.push_back(new StaticCamera(vec3(3.0f, 30.0f, 5.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f)));
	//mCamera.push_back(new StaticCamera(vec3(0.5f, 0.5f, 5.0f), vec3(0.0f, 0.5f, 0.0f), vec3(0.0f, 1.0f, 0.0f)));
	mCurrentCamera = 0;

#if defined(PLATFORM_OSX)
	//    int billboardTextureID = TextureLoader::LoadTexture("Textures/BillboardTest.bmp");
	int billboardTextureID = TextureLoader::LoadTexture("Textures/Particle.png");
#else
	//    int billboardTextureID = TextureLoader::LoadTexture("../Assets/Textures/BillboardTest.bmp");
	int billboardTextureID = TextureLoader::LoadTexture("../Assets/Textures/Color.png");
#endif
	assert(billboardTextureID != 0);

	mpBillboardList = new BillboardList(2048, billboardTextureID);
}


World::~World() {
	//delete mWorldBlock0;
	//delete mWorldBlock1;
	//delete mWorldBlock2;
	//delete mWorldBlock3;
	//delete mWorldBlock4;
	//delete mWorldBlock5;
	//delete mWorldBlock6;
	//delete mWorldBlock7;
	//delete mWorldBlock8;
};


void World::AddParticleDescriptor(ParticleDescriptor* particleDescriptor)
{
	mParticleDescriptorList.push_back(particleDescriptor);
}


Animation* World::FindAnimation(ci_string animName)
{
	for (std::vector<Animation*>::iterator it = mAnimation.begin(); it < mAnimation.end(); ++it)
	{
		if ((*it)->GetName() == animName)
		{
			return *it;
		}
	}
	return nullptr;
}

AnimationKey* World::FindAnimationKey(ci_string keyName)
{
	for (std::vector<AnimationKey*>::iterator it = mAnimationKey.begin(); it < mAnimationKey.end(); ++it)
	{
		if ((*it)->GetName() == keyName)
		{
			return *it;
		}
	}
	return nullptr;
}

ParticleDescriptor* World::FindParticleDescriptor(ci_string name)
{
	for (std::vector<ParticleDescriptor*>::iterator it = mParticleDescriptorList.begin(); it < mParticleDescriptorList.end(); ++it)
	{
		if ((*it)->GetName() == name)
		{
			return *it;
		}
	}
	return nullptr;
}


Camera* World::GetCurrentCamera() const
{
	return mCamera[mCurrentCamera];
}

LightSource const World::getLightSourceAt(int index) {
	return *(lightSource[index]);
}

void World::AddParticleSystem(ParticleSystem* particleSystem)
{
	mParticleSystemList.push_back(particleSystem);
}

void World::AddBillboard(Billboard* b)
{
	mpBillboardList->AddBillboard(b);
}

void World::RemoveBillboard(Billboard* b)
{
	mpBillboardList->RemoveBillboard(b);
}

void World::checkNeighbors() {
	mNeighbors.clear();


	mNeighbors.push_back(vec2(CenterBlock.x - 1, CenterBlock.y + 1));	// upper left
	mNeighbors.push_back(vec2(CenterBlock.x, CenterBlock.y + 1));		// upper mid
	mNeighbors.push_back(vec2(CenterBlock.x + 1, CenterBlock.y + 1));	// upper right
	mNeighbors.push_back(vec2(CenterBlock.x - 1, CenterBlock.y));		// level left
	mNeighbors.push_back(vec2(CenterBlock.x + 1, CenterBlock.y));		// level right
	mNeighbors.push_back(vec2(CenterBlock.x - 1, CenterBlock.y - 1));	// lower left
	mNeighbors.push_back(vec2(CenterBlock.x, CenterBlock.y - 1));		// lower mid
	mNeighbors.push_back(vec2(CenterBlock.x + 1, CenterBlock.y - 1));	// lower right
	assert(mNeighbors.size() == 8);

	// generate the world blocks that were not called before
	int dIndex = 0;
	for (int i = 0; i < 8; i++) {
		bool isNew = true;
		for (int j = 0; j < mWorldBlock.size(); j++) {
			if (mWorldBlock[j]->getWorldBlockCoor() == mNeighbors[i]) {
				DisplayedWBIndex[dIndex++] = j;
				isNew = false;
				break;
			}
				
		}
		if (isNew) {
			DisplayedWBIndex[dIndex++] = mWorldBlock.size();
			WorldBlock* newWorldBlock = new WorldBlock(mNeighbors[i]);
			setupWorldBlock(newWorldBlock);
			mWorldBlock.push_back(newWorldBlock);
		}
			//mWorldBlock.push_back(new WorldBlock(mNeighbors[i]));
	}

	for (int i = 0; i < mWorldBlock.size(); i++) {
		if (mWorldBlock[i]->getWorldBlockCoor() == CenterBlock) {
			DisplayedWBIndex[8] = i;
			oldCenterBlock->setOnThis(false);
			oldCenterBlock = mWorldBlock[i];
			oldCenterBlock->setOnThis(true);
			break;
		}
	}

}
