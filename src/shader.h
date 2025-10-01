#pragma once

#include <cerrno>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>

std::string get_file_contents(const char *filename);

class Shader {
  public:
    GLuint ID;
    Shader(const char *vertexFile, const char *fragmentFile);

    void Activate();
    void Delete();
    // Uniform helpers
    void setInt(const char *name, int v) const;
    void setFloat(const char *name, float v) const;
    void setVec2(const char *name, const glm::vec2 &v) const;
    void setMat4(const char *name, const glm::mat4 &m) const;
};
