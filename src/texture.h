#pragma once

#include "stb_image.h"
#include <glad/glad.h>

#include "shader.h"

class Texture {
  public:
    GLuint ID;
    GLenum type;
    int width;
    int height;
    Texture(const char *image, GLenum texType, GLenum slot, GLenum format,
            GLenum pixelType);

    void texUnit(Shader *shader, const char *uniform, GLuint unit);
    void Bind();
    void Unbind();
    void Delete();
};
