//
//  SphereObj.hpp
//  COMP371_Framework
//
//  Created by David Ronci on 2019-08-08.
//  Copyright © 2019 Concordia. All rights reserved.
//

#ifndef SphereObj_hpp
#define SphereObj_hpp
#include "Model.h"

class SphereObj : public Model
{
public:
    SphereObj(glm::vec3 size = glm::vec3(1.0f, 1.0f, 1.0f));
    virtual ~SphereObj();
    
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
    const std::string sphereObjFile = "./Models/sphere.obj";
#else
    const std::string sphereObjFile ="../Assets/Models/sphere.obj";
#endif
    unsigned int mVAO;
    unsigned int mVBO;
    unsigned int vertexCount;
    
    glm::vec3 max;
    glm::vec3 min;
    
};

#endif /* SphereObj_hpp */
