#include "image_loader.h"

#include <iostream>
#include <string>

#include "stb_image.h"

bool LoadTextureFromFile(const char* filename, GLuint* out_texture,
                         int* out_width, int* out_height) {
  int image_width = 0;
  int image_height = 0;
  int channels = 0;

  std::cout << "Loading image: " << filename << std::endl;

  unsigned char* image_data =
      stbi_load(filename, &image_width, &image_height, &channels, 4);
  if (image_data == NULL) {
    std::cout << "Failed to load image: " << stbi_failure_reason() << std::endl;
    return false;
  }

  GLuint image_texture;
  glGenTextures(1, &image_texture);
  glBindTexture(GL_TEXTURE_2D, image_texture);

  // Setup filtering parameters for display
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  // Upload pixels into texture
#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
  glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, image_data);
  stbi_image_free(image_data);

  *out_texture = image_texture;
  *out_width = image_width;
  *out_height = image_height;

  std::cout << "Image loaded successfully: " << image_width << "x"
            << image_height << std::endl;

  return true;
}
