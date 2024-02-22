#include <stdio.h>

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "ktxView.h"

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define WINDOW_TITLE "ktxView"

int main() {
  GLFWwindow *window = NULL;
  StatusCode status = SUCCESS;

  // Init GLFW.
  CHECK_ERROR(!glfwInit(), E_CANNOT_INIT_GLFW);

  // Window hints for OpenGL.
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

  // Create window.
  window =
      glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, NULL, NULL);
  CHECK_ERROR(!window, E_CANNOT_CREATE_WINDOW);

  // Without this gladLoadGL will fail.
  glfwMakeContextCurrent(window);
  CHECK_ERROR(!gladLoadGL(), E_CANNOT_LOAD_GL);

  // TODO(cedmundo): Load and compile shaders.
  // TODO(cedmundo): Create and load plane.

  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

  // Main loop
  while (!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

terminate:
  return Exit(status);
}
