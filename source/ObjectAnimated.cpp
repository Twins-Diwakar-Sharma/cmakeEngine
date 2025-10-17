#include "ObjectAnimated.h"

void ObjectAnimated::syncTransform()
{

	float conv = M_PI / 180.0f;
	float radx = rot[0]*conv , rady = rot[1]*conv , radz = rot[2]*conv ;
	float cx = std::cos(radx), sx = std::sin(radx);
	float cy = std::cos(rady), sy = std::sin(rady);
	float cz = std::cos(radz), sz = std::sin(radz);

	transform[0][0] = (cy * cz) * sca[0];							transform[0][1] = (-sz * cy) * sca[1];								transform[0][2] = sy * sca[2];					transform[0][3] = pos[0];
	transform[1][0] = (cx * sz + cz * sy * sx) * sca[0];			transform[1][1] = (cz * cx - sx * sz * sy) * sca[1];				transform[1][2] = -cy * sx * sca[2];			transform[1][3] = pos[1];
	transform[2][0] = (sz * sx - cx * cz * sy) * sca[0];			transform[2][1] = (cz * sx + sz * sy * cx) * sca[1];				transform[2][2] = cx * cy * sca[2];				transform[2][3] = pos[2];
	transform[3][0] = 0;											transform[3][1] = 0;												transform[3][2] = 0;							transform[3][3] = 1;

}

ObjectAnimated::ObjectAnimated(MeshAnimated& mesh, Texture& tex) : mesh(mesh), tex(tex), currentFrameBones(mesh.skeleton.jointTree.size())
{
	for (int i = 0; i < 3; i++)
	{
		pos[i] = 0.0f;
		rot[i] = 0.0f;
		sca[i] = 1.0f;
	}
	syncTransform();
}

ObjectAnimated::~ObjectAnimated()
{
}

ObjectAnimated::ObjectAnimated(ObjectAnimated& g) : mesh(g.mesh), tex(g.tex), currentFrameBones(mesh.skeleton.jointTree.size())
{
	pos = g.pos;
	rot = g.rot;
	sca = g.sca;
	transform = g.transform;
}

ObjectAnimated::ObjectAnimated(ObjectAnimated&& g) : mesh(g.mesh), tex(g.tex), currentFrameBones(mesh.skeleton.jointTree.size())
{
	pos = std::move(g.pos);
	rot = std::move(g.rot);
	sca = std::move(g.sca);
	transform = std::move(g.transform);
}

void ObjectAnimated::bind()
{
	mesh.bind();
}

void ObjectAnimated::unbind()
{
	mesh.unbind();
}

Texture& ObjectAnimated::getTexture()
{
	return tex;
}

MeshAnimated& ObjectAnimated::getMesh()
{
	return mesh;
}

int ObjectAnimated::size()
{
	return mesh.size();
}

unsigned int ObjectAnimated::getTextureId()
{
	return tex.getTextureId();
}

void ObjectAnimated::translate(float dx, float dy, float dz)
{
	pos[0] += dx;	pos[1] += dy;	pos[2] += dz;
	for (int i = 0; i < 3; i++)
		transform[i][3] = pos[i];
}

void ObjectAnimated::rotate(float dx, float dy, float dz)
{
	rot[0] += dx;	rot[1] += dy;	rot[2] += dz;
	for (int i = 0; i < 3; i++)
	{
		if (rot[i] > 360.0f)
			rot[i] -= 360.0f;
	}
	syncTransform();
}

void ObjectAnimated::scale(float dx, float dy, float dz)
{
	sca[0] += dx;	sca[1] += dy;	sca[2] += dz;
	syncTransform();
}

void ObjectAnimated::setPosition(float x, float y, float z)
{
	pos[0] = x;	 pos[1] = y;	pos[2] = z;
	for (int i = 0; i < 3; i++)
		transform[i][3] = pos[i];
}

void ObjectAnimated::setRotation(float x, float y, float z)
{
	rot[0] = x;	rot[1] = y;	rot[2] = z;
	syncTransform();
}

void ObjectAnimated::setScale(float x, float y, float z)
{
	sca[0] = x;	sca[1] = y;	 sca[2] = z;
	syncTransform();
}

Mat4& ObjectAnimated::getTransform()
{
	return transform;
}

void ObjectAnimated::setAnimation(std::string animName)
{
  currentAnimName = animName; 
  currentAnimName = "Idle";
}

void ObjectAnimated::animate()
{
  std::vector<std::vector<JointData>>& animData = mesh.animation[currentAnimName];  

  setGlobalTransforms(-1, 0, animData, currentFrameIndex);
  animTime += animTimeDelta;
  if(animTime >= 1.0)
  {
    animTime = 0.0;
    currentFrameIndex = (currentFrameIndex + 1) % (animData.size());
  }
  for(int jointIndex = 0; jointIndex < currentFrameBones.size(); jointIndex++)
  {
    currentFrameBones[jointIndex].translation = currentFrameBones[jointIndex].translation + mesh.invBind[jointIndex].translation;
    currentFrameBones[jointIndex].rotation = currentFrameBones[jointIndex].rotation * mesh.invBind[jointIndex].rotation;
    for(int comp=0; comp<3; comp++)
    {
      currentFrameBones[jointIndex].scale[comp] = currentFrameBones[jointIndex].scale[comp] * mesh.invBind[jointIndex].scale[comp];
    }

  }
}

void ObjectAnimated::setGlobalTransforms(int parentIndex, int childIndex, std::vector<std::vector<JointData>>& animData, unsigned int frameIndex)
{

  int nextFrameIndex = (frameIndex + 1) % (animData.size());
  for(int comp=0; comp<3; comp++) 
    currentFrameBones[childIndex].scale[comp] = (1.0 - animTime)*animData[frameIndex][childIndex].scale[comp] + animTime*animData[nextFrameIndex][childIndex].scale[comp];
  currentFrameBones[childIndex].translation = (1.0 - animTime)*animData[frameIndex][childIndex].translation + animTime*animData[nextFrameIndex][childIndex].translation;
  currentFrameBones[childIndex].rotation = slerp(animData[frameIndex][childIndex].rotation, animData[nextFrameIndex][childIndex].rotation, animTime);

  if(parentIndex >= 0)
  {
    Quat relativePosQuat(0.0f, 
                         currentFrameBones[childIndex].translation[0],
                         currentFrameBones[childIndex].translation[1],
                         currentFrameBones[childIndex].translation[2]);
    relativePosQuat =  currentFrameBones[parentIndex].rotation * relativePosQuat * (~currentFrameBones[parentIndex].rotation);
    currentFrameBones[childIndex].translation = currentFrameBones[parentIndex].translation + Vec3(relativePosQuat[1], relativePosQuat[2], relativePosQuat[3]); 
    //for(int comp=0; comp<3; comp++)  // scale should not depend on parent
    //  currentFrameBones[childIndex].scale[comp] = currentFrameBones[parentIndex].scale[comp] * currentFrameBones[childIndex].scale[comp]; 
    currentFrameBones[childIndex].rotation = currentFrameBones[parentIndex].rotation * currentFrameBones[childIndex].rotation; 
  }

  for(int grandChildIndex=0; grandChildIndex < mesh.skeleton.jointTree[childIndex].size(); grandChildIndex++)
  {
    int trueGrandChildIndex = (int)(mesh.skeleton.jointTree[childIndex][grandChildIndex]);
    setGlobalTransforms(childIndex, trueGrandChildIndex, animData, frameIndex);
  }
}

