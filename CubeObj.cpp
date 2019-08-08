//
//  CubeObj.cpp
//  COMP371_Framework
//
//  Created by David Ronci on 2019-08-03.
//  Copyright © 2019 Concordia. All rights reserved.
//

#include "CubeObj.hpp"
#include "Renderer.h"
#include "objLoader.hpp"

using namespace glm;
using namespace std;

CubeObj::CubeObj(glm::vec3 size) : Model(){
    //string path, int& vertexCount
    //string path;
    //vector<int> vertexIndices; //The contiguous sets of three indices of vertices, normals and UVs, used to make a triangle
    vector<glm::vec3> vertices;
    vector<glm::vec3> normals;
    vector<glm::vec2> UVs;
    
    //read the vertices from the cube.obj file
    //We won't be needing the normals or UVs for this program
    loadOBJ(cubeObjFile.c_str(),vertices, normals, UVs);
    
    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO); //Becomes active VAO
    // Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
    
    //Vertex VBO setup
    GLuint vertices_VBO;
    glGenBuffers(1, &vertices_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, vertices_VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices.front(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    
    //Normals VBO setup
    GLuint normals_VBO;
    glGenBuffers(1, &normals_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, normals_VBO);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals.front(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(1);
    
    //UVs VBO setup
    GLuint uvs_VBO;
    glGenBuffers(1, &uvs_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, uvs_VBO);
    glBufferData(GL_ARRAY_BUFFER, UVs.size() * sizeof(glm::vec2), &UVs.front(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(2);
    
    glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO
    vertexCount = vertices.size();
}

void CubeObj::Draw(glm::mat4 offsetMatrix)
{
    // Draw the Vertex Buffer
    // Note this draws a unit Cube
    // The Model View Projection transforms are computed in the Vertex Shader
    glBindVertexArray(mVAO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    
    GLuint WorldMatrixLocation = glGetUniformLocation(Renderer::GetShaderProgramID(), "WorldTransform");
    glm::mat4 WorldMatrix = offsetMatrix * GetWorldMatrix();
    glUniformMatrix4fv(WorldMatrixLocation, 1, GL_FALSE, &WorldMatrix[0][0]);
    //glUniformMatrix4fv(WorldMatrixLocation, 1, GL_FALSE, mAnimation->GetAnimationWorldMatrix()[0][0]);
    
    // Get a handle for Material Attributes uniform
    GLuint MaterialID = glGetUniformLocation(Renderer::GetShaderProgramID(), "materialCoefficients");
    // glUniform4f(MaterialID, ka, kd, ks, n);
    glUniform4f(MaterialID, 0.2f, 0.8f, 0.2f, 50);
    
    // Draw the triangles !
    glDrawArrays(GL_TRIANGLES, 0, vertexCount); // 36 vertices: 3 * 2 * 6 (3 per triangle, 2 triangles per face, 6 faces)
}

void CubeObj::Update(float dt)
{
    // If you are curious, un-comment this line to have spinning cubes!
    // That will only work if your world transform is correct...
    // mRotationAngleInDegrees += 90 * dt; // spins by 90 degrees per second
    
    Model::Update(dt);
}
bool CubeObj::ParseLine(const std::vector<ci_string> &token){
    if (token.empty())
    {
        return true;
    }
    else
    {
        return Model::ParseLine(token);
    }
}
CubeObj::~CubeObj()
{
    // Free the GPU from the Vertex Buffer
    glDeleteBuffers(1, &mVBO);
    glDeleteVertexArrays(1, &mVAO);
}

