#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Texture::Texture()
{
}

void Texture::loadDefault(std::string path)
{
  type = TEX_DEF;
	const char* cTarget = path.c_str();
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	stbi_set_flip_vertically_on_load(true);
	unsigned char * data = stbi_load(cTarget, &width, &height, &channels, 0);
	if (data == nullptr)
	{
		std::cerr << "Texture not found : " << path << std::endl;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(data);
}

void Texture::loadHDR(std::string path)
{
  type = TEX_HDR;
	const char* cTarget = path.c_str();
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	stbi_set_flip_vertically_on_load(true);

  //stbi_ldr_to_hdr_gamma(1.0f);
	 float * data = stbi_loadf(cTarget, &width, &height, &channels, 0);

	if (data == nullptr)
	{
		std::cerr << "Texture not found : " << path << std::endl;
	}

  std::cout << "width " << width << " height: " << height << " channels : " << channels << std::endl;

	glTexImage2D(GL_TEXTURE_2D, 0, GL_R16F, width, height, 0, GL_R16F, GL_FLOAT, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(data);
}

Texture::~Texture()
{
	glDeleteTextures(1, &id);
}

unsigned int Texture::getTextureId()
{
	return id;
}

std::string& Texture::getName()
{
	return name;
}


Texture::Texture(std::string name, Tex_Type type)
{
	std::string path = "inventory/textures/" + name + ".png";
	this->name = name;
  switch(type)
  {
    case TEX_DEF:
      loadDefault(path);
      break;
    case TEX_HDR:
      loadHDR(path);
      break;
  }
}

