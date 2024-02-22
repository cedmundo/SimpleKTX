#include "ktxView.h"
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

typedef struct {
  bool didInitGLFW;
  float aspectRatio;
  float deltaTime;
  double curFrame;
  double lstFrame;
  float fpsTime;
  int fpsCount;
  int curFPS;
  GLFWwindow *window;
} App;

static App app = {0};

char *LoadTextFile(const char *name);
void UnloadTextFile(char *);

int Exit(StatusCode status) {
  assert(status >= SUCCESS && status < E_ERROR_COUNT &&
         "invalid arg status: outside range");
  if (status != SUCCESS) {
    assert(fprintf(stderr, "error: %s \n", strerror(errno)) > 0);
    return -1;
  }

  return 0;
}

StatusCode AppInit(int window_width, int window_height,
                   const char *window_title) {
  // Init GLFW.
  if (!glfwInit()) {
    return E_CANNOT_INIT_GLFW;
  }

  // Allow GLFW termination
  app.didInitGLFW = true;

  // Window hints for OpenGL.
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

  // Create window.
  app.window =
      glfwCreateWindow(window_width, window_height, window_title, NULL, NULL);
  if (app.window == NULL) {
    return E_CANNOT_CREATE_WINDOW;
  }

  // Without this gladLoadGL will fail.
  glfwMakeContextCurrent(app.window);
  if (!gladLoadGL()) {
    return E_CANNOT_LOAD_GL;
  }

  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  return SUCCESS;
}

bool AppShouldClose() {
  assert(app.window != NULL && "invalid state: app.window is not initialized");
  return glfwWindowShouldClose(app.window);
}

void AppClose() {
  if (app.window != NULL) {
    glfwDestroyWindow(app.window);
  }

  if (app.didInitGLFW) {
    glfwTerminate();
  }
}

void AppBeginFrame() {
  assert(app.window != NULL && "invalid state: app.window is not initialized");
  int width = 0;
  int height = 0;

  // Framebuffer resize and aspect ratio
  glfwGetFramebufferSize(app.window, &width, &height);
  app.aspectRatio = (float)width / (float)height;

  // Uptime and delta time
  app.curFrame = glfwGetTime();
  app.deltaTime = (float)(app.curFrame - app.lstFrame);
  app.lstFrame = app.curFrame;

  // FPS Counting
  app.fpsTime += app.deltaTime;
  app.fpsCount += 1;
  if (app.fpsTime >= 1.0f) {
    app.curFPS = app.fpsCount;
    app.fpsCount = 0;
    app.fpsTime = 0.0f;
  }

  // Clear buffer and start frame
  glViewport(0, 0, width, height);
  glClear(GL_COLOR_BUFFER_BIT);
}

void AppEndFrame() {
  assert(app.window != NULL && "invalid state: app.window is not initialized");
  glfwPollEvents();
  glfwSwapBuffers(app.window);
}

Shader AppLoadShader(const char *vs_path, const char *fs_path) {
  Shader shader = {0};
  int glStatus = 0;
  char *vs_source = NULL;
  char *fs_source = NULL;
  unsigned vsId = 0;
  unsigned fsId = 0;

  vs_source = LoadTextFile(vs_path);
  if (vs_source == NULL) {
    // TODO(cedmundo): Add log messages
    shader.status = E_CANNOT_LOAD_FILE;
    goto terminate;
  }

  fs_source = LoadTextFile(fs_path);
  if (fs_source == NULL) {
    // TODO(cedmundo): Add log messages
    shader.status = E_CANNOT_LOAD_FILE;
    goto terminate;
  }

  vsId = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vsId, 1, (const char **)&vs_source, NULL);
  glCompileShader(vsId);
  glGetShaderiv(vsId, GL_COMPILE_STATUS, &glStatus);
  if (glStatus != 0) {
    // TODO(cedmundo): Add log messages
    shader.status = E_SHADER_COMPILE_ERROR;
    goto terminate;
  }

  fsId = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fsId, 1, (const char **)&fs_source, NULL);
  glCompileShader(fsId);
  glGetShaderiv(fsId, GL_COMPILE_STATUS, &glStatus);
  if (glStatus != 0) {
    // TODO(cedmundo): Add log messages
    shader.status = E_SHADER_COMPILE_ERROR;
    goto terminate;
  }

  shader.spId = glCreateProgram();
  glAttachShader(shader.spId, vsId);
  glAttachShader(shader.spId, fsId);
  glLinkProgram(shader.spId);
  glGetShaderiv(shader.spId, GL_LINK_STATUS, &glStatus);
  if (glStatus != 0) {
    // TODO(cedmundo): Add log messages
    shader.status = E_SHADER_LINK_ERROR;
    goto terminate;
  }

terminate:
  if (vsId != 0) {
    glDetachShader(shader.spId, vsId);
    glDeleteShader(vsId);
  }

  if (fsId != 0) {
    glDetachShader(shader.spId, fsId);
    glDeleteShader(fsId);
  }

  if (shader.status != SUCCESS) {
    AppDestroyShader(shader);
  }

  if (vs_source != NULL) {
    free(vs_source);
  }

  if (fs_source != NULL) {
    free(fs_source);
  }

  return shader;
}

void AppDestroyShader(Shader shader) {
  if (shader.spId != 0) {
    glDeleteProgram(shader.spId);
  }
}

char *LoadTextFile(const char *name) {
  assert(name != NULL && "invalid arg name: cannot be NULL");
  FILE *file = fopen(name, "re");
  if (file == NULL) {
    return NULL;
  }

  size_t file_size = 0L;
  assert(fseek(file, 0, SEEK_END) == 0);
  file_size = ftell(file);
  assert(fseek(file, 0, SEEK_SET) == 0);

  char *data = calloc(sizeof(char), file_size);
  size_t read_size = fread(data, sizeof(char), file_size, file);
  if (read_size == 0) {
    assert(fclose(file) == 0);
    free(data);
    return NULL;
  }

  return data;
}

void UnloadTextFile(char *data) {
  assert(data != NULL && "invalid arg data: cannot be NULL");
  if (data != NULL) {
    free(data);
  }
}
