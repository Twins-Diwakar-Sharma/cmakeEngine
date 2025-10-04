#include "MeshAnimated.h"

void MeshAnimated::generate(std::vector<float>& vertexData, std::vector<unsigned int>& indices)
{
	
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), &vertexData[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 16 * sizeof(float), (void*)0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 16 * sizeof(float), (void*)(3 * sizeof(float)));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 16 * sizeof(float), (void*)(5 * sizeof(float)));
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 16 * sizeof(float), (void*)(8 * sizeof(float)));
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 16 * sizeof(float), (void*)(12 * sizeof(float)));

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
	
	glBindVertexArray(0);

	sizeOfIndices = indices.size();

}

MeshAnimated::MeshAnimated()
{
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);
	glDeleteVertexArrays(1, &vao);
}

MeshAnimated::MeshAnimated(std::string name)
{
	std::string path = "inventory/models/" + name + ".dyn";
	std::ifstream ifs(path, std::ios::binary | std::ios::in);
  unsigned int dataSize = 0;
  ifs.read((char*) &dataSize, sizeof(unsigned int));
  std::vector<float> vertexData(dataSize);
  ifs.read( (char*)(vertexData.data()), dataSize * sizeof(float));
  ifs.read((char*) &dataSize, sizeof(unsigned int));
	std::vector<unsigned int> indices(dataSize);
  ifs.read((char*)(indices.data()), dataSize * sizeof(unsigned int));
  ifs.close();

  generate(vertexData, indices);
}

MeshAnimated::~MeshAnimated()
{
}


void MeshAnimated::bind()
{
	glBindVertexArray(vao);
}

void MeshAnimated::unbind()
{
	glBindVertexArray(0);
}

int MeshAnimated::size()
{
	return sizeOfIndices;
}
