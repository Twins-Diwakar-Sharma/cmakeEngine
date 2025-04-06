#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize2.h"

#include <iostream>
#include <string>
#include "Mathril.h"

Mat3 g_sobelX(-1.0, 0.0, 1.0, 
              -2.0, 0.0, 2.0,
              -1.0, 0.0, 1.0);

Mat3 g_sobelZ(-1.0, -2.0, -1.0,
               0.0,  0.0,  0.0,
               1.0,  2.0,  1.0);


Vec3 getPositionClamped(int row, int col, int width, int height, int channels, float* data)
{
  row = row < 0 ? 0 : row;
  row = row >= width ? width-1 : row;
  col = col < 0 ? 0 : col;
  col = col >= height ? height-1 : col;
  
  float res = 1024.0;

  int index = (row * width + col) * channels;  
  float y = data[index*channels];
  y = y*256.0;  
  float x = (res*col/width);
  float z = (res*row/height);
  
  return Vec3(x,y,z);
}

Vec3 simpleNormal(int row, int col, int width, int height, int channels, float* data)
{
  Vec3 top = getPositionClamped(row-1, col, width, height, channels, data);
  Vec3 bot = getPositionClamped(row+1, col, width, height, channels, data);
  Vec3 left = getPositionClamped(row, col-1, width, height, channels, data);
  Vec3 right = getPositionClamped(row, col+1, width, height, channels, data);
  
  float dx = (right[1] - left[1])/2.0;
  float dz = (bot[1] - top[1])/2.0;

  Vec3 cross(-dx, 1.0, -dz);

  float len = (float)cross;
  cross = (1.0f/len) * cross;

  return cross;
}

Vec3 sobelNormal(int row, int col, int width, int height, int channels, float* data)
{
  Vec3 poses[3][3];
  poses[0][0] = getPositionClamped(row-1, col-1, width, height, channels, data);
  poses[0][1] = getPositionClamped(row, col-1, width, height, channels, data);
  poses[0][2] = getPositionClamped(row+1, col-1, width, height, channels, data);
  poses[1][0] = getPositionClamped(row-1, col, width, height, channels, data);
  poses[1][1] = getPositionClamped(row, col, width, height, channels, data);
  poses[1][2] = getPositionClamped(row+1, col, width, height, channels, data);
  poses[2][0] = getPositionClamped(row-1, col+1, width, height, channels, data);
  poses[2][1] = getPositionClamped(row, col+1, width, height, channels, data);
  poses[2][2] = getPositionClamped(row+1, col+1, width, height, channels, data);
  
  float dx=0, dz=0;
  for(int i=0; i<3; i++)
  {
    for(int j=0; j<3; j++)
    {
      dx += g_sobelX[i][j] * poses[i][j][1];
      dz += g_sobelZ[i][j] * poses[i][j][1];
    }
  }

  Vec3 normal(-dx, 1.0, -dz);
  normal = (1.0/(float)(normal)) * normal;


  return normal;
}

int main()
{

  bool debug = true;
  std::string name = "sweden16";
  std::string path = name + ".png";
  
  int width, height, channels = 0;


  float* originalData = stbi_loadf(path.c_str(), &width, &height, &channels, 0);

  std::cout << "loaded original" << std::endl;

  int resizedWidth = width*4, resizedHeight = height*4;
  float* resizedData = stbir_resize_float_linear(originalData, width, height, width*channels*sizeof(float),
                                                 0, resizedWidth, resizedHeight, resizedWidth*channels*sizeof(float),
                                                 STBIR_1CHANNEL);

  std::cout << "resized " << std::endl;

  int normChannels = 3;
  float* normalMap = new float[resizedWidth * resizedHeight * normChannels];
  
  std::cout << "Calculating normals" << std::endl;
  for(int row=0; row<resizedHeight; row++)
  {
    for(int col=0; col<resizedWidth; col++)
    {
      Vec3 norm = sobelNormal(row, col, resizedWidth, resizedHeight, channels, resizedData);
      norm = 0.5f * (norm + Vec3(1,1,1));
      int index = (row * resizedWidth + col)*normChannels;
      int indexOld = (row * resizedWidth + col);
      for(int i=0; i<3; i++)
        normalMap[index+i] = norm[i];
    }
  }
  
  std::string outputName = name + "Normal";
  stbi_write_hdr( (outputName+".png").c_str(), resizedWidth, resizedHeight, normChannels, normalMap);

  std::cout << "normals written" << std::endl;

  if(debug)
  {
    unsigned char* letsLook = new unsigned char[resizedWidth * resizedHeight * normChannels];
    for(int i=0; i<resizedHeight; i++)
    {
      for(int j=0; j<resizedWidth; j++)
      {
        int index = (i*resizedWidth + j)*normChannels;
        for(int k=0; k<3; k++)
        {
          letsLook[index+k] = (unsigned char)( (int) ( 255 * normalMap[index+k] ) );  
        }
      }
    }
    stbi_write_png( (outputName + "_debug.png").c_str(), resizedWidth, resizedHeight, 3, letsLook, resizedWidth*3);
    std::cout << "debug written" << std::endl;
    free(letsLook);
  }
  stbi_image_free(originalData);
  free(resizedData);
  free(normalMap);
}
