#pragma once

#include <glm/glm.hpp>
#include "WorldBlock.h"

using namespace std;
using namespace glm;

class World
{
public:
	static World* getWorldInstance();
	~World();
	
	WorldBlock* getWorldBlock()const;
	void Update(float dt);
	void Draw();
	void LoadScene(const char * scene_path);

	void AddParticleDescriptor(ParticleDescriptor* particleDescriptor);
	void setupWorldBlock(WorldBlock*);

	Animation* FindAnimation(ci_string animName);
	AnimationKey* FindAnimationKey(ci_string keyName);
	ParticleDescriptor* FindParticleDescriptor(ci_string name);

private:
	World();
	static World* worldInstance;
	WorldBlock* mWorldBlock;

	std::vector<Model*> mModel;
	std::vector<Animation*> mAnimation;
	std::vector<AnimationKey*> mAnimationKey;
	std::vector<Camera*> mCamera;
	std::vector<ParticleSystem*> mParticleSystemList;
	std::vector<ParticleDescriptor*> mParticleDescriptorList;
	unsigned int mCurrentCamera;
	std::vector<LightSource*> lightSource;


};

