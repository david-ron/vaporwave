#include "World.h"

void World::LoadScene(const char * scene_path) {
	mWorldBlock->LoadScene(scene_path);
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
