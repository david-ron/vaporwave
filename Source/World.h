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

private:
	World();
	static World* worldInstance;
	WorldBlock* mWorldBlock;
};

