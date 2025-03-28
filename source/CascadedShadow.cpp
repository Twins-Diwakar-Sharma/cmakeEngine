#include "CascadedShadow.h"

CascadedShadow::CascadedShadow(): prevCamPos(0,0,0), prevCamSpin(1,0,0,0)
{
  int base = 5;
  int power = base;
  partitions[0] = 1;
  for(int i=1; i<n_cascades+1; i++)
  {
    partitions[i] = power;
    power = power * base;
  }

}

CascadedShadow::~CascadedShadow()
{}

void CascadedShadow::update(Camera& cam, DirectionalLight& sun)
{

  if(firstUpdate)
  {
    partitions[0] = proj::near;
    partitions[n_cascades] = proj::far;
    firstUpdate = false;
  }

  if( cam.position[0] != prevCamPos[0] || cam.position[1] != prevCamPos[1] || cam.position[2] != prevCamPos[2] || 
      cam.spin[0] != prevCamSpin[0] ||  cam.spin[1] != prevCamSpin[1] ||  cam.spin[2] != prevCamSpin[2] ||  cam.spin[3] != prevCamSpin[3])
  {

    sunCam.spin = sun.getSpin();
    prevCamPos = cam.position;
    prevCamSpin = cam.spin;

    createCascades(cam, sun);
  }
}

void CascadedShadow::createCascades(Camera& cam, DirectionalLight& sun)
{
  // start with near corner, interpolate to make others
  Vec3 origin(0,0,0); 
  float oneByA = 1.0/proj::aspectRatio;
  Vec3 corners[4] = { 
        { -oneByA, 1, -proj::near},  {oneByA, 1, -proj::near},
        { -oneByA, -1, -proj::near},  {oneByA, -1, -proj::near},
  };

  cornerObjectToWorld(origin, cam);
  for(int i=0; i<4; i++)
    cornerObjectToWorld(corners[i], cam);  

  for(int i=0; i<n_cascades; i++)
  {
    setOrthoAndSunPos(i, corners, origin, sun);
  }
}


Mat4* CascadedShadow::getCascOrthos()
{
  return cascOrtho;
}

// updates in place
void CascadedShadow::cornerObjectToWorld(Vec3& corner, Camera& cam)
{
  // rotate first
  Quat posQuat(0, corner[0], corner[1], corner[2]);
  posQuat = (cam.spin) * (posQuat) * (~cam.spin);
    
  // then translate
  corner = cam.position + Vec3(posQuat[1], posQuat[2], posQuat[3]);
}

// updates in place
void CascadedShadow::setOrthoAndSunPos(int index, Vec3 corners[4], Vec3& origin, DirectionalLight& sun)
{
  float lenCorners[4];
  Vec3 unitVectors[4];
  for(int i=0; i<4; i++)
  {
    lenCorners[i] = (float)(corners[i] - origin);
    unitVectors[i] = (1.0f/lenCorners[i]) * (corners[i] - origin);
  }
  Vec3 frustomCorners[8];
  for(int i=0; i<4; i++)
  {
    frustomCorners[i] = (partitions[index]/proj::near) * (corners[i] - origin)   + origin;
    frustomCorners[i+4] = (partitions[index+1]/proj::near) * (corners[i] - origin)   + origin;
  }

  float minX = frustomCorners[0][0], maxX = minX;
  float minY = frustomCorners[0][1], maxY = minY;
  float minZ = frustomCorners[0][2], maxZ = minZ;

  for(int i=0; i<8; i++)
  {
    minX = std::min(minX, frustomCorners[i][0]);
    maxX = std::max(maxX, frustomCorners[i][0]);
    minY = std::min(minY, frustomCorners[i][1]);
    maxY = std::max(maxY, frustomCorners[i][1]);
    minZ = std::min(minZ, frustomCorners[i][2]);
    maxZ = std::max(maxZ, frustomCorners[i][2]);
  }
  
  Vec3 center( (maxX + minX)/2.0, (maxY + minY)/2.0, (maxZ + minZ)/2.0 );

  cascOrtho[index][0][0] = 2.0f/(maxX - minX);
  cascOrtho[index][1][1] = 2.0f/(maxY - minY);
  cascOrtho[index][2][2] = -2.0f/(maxZ - minZ);
  cascOrtho[index][3][3] = 1; 
  cascOrtho[index][0][3] = -(maxX + minX)/(maxX - minX);
  cascOrtho[index][1][3] = -(maxY + minY)/(maxY - minY);
  cascOrtho[index][2][3] = -(maxZ + minZ)/(maxZ - minZ);

  float distance = (proj::far - proj::near);
  distance = 5;
  sunPos[index] = center + distance*(-1 * sun.getDirection());
}

Camera& CascadedShadow::getSunCam(int cascadeIndex)
{
  sunCam.position = sunPos[cascadeIndex];
  return sunCam;
}


Mat4& CascadedShadow::getCascOrtho(int cascIndex)
{
  return cascOrtho[cascIndex];
}

void CascadedShadow::createShadowFBOs(unsigned int width, unsigned int height)
{
  for(int i=0; i<n_cascades; i++)
  {
    shadowFBO[i].create(width, height);
  }
}
