#pragma once
#include "Camera.h"
#include "Transform.h"
#include <GL/glew.h>

class Shader
{
public:
	Shader(const char* vshaderName, const char* fshaderName);
	~Shader();
	void Update(const Transform& transform, const Camera& camera);
	void Bind();
	void Hot_Reload(const char* vshaderName, const char* fshaderName);
private:
	void CreateShaderProgram(GLuint& newProgramID, GLuint* newShaders,const char* vshaderName, const char* fshaderName);
	void RemoveShaderProgram();
private:
	GLuint programID;
	//Vertex and Fragment Shaders
	GLuint shaders[2];
	//Transform and view matrices
	GLuint uniform[2];
};

