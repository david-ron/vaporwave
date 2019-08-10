//
//  ShadowMapDetails.hpp
//  COMP371_Framework
//
//  Created by David Ronci on 2019-08-10.
//  Copyright © 2019 Concordia. All rights reserved.
//

#ifndef ShadowMapDetails_hpp
#define ShadowMapDetails_hpp

#include <stdio.h>
#include <vector>
#include <glm/glm.hpp>
#include "LightSource.h"


class ShadowMapDetails{
public:
    glm::mat4 getLightViewMatrix(LightSource LightSource);
    
private:

};
#endif /* ShadowMapDetails_hpp */

