#pragma once

#include <glm/glm.hpp>
#include "WorldBlock.h"

using namespace std;
using namespace glm;
//->getWorldBlock()
class World
{
public:
	static World* getWorldInstance();
	~World();
	
	//WorldBlock* getWorldBlock()const;
	void Update(float dt);
	void Draw();
	void LoadScene(const char * scene_path);

	void AddBillboard(Billboard* b);
	void RemoveBillboard(Billboard* b);
	void AddParticleSystem(ParticleSystem* particleSystem);
	void AddParticleDescriptor(ParticleDescriptor* particleDescriptor);
	void setupWorldBlock(WorldBlock*);
	void setCharacterPosition(vec3 cPosition) { mCharacterPosition = cPosition; }
	void updateMCharacterPosition(vec3 newPosition) { mCharacterPosition = newPosition; }

	Animation* FindAnimation(ci_string animName);
	AnimationKey* FindAnimationKey(ci_string keyName);
	ParticleDescriptor* FindParticleDescriptor(ci_string name);
	Camera* GetCurrentCamera() const;
	const int getLightSize() { return lightSource.size(); };
	const LightSource getLightSourceAt(int);
	vec3 getCharacterPosition() const { return mCharacterPosition; }

	static const float WorldBlockSize;

private:
	World();
	static World* worldInstance;
	//WorldBlock* mWorldBlock0;
	//WorldBlock* mWorldBlock1;
	//WorldBlock* mWorldBlock2;
	//WorldBlock* mWorldBlock3;
	//WorldBlock* mWorldBlock4;
	//WorldBlock* mWorldBlock5;
	//WorldBlock* mWorldBlock6;
	//WorldBlock* mWorldBlock7;
	//WorldBlock* mWorldBlock8;

	vec2 CenterBlock;
	vector<vec2> mNeighbors;
	vector<WorldBlock*> mWorldBlock;
	vec3 mCharacterPosition;
	int DisplayedWBIndex[9];

	std::vector<Model*> mModel;
	std::vector<Animation*> mAnimation;
	std::vector<AnimationKey*> mAnimationKey;
	std::vector<Camera*> mCamera;
	std::vector<ParticleSystem*> mParticleSystemList;
	std::vector<ParticleDescriptor*> mParticleDescriptorList;
	unsigned int mCurrentCamera;
	const float mCharacterDefaultSpeed = 5.0f;
	const float mCharacterSpeedUpRate = 4.0f;
	std::vector<LightSource*> lightSource;

	BillboardList* mpBillboardList;



	// private functions
	void checkNeighbors();


};

