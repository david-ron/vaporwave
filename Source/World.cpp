#include "World.h"


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
