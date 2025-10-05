#ifndef JALLA_ANIMATED
#define JALLA_ANIMATED

#include "Mesh.h"
#include <fstream>
#include <sstream>
#include <vector>
#include "glad/glad.h"
#include <iostream>
#include <map>
#include "Mathril.h"

struct Skeleton
{
  std::map<std::string, unsigned int> jointIndex;
  std::vector<std::vector<unsigned int>> jointTree;
};

struct JointData
{
  Vec3 position;
  Quat rotation;
  Vec3 scale;

  JointData();
  JointData(const JointData&);
  JointData(JointData&&);
  ~JointData();
};

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

  Skeleton skeleton;
  std::vector<JointData> invBind;
  std::map<std::string, std::vector<std::vector<JointData>>> animation;
};

#endif
