//
// COMP 371 Assignment Framework
//
// Created by Nicolas Bergeron on 8/7/14.
// Updated by Gary Chang on 14/1/15
//
// Copyright (c) 2014-2019 Concordia University. All rights reserved.
//

#pragma once

struct GLFWwindow;

// Include GLEW - OpenGL Extension Wrangler
#define GLEW_STATIC 1
#include <GL/glew.h>

#include <vector>
#include <glm/glm.hpp>


enum ShaderType
{
	SHADER_SOLID_COLOR,
	SHADER_PATH_LINES,
	SHADER_BLUE,
    SHADER_TEXTURED,
    SHADER_SKY,
	SHADER_LIGHTSOURCE,
	NUM_SHADERS
};


class Renderer
{
public:
	static void Initialize();
	static void Shutdown();

	static void BeginFrame();
	static void EndFrame();

	static GLuint LoadShaders(std::string vertex_shader_path, std::string fragment_shader_path);
    static GLuint LoadShadowFrameBuffer();
	static unsigned int GetShaderProgramID() { return sShaderProgramID[sCurrentShader]; }
	static unsigned int GetCurrentShader() { return sCurrentShader; }
    static unsigned int GetFrameBufferID() { return sShaderProgramID[sCurrentShader]; }
    // not sure if needed
    static unsigned int GetFrameBuffer() { return sCurrentShader; }
	static void SetShader(ShaderType type);
    
    static void CheckForErrors();
    static bool PrintError();

private:
	static GLFWwindow* spWindow;

	static std::vector<unsigned int> sShaderProgramID;
    static std::vector<unsigned int> fShaderProgramID;
    static unsigned int CurrentFrameBuffer;
	static unsigned int sCurrentShader;

};

