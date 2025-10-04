#ifndef JALLA_ANIMATED
#define JALLA_ANIMATED

#include "Mesh.h"
#include <fstream>
#include <sstream>
#include <vector>
#include "glad/glad.h"
#include <iostream>

class MeshAnimated : public Mesh
{
protected:
	void generate(std::vector<float>&, std::vector<unsigned int>&); // creates buffers and assigns sizeOfIndices
	
public:
	MeshAnimated();
	MeshAnimated(std::string);
	~MeshAnimated();
	void bind();
	void unbind();
	int size();
};

#endif
