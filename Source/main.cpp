//
// COMP 371 Assignment Framework
//
// Created by Nicolas Bergeron on 8/7/14.
// Updated by Gary Chang on 14/1/15
//
// Copyright (c) 2014-2019 Concordia University. All rights reserved.
//

//#include "Lights.h"
#include "Renderer.h"
#include "World.h"
#include "WorldBlock.h"
#include "EventManager.h"
#include "Billboard.h"
#include "TextureLoader.h"
#include "LightSource.h"

int main(int argc, char*argv[])
{
	EventManager::Initialize();
	Renderer::Initialize();
	//myLights myLights;
	LightSource lightSource;
	World* mWorld = World::getWorldInstance();
	//WorldBlock* worldBlock = mWorld->getWorldBlock();

    
	if (argc > 1)
	{
		mWorld->LoadScene(argv[1]);
	}
	else
	{
		// TODO - You can alternate between different scenes for testing different things
		// Static Scene contains no animation
		// Animated Scene does
#if defined(PLATFORM_OSX)		
		mWorld.LoadScene("Scenes/AnimatedSceneWithParticles.scene");
//		worldBlock->LoadScene("Scenes/AnimatedSceneWithParticles.scene");
//		worldBlock.LoadScene("Scenes/AnimatedScene.scene");
//		worldBlock.LoadScene("Scenes/StaticScene.scene");
//		worldBlock.LoadScene("Scenes/CoordinateSystem.scene");
#else
		mWorld->LoadScene("../Assets/Scenes/AnimatedSceneWithParticles.scene");
//		worldBlock->LoadScene("../Assets/Scenes/AnimatedSceneWithParticles.scene");
//		worldBlock.LoadScene("../Assets/Scenes/AnimatedScene.scene");
//		worldBlock.LoadScene("../Assets/Scenes/StaticScene.scene");
//		worldBlock.LoadScene("../Assets/Scenes/CoordinateSystem.scene");
#endif
	}

	// Main Loop
	do
	{
		// Update Event Manager - Frame time / input / events processing 
		EventManager::Update();

		// Update worldBlock
		float dt = EventManager::GetFrameTime();
		//worldBlock->Update(dt);
		mWorld->Update(dt);

		// Draw worldBlock
		//worldBlock->Draw();
		mWorld->Draw();
	}
	while(EventManager::ExitRequested() == false);

	Renderer::Shutdown();
	EventManager::Shutdown();

	return 0;
}
