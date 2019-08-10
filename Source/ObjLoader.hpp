//
//  ObjLoader.hpp
//  COMP371_Framework
//
//  Created by David Ronci on 2019-08-09.
//  Copyright © 2019 Concordia. All rights reserved.
//

#ifndef ObjLoader_hpp
#define ObjLoader_hpp
#include <glm/glm.hpp>
#include <cstring>
#include <vector>
#include <string>

class ObjLoader
{
public:
    static bool loadOBJ(const char * path,
                       std::vector<glm::vec3> & out_vertices,
                       std::vector<glm::vec3> & out_normals,
                       std::vector<glm::vec2> & out_uvs,
                       glm::vec3 & max,
                       glm::vec3 & min);
    
private:
    
};

#endif /* ObjLoader_hpp */
