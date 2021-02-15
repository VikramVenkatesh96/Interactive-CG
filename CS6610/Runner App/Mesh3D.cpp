#include "Mesh3D.h"

Mesh3D::Mesh3D(std::vector<Vertex>& i_verts, std::vector<unsigned short>& i_indices):
	numVerts(i_verts.size()),
	numTris(i_indices.size()/3)
{
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(2, VBO);
	glBindBuffer(GL_ARRAY_BUFFER ,VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, i_verts.size() * sizeof(i_verts[0]), &i_verts[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, i_indices.size() * sizeof(i_indices[0]), &i_indices[0], GL_STATIC_DRAW);
	
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

 	glBindVertexArray(0);
}

Mesh3D::~Mesh3D()
{
	glDeleteVertexArrays(1, &VAO);
}

void Mesh3D::Draw(Shader*& shader)
{
	shader->Bind();
	glBindVertexArray(VAO);
	glDrawElements(GL_POINTS,numTris * 3, GL_UNSIGNED_SHORT,0);
	glBindVertexArray(0);
}
