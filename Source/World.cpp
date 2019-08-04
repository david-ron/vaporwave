#include "World.h"

#include "StaticCamera.h"
#include "FirstPersonCamera.h"

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

	setupWorldBlock(mWorldBlock);
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

}



void World::Update(float dt) {
	mWorldBlock->Update(dt);
}
void World::Draw() {
	mWorldBlock->Draw();
}

World* World::worldInstance;

World* World::getWorldInstance() {
	if (worldInstance == NULL)
	{
		worldInstance = new World();
	}

	return worldInstance;
}

WorldBlock* World::getWorldBlock() const{ 
	return mWorldBlock;
}

World::World() {
	mWorldBlock = new WorldBlock();
}
World::~World() {
	delete mWorldBlock;
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