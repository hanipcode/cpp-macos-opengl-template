// clang-format off
#include "texture.h"
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE 
#include <GLFW/glfw3.h>
// clang-format on
#include "ebo.h"
#include "shader.h"
#include "stb_image.h"
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

struct Rect {
    float x, y, w, h;
}; // in window coords (origin top-left)

Rect computeContentRect(int winW, int winH, int imgW, int imgH) {
  float winAR = float(winW) / float(winH);
  float imgAR = float(imgW) / float(imgH);
  Rect r;
  if (winAR > imgAR) {
    // Pillarbox: full height, centered horizontally
    r.h = float(winH);
    r.w = r.h * imgAR;
    r.x = 0.5f * (winW - r.w);
    r.y = 0.0f;
  } else {
    // Letterbox: full width, centered vertically
    r.w = float(winW);
    r.h = r.w / imgAR;
    r.x = 0.0f;
    r.y = 0.5f * (winH - r.h);
  }
  return r;
}

struct MouseUV {
    bool inside;
    glm::vec2 uv; // (0..1, 0..1) bottom-left origin (TexCoord space)
};

MouseUV mouseToUV(GLFWwindow *window, const Rect &r) {
  // Mouse in window coords (origin top-left)
  double mx, my;
  glfwGetCursorPos(window, &mx, &my);

  MouseUV out{};
  out.inside = (mx >= r.x && mx <= r.x + r.w && my >= r.y && my <= r.y + r.h);
  if (!out.inside) {
    out.uv = glm::vec2(0.0f); // unused
    return out;
  }

  // Normalize to [0..1] within the content rect; flip Y to TexCoord convention
  float u = float((mx - r.x) / r.w);
  float v = 1.0f - float((my - r.y) / r.h);
  out.uv = glm::vec2(u, v);
  return out;
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
      // COORDINATES       / COLORS         /TEXTURE
      -1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // lower left
      -1.0f, 1.0f,  0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // upper left
      1.0f,  1.0f,  0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // upper right
      1.0f,  -1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, // lower right
  };

  unsigned int indices[] = {0, 2, 1, 0, 3, 2};

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
  VAO1.LinkAttrib(&VBO1, 0, 3, GL_FLOAT, 8 * sizeof(float), (void *)0);
  VAO1.LinkAttrib(&VBO1, 1, 3, GL_FLOAT, 8 * sizeof(float),
                  (void *)(3 * sizeof(float)));

  VAO1.LinkAttrib(&VBO1, 2, 3, GL_FLOAT, 8 * sizeof(float),
                  (void *)(6 * sizeof(float)));

  EBO EBO1(indices, sizeof(indices));

  VAO1.Unbind();
  VBO1.Unbind();
  EBO1.Unbind();

  Texture popCat("assets/textures/pop_cat.png", GL_TEXTURE_2D, GL_TEXTURE0,
                 GL_RGBA, GL_UNSIGNED_BYTE);
  popCat.texUnit(&shaderProgram, "tex0", 0);

  glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

  while (!glfwWindowShouldClose(window)) {
    glClearColor(0.1f, 0.12f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    int winW, winH;
    glfwGetWindowSize(window, &winW, &winH);
    // int imgW = fbW, imgH = fbH; // or your source image size

    Rect content = computeContentRect(winW, winH, fbW, fbH);
    MouseUV m = mouseToUV(window, content);

    popCat.Bind();
    shaderProgram.Activate();
    shaderProgram.setFloat("scale", 1.0f);
    shaderProgram.setVec2("magnifyCenter", m.uv); // see next section
    shaderProgram.setFloat("magnifyRadius", 0.18f);
    shaderProgram.setFloat("magnifyZoom", 2.0f);

    VAO1.Bind();
    const GLsizei idxCount = sizeof(indices) / sizeof(indices[0]);

    glDrawElements(GL_TRIANGLES, idxCount, GL_UNSIGNED_INT, 0);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  VAO1.Delete();
  VBO1.Delete();
  EBO1.Delete();
  shaderProgram.Delete();
  popCat.Delete();

  glfwDestroyWindow(window);
  glfwTerminate();
} catch (const std::exception &e) {
  std::cerr << "Error: " << e.what() << "\n";
  return 1;
}
