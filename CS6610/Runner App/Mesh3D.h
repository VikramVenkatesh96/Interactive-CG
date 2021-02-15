#pragma once

#include "Shader.h"
#include <GL\glew.h>
#include <glm\glm.hpp>
#include <vector>

struct Vertex
{
	glm::vec3 position;
};

class Mesh3D
{
public:
	Mesh3D(std::vector<Vertex>& i_verts, std::vector<unsigned short>& i_indices);
	~Mesh3D();
	void Draw(Shader*& shader);
	inline Transform& GetTransform(){ return transform; }
public:
	unsigned int numVerts;
	unsigned int numTris;
private:
	GLuint VAO;
	//Two buffers - verts and indices
	GLuint VBO[2];
	Transform transform;
};

