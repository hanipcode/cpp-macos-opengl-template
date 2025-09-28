#pragma once

#include <cerrno>
#include <glad/glad.h>
#include <string>

std::string get_file_contents(const char *filename);

class Shader {
  public:
    GLuint ID;
    Shader(const char *vertexFile, const char *fragmentFile);

    void Activate();
    void Delete();
};
