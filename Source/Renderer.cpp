//
// COMP 371 Assignment Framework
//
// Created by Nicolas Bergeron on 8/7/14.
// Updated by Gary Chang on 14/1/15
//
// Copyright (c) 2014-2019 Concordia University. All rights reserved.
//

#include "Renderer.h"


#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
using namespace std;

#include <stdlib.h>
#include <string.h>

#include "Renderer.h"
#include "EventManager.h"

#include <GLFW/glfw3.h>


#if defined(PLATFORM_OSX)
#define fscanf_s fscanf
#endif


std::vector<unsigned int> Renderer::sShaderProgramID;
unsigned int Renderer::sCurrentShader;

GLFWwindow* Renderer::spWindow = nullptr;


void Renderer::Initialize()
{
	spWindow = EventManager::GetWindow();
	glfwMakeContextCurrent(spWindow);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		
		getchar();
		exit(-1);
	}
    
    // Somehow, glewInit triggers a glInvalidEnum... Let's ignore it
    glGetError();
    
	// Black background
	glClearColor(0.678f, 0.549f, 1.0f, 0.0f);
	
	// Enable depth test
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    
    
    CheckForErrors();
    

    
    
	// Loading Shaders
#if defined(PLATFORM_OSX)
    std::string shaderPathPrefix = "Shaders/";
#else
    std::string shaderPathPrefix = "../Assets/Shaders/";
#endif

	sShaderProgramID.push_back(
                LoadShaders(shaderPathPrefix + "SolidColor.vertexshader",
                            shaderPathPrefix + "SolidColor.fragmentshader")
                               );
	sShaderProgramID.push_back(
                LoadShaders(shaderPathPrefix + "PathLines.vertexshader",
                            shaderPathPrefix + "PathLines.fragmentshader")
                               );
	sShaderProgramID.push_back(
                LoadShaders(shaderPathPrefix + "SolidColor.vertexshader",
                            shaderPathPrefix + "BlueColor.fragmentshader")
                               );
    
    sShaderProgramID.push_back(
                               LoadShaders(shaderPathPrefix + "Texture.vertexshader",
                                           shaderPathPrefix + "Texture.fragmentshader")
                               );
    sShaderProgramID.push_back(LoadShaders(shaderPathPrefix + "Sky.vertexshader",
                                           shaderPathPrefix + "Sky.fragmentshader"));
	sCurrentShader = 0;

}

void Renderer::Shutdown()
{
		// Shaders
	for (vector<unsigned int>::iterator it = sShaderProgramID.begin(); it < sShaderProgramID.end(); ++it)
	{
		glDeleteProgram(*it);
	}
	sShaderProgramID.clear();


	// Managed by EventManager
	spWindow = nullptr;
}

void Renderer::BeginFrame()
{
	// Clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

}

void Renderer::EndFrame()
{
	// Swap buffers
	glfwSwapBuffers(spWindow);
    
    CheckForErrors();
}

void Renderer::SetShader(ShaderType type)
{
	if (type < (int) sShaderProgramID.size())
	{
		sCurrentShader = type;
	}
}

//
// The following code is taken from
// www.opengl-tutorial.org
//
GLuint Renderer::LoadShaders(std::string vertex_shader_path,std::string fragment_shader_path)
{
	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_shader_path, std::ios::in);
	if(VertexShaderStream.is_open()){
		std::string Line = "";
		while(getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}else{
		printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_shader_path.c_str());
		getchar();
		exit(-1);
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_shader_path, std::ios::in);
	if(FragmentShaderStream.is_open()){
		std::string Line = "";
		while(getline(FragmentShaderStream, Line))
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_shader_path.c_str());
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, nullptr);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, nullptr, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}

	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_shader_path.c_str());
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, nullptr);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, nullptr, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}

	// Link the program
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> ProgramErrorMessage(InfoLogLength+1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, nullptr, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}

// note: when using frame Buffer we need to set it back to 0 (the default one that draws to the screen!)
GLuint Renderer::LoadShadowFrameBuffer(){


    // Just Copy Pasta
    // The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
    GLuint FramebufferName = 0;
    glGenFramebuffers(1, &FramebufferName);
    glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);

    // Depth texture. Slower than a depth buffer, but you can sample it later in your shader
    GLuint depthTexture;
    glGenTextures(1, &depthTexture);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0,GL_DEPTH_COMPONENT16, 1024, 1024, 0,GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture, 0);

    // No color output in the bound framebuffer, only depth.
    glDrawBuffer(GL_NONE);

    // Always check that our framebuffer is ok
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        return false;

    return FramebufferName;

}

bool Renderer::PrintError()
{
    static bool checkForErrors = true;
    
    //
    if( !checkForErrors )
    {
        return false;
    }
    
    //
    const char * errorString = NULL;
    bool retVal = false;
    
    switch( glGetError() )
    {
        case GL_NO_ERROR:
            retVal = true;
            break;
            
        case GL_INVALID_ENUM:
            errorString = "GL_INVALID_ENUM";
            break;
            
        case GL_INVALID_VALUE:
            errorString = "GL_INVALID_VALUE";
            break;
            
        case GL_INVALID_OPERATION:
            errorString = "GL_INVALID_OPERATION";
            break;
            
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            errorString = "GL_INVALID_FRAMEBUFFER_OPERATION";
            break;
            
        case GL_OUT_OF_MEMORY:
            errorString = "GL_OUT_OF_MEMORY";
            break;
            
        default:
            errorString = "UNKNOWN";
            break;
    }
    
    //
    if( !retVal )
    {
        printf( "%s\n", errorString );
    }
    
    //
    return retVal;
}


void Renderer::CheckForErrors()
{
    while(PrintError() == false)
    {
    }
}

