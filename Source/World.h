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
	void setCharacterPosition(vec3 cPosition) { mcPosition = cPosition; }
	void updateMCharacterPosition(vec3 newPosition) { mcPosition = newPosition; }

	Animation* FindAnimation(ci_string animName);
	AnimationKey* FindAnimationKey(ci_string keyName);
	ParticleDescriptor* FindParticleDescriptor(ci_string name);
	Camera* GetCurrentCamera() const;
	const int getLightSize() { return lightSource.size(); };
	const LightSource getLightSourceAt(int);
	vec3 getCharacterPosition() const { return mcPosition; }
	vec3 getMCpositionInitial() const { return mcPositionInitial; }
	vec3 getMCsideVector() const { return mcSideVector; }

	vec3 getMClookAt() { return mcLookAt; }
	vec3 getMCposition() { return mcPosition; }


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
	
	int DisplayedWBIndex[9];

	std::vector<Model*> mModel;
	std::vector<Animation*> mAnimation;
	std::vector<AnimationKey*> mAnimationKey;
	std::vector<Camera*> mCamera;
	std::vector<ParticleSystem*> mParticleSystemList;
	std::vector<ParticleDescriptor*> mParticleDescriptorList;
	
	unsigned int mCurrentCamera;

	vec3 mcPositionInitial; 
	vec3 mcPosition;		// my character's position
	const float mcRadius = 10.0f;
	vec3 mcLookAt;			// my character's facing direction(lookAt vector for FPV)
	vec3 mcSideVector;
	const float mCharacterDefaultSpeed = 5.0f;
	const float mCharacterSpeedUpRate = 4.0f;
	const float mAngularSpeed = 2.5f;

	float mVerticalAngle = 0.0f;
	float mHorizontalAngle = 90.0f;


	std::vector<LightSource*> lightSource;

	BillboardList* mpBillboardList;
	PixelList* mpPixelList;



	// private functions
	void checkNeighbors();


};

