//
//  SkyBox.hpp
//  COMP371_Framework
//
//  Created by David Ronci on 2019-08-10.
//  Copyright © 2019 Concordia. All rights reserved.
//

#ifndef SkyBox_hpp
#define SkyBox_hpp

#include <stdio.h>
#include <vector>
#include <glm/glm.hpp>

class SkyBox{
public:
    SkyBox();
    ~SkyBox();

    
    void Draw();
    
private:
    std::vector<std::string> faces;
    void loadCubemap(std::vector<std::string> faces);
    unsigned int cubeMapId;
    unsigned int mVAO;
    unsigned int mVBO;
};
#endif /* SkyBox_hpp */
