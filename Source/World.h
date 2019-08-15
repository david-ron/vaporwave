#pragma once

#include <glm/glm.hpp>
#include "WorldBlock.h"
#include "Model.h"
#include "MainCharacter.hpp"
#include "Terrain\Terrain.h"
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
	void AddMCBillborad(Billboard* b) { mcBillboardList->AddBillboard(b); }
	void RemoveBillboard(Billboard* b);
	void RemoveMCBillboard(Billboard* b) { mcBillboardList->RemoveBillboard(b); };
	void AddParticleSystem(ParticleSystem* particleSystem);

	void AddParticleDescriptor(ParticleDescriptor* particleDescriptor);
	void AddMCParticleSystem(ParticleSystem* particleSystem) { this->mcParticleSystem = particleSystem; }
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
	Camera* getTCP() { return mCamera[1]; }

	vec3 getMClookAt() { return mcLookAt; }
	vec3 getMCposition() { return mcPosition; }
	float getMCradius() { return mcRadius; }
	float getVerticalAngle() { return mVerticalAngle; }
	float getHorizontalAngle() { return mHorizontalAngle; }

	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

	static const float WorldBlockSize;

private:
	World();
	static World* worldInstance;


	vec2 CenterBlock;
	vector<vec2> mNeighbors;
	vector<WorldBlock*> mWorldBlock;
	WorldBlock* oldCenterBlock;
	
	int DisplayedWBIndex[9];

	
	std::vector<Model*> mModel;
	int SphereIndex;
	Terrain* mTerrain;
	Model* mBuildingModel = nullptr;
	vector<vec3> cornerPoint;		// 8 corner points for the model
	vector<mat4> mBuildingsMw;		// all Buildings' world matrixes in the current 9 blocks
	std::vector<Animation*> mAnimation;
	std::vector<AnimationKey*> mAnimationKey;
	std::vector<Camera*> mCamera;
	std::vector<ParticleSystem*> mParticleSystemList;
	std::vector<ParticleDescriptor*> mParticleDescriptorList;
	
	unsigned int mCurrentCamera;
	MainCharacter* mCharater;
	vec3 mcPositionInitial; 
	vec3 mcPosition;		// my character's position
	const float mcRadius = 5.0f;
	vec3 mcLookAt;			// my character's facing direction(lookAt vector for FPV)
	vec3 mcSideVector;
	vec3 cLookAt;
	vec3 cSideVector;
	const float mCharacterDefaultSpeed = 5.0f;
	const float mCharacterSpeedUpRate = 5.0f;
	const float mAngularSpeed = 2.5f;
	float mVerticalAngle = 0.0f;
	float mHorizontalAngle = 90.0f;
	BillboardList* mcBillboardList;
	ParticleSystem* mcParticleSystem;

    SkyBox* mskybox;
	std::vector<LightSource*> lightSource;

	BillboardList* mpBillboardList;


	// private functions
	void checkNeighbors();


};

