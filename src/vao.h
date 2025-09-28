#pragma once

#include "vbo.h"
#include <glad/glad.h>

class VAO {
  public:
    GLuint ID;
    VAO();

    void LinkVBO(VBO *VBO, GLuint layout);
    void Bind();
    void Unbind();
    void Delete();
};
