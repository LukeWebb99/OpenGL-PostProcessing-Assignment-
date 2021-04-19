#include "Mesh.h"

Mesh::Mesh()
{
	VAO = 0; 
	VBO = 0;
	IBO = 0;
	m_indexCount = 0;
}

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices)
{
	Create(vertices, indices);
}

Mesh::~Mesh()
{
	Clear();
}

void Mesh::Create(GLfloat* vertices, unsigned int* indices, unsigned int vertexCount, unsigned int indicesCount) 
{
	m_indexCount = indicesCount; // store index count for drawing later

	glGenVertexArrays(1, &VAO); // gen vertex array object
	glBindVertexArray(VAO);	    // bind vertex array object
	 
	glGenBuffers(1, &IBO); // gen index buffer object 
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO); // bind index buffer object
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * m_indexCount, &indices[0], GL_STATIC_DRAW); // init buffer data

	glGenBuffers(1, &VBO); // bind vertex buffer object
	glBindBuffer(GL_ARRAY_BUFFER, VBO); // bind vertex buffer object
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertexCount, &vertices[0], GL_STATIC_DRAW); // init buffer data

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertices[0]) * 8, 0); // set attrib pointer vertex data
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertices[0]) * 8, (void*)(sizeof(vertices[0]) * 3)); // set attrib pointer texcoord data
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertices[0]) * 8, (void*)(sizeof(vertices[0]) * 5));  // set attrib pointer normal data
	glEnableVertexAttribArray(2);

	//clean up
	glBindBuffer(GL_ARRAY_BUFFER, 0); // bind buffer 0
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // bind element array buffer 0
	glBindVertexArray(0); // bind vertex array 0

}

void Mesh::Create(std::vector<Vertex> vertices, std::vector<unsigned int> indices)
{
	m_indexCount = indices.size(); // store index count for drawing later

	glGenVertexArrays(1, &VAO); // gen vertex array object
	glBindVertexArray(VAO);	    // bind vertex array object

	glGenBuffers(1, &IBO); // gen index buffer object
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);  // bind index buffer object
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices[0], GL_STATIC_DRAW); // init buffer data

	glGenBuffers(1, &VBO);  // bind vertex buffer object
	glBindBuffer(GL_ARRAY_BUFFER, VBO); // bind vertex buffer object
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW); // init buffer data

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_position)); // set attrib pointer vertex data

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_texcoords)); // set attrib pointer texcoords data

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_normal)); // set attrib pointer normal data

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_tangent)); // set attrib pointer tangent data

	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_bitangent)); // set attrib pointer bitangent data

	//clean up
	glBindBuffer(GL_ARRAY_BUFFER, 0); // bind everything back to 0
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

}

void Mesh::Render() {

	glBindVertexArray(VAO); // bind vertex array
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO); // bind element buffer
	
	glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0); // draw 

}

void Mesh::Clear() {

	// clean and delete current IBO, VBA & VAO
	if (IBO != 0) {
		glDeleteBuffers(1, &IBO);
		IBO = 0;
	}

	if (VBO != 0) {
		glDeleteBuffers(1, &VBO);
		VBO = 0;
	}

	if (VAO != 0) {
		glDeleteVertexArrays(1, &VAO);
		VAO = 0;
	}

	m_indexCount = 0; // set to 0 
}
