#include "data/Texture.h"

#pragma region Public Methods

Texture::Texture(const char* texturePath, std::string name, TextureParam params) : Name(name), ID(0), Path(texturePath)
{
   glGenTextures(1, &ID);

   // load image, create texture and generate mipmaps
   int width, height, nrChannels;

   if (params.flip)
   {
      stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
   }
   // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
   unsigned char* data = stbi_load(texturePath, &width, &height, &nrChannels, 0);
   if (data)
   {
       GLenum format;
       if (nrChannels == 1)
           format = GL_RED;
       else if (nrChannels == 3)
           format = GL_RGB;
       else if (nrChannels == 4)
           format = GL_RGBA;

       glBindTexture(GL_TEXTURE_2D, ID);
       glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
       glGenerateMipmap(GL_TEXTURE_2D);

       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

   }
   else
   {
      std::cout << "Failed to load texture" << std::endl;
   }
}

#pragma endregion
