//
//  CubeObj.hpp
//  COMP371_Framework
//
//  Created by David Ronci on 2019-08-03.
//  Copyright © 2019 Concordia. All rights reserved.
//



#pragma once
#include <stdio.h>
#include "Model.h"

class CubeObj : public Model
{
public:
    CubeObj(glm::vec3 size = glm::vec3(1.0f, 1.0f, 1.0f));
    virtual ~CubeObj();
    
    virtual void Update(float dt);
    virtual void Draw(glm::mat4 offsetMatrix);
    
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
    const std::string cubeObjFile = "./Models/cube.obj";
#else
    const std::string cubeObjFile ="../Assets/Models/cube.obj";
#endif
    unsigned int mVAO;
    unsigned int mVBO;
    unsigned int vertexCount;

	glm::vec3 max;
	glm::vec3 min;

};


