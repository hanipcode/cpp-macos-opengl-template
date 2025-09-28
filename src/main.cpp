// clang-format off
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE 
#include <GLFW/glfw3.h>
// clang-format on
#include "ebo.h"
#include "shader.h"
#include "vao.h"
#include "vbo.h"
#include <iostream>

static void framebufferSizeCallback(GLFWwindow *window, int width, int height) {
  (void)window;
  glViewport(0, 0, width, height);
}

static void glfw_error(int code, const char *desc) {
  std::cerr << "GLFW error " << code << ": " << desc << "\n";
}

int main() try {
  glfwSetErrorCallback(glfw_error);
  if (!glfwInit()) {
    std::cerr << "Failed to init GLFW \n";
    return -1;
  }

  glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  GLfloat vertices[] = {
      0.0f,   0.0f,  0.0f, // 0 center mid
      0.0f,   -0.7f, 0.0f, // 1  bottom center
      -0.25f, -0.7f, 0.0f, // 2  left bottom
      -0.25f, 0.0f,  0.0f, // 3 lefft mid
      0.0f,   0.7f,  0.0f, // 4 center top
      -0.25f, 0.7f,  0.0f, // 5 left top
      0.5f,   0.7f,  0.0f, // 6 horizontal right top
      0.5f,   0.5f,  0.0f, // 7 horizontal right bottom
      0.0f,   0.5f,  0.0f, // 8 horizontal center bottom
      0.5f,   0.1f,  0.0f, // 9 horizontal2 right top
      0.5f,   -0.1f, 0.0f, // 10 horizontal2 right bottom
      0.0f,   -0.1f, 0.0f, // 11 horizontal 2 center bottom
      0.0f,   0.1f,  0.0f, // 12 horizontal2 center top
  };

  unsigned int indices[] = {
      0,  1,  2, // 1
      0,  4,  5, // 3
      4,  6,  7, // 5
      12, 11, 10 // 8
  };

  unsigned int indices2[] = {
      0,  2, 3,  // 2
      0,  5, 3,  // 4
      4,  7, 8,  // 6
      12, 9, 10, // 7
  };

  GLFWwindow *window = glfwCreateWindow(800, 800, "MyOpenGL", nullptr, nullptr);
  if (!window) {
    std::cerr << "Failed to create GLFW window \n";
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cerr << "Failed to initialize GLAD\n";
    return -1;
  }

  int fbW, fbH;
  glfwGetFramebufferSize(window, &fbW, &fbH);
  glViewport(0, 0, fbW, fbH);

  Shader shaderProgram("assets/shader/default.vert",
                       "assets/shader/default.frag");

  Shader shaderProgram2("assets/shader/default.vert",
                        "assets/shader/default2.frag");

  VAO VAO1;
  VAO1.Bind();

  VBO VBO1(vertices, sizeof(vertices));
  VAO1.LinkVBO(&VBO1, 0);

  EBO EBO1(indices, sizeof(indices));

  VAO1.Unbind();
  VBO1.Unbind();
  EBO1.Unbind();

  VAO VAO2;
  VAO2.Bind();
  VAO2.LinkVBO(&VBO1, 0);

  EBO EBO2(indices2, sizeof(indices2));

  VAO2.Unbind();
  VBO1.Unbind();
  EBO2.Unbind();

  glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

  while (!glfwWindowShouldClose(window)) {
    glClearColor(0.1f, 0.12f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    shaderProgram.Activate();

    VAO1.Bind();
    const GLsizei idxCount = sizeof(indices) / sizeof(indices[0]);

    glDrawElements(GL_TRIANGLES, idxCount, GL_UNSIGNED_INT, 0);

    shaderProgram2.Activate();

    const GLsizei idxCount2 = sizeof(indices2) / sizeof(indices2[0]);
    VAO2.Bind();
    glDrawElements(GL_TRIANGLES, idxCount2, GL_UNSIGNED_INT, 0);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  VAO1.Delete();
  VAO2.Delete();
  VBO1.Delete();
  EBO1.Delete();
  EBO2.Delete();
  shaderProgram.Delete();

  glfwDestroyWindow(window);
  glfwTerminate();
} catch (const std::exception &e) {
  std::cerr << "Error: " << e.what() << "\n";
  return 1;
}
