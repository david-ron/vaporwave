//
//  MainCharacter.hpp
//  COMP371_Framework
//
//  Created by David Ronci on 2019-08-10.
//  Copyright © 2019 Concordia. All rights reserved.
//
#pragma once

#ifndef MainCharacter_hpp
#define MainCharacter_hpp
#include <stdio.h>
#include "Model.h"

class MainCharacter : public Model
{
public:
    MainCharacter(glm::vec3 size = glm::vec3(1.0f, 1.0f, 1.0f));
    virtual ~MainCharacter();
    
    virtual void Update(float dt);
    virtual void Draw(glm::mat4 offsetMatrix);
	//virtual void Draw();
    
    void getCornerPoint(std::vector<glm::vec3>&);
    //virtual bool isCollided();
    
protected:
    virtual bool ParseLine(const std::vector<ci_string> &token);
    
private:
    // The vertex format could be different for different types of models
    struct Vertexs
    {
        std::vector<glm::vec3> position;
        std::vector<glm::vec3> normal;
        std::vector<glm::vec3> uv;
    };
    
#if defined(PLATFORM_OSX)
    const std::string characterObjFile = "./Models/gameChar.obj";
#else
    const std::string characterObjFile ="../Assets/Models/gameChar.obj";
#endif
    unsigned int mVAO;
    unsigned int mVBO;
    unsigned int vertexCount;
    unsigned int textureID;
    glm::vec3 max;
    glm::vec3 min;
    glm::vec3 mLookAt;
    std::vector<glm::vec3> CornerPoint;
    
	float breakTime = 30;
	float aniTime = 12;
	float timer = 0;
	glm::mat4 HeadMatrix;

};



#endif /* MainCharacter_hpp */
