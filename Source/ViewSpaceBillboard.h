//
// COMP 371 Assignment Framework
//
// Created by Nicolas Bergeron on 15/7/15.
//
// Copyright (c) 2014-2019 Concordia University. All rights reserved.
//

#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <list>
#include "Billboard.h"

using namespace std;
using namespace glm;


// We should render billboards in the fewest amount of render calls possible
// Billboards are semi-transparent, so they need to be sorted and rendered from back to front
class ViewSpaceBillboard
{
public:
	ViewSpaceBillboard(unsigned int maxNumBillboards);
    ~ViewSpaceBillboard();
    
    void AddBillboard(Billboard* b);
    void RemoveBillboard(Billboard* b);
    
    void Update(float dt);
    //void Draw(glm::mat4 offsetMatrix);
	void Draw();
	void AddTextureID(int id) { mTextureID.push_back(id); }
    
private:
    // Each vertex on a billboard
    struct BillboardVertex
    {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec4 color;
        glm::vec2 textureCoordinate;
    };

    
    std::vector<BillboardVertex> mVertexBuffer;
    std::list<Billboard*> mBillboardList;
    
    vector<int> mTextureID;
    unsigned int mMaxNumBillboards;

    unsigned int mVAO;
    unsigned int mVBO;
};


