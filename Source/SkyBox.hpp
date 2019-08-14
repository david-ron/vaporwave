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
#include "Model.h"
class SkyBox : public Model {
public:
    SkyBox(glm::vec3 size = glm::vec3(1.0f, 1.0f, 1.0f));
    ~SkyBox();
	
	virtual void Update(float dt);
	bool ParseLine(const std::vector<ci_string> &token);
	void Draw(glm::mat4 offsetMatrix);

	void getCornerPoint(std::vector<glm::vec3>&);
private:
    std::vector<std::string> faces;
    void loadCubemap(std::vector<std::string> faces);
    unsigned int cubeMapId;
    unsigned int mVAO;
    unsigned int mVBO;
	std::vector<glm::vec3> CornerPoint;
};
#endif /* SkyBox_hpp */
