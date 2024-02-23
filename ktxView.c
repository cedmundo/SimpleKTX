#include "ktxView.h"
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <ktx.h>

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

char *LoadFileContents(const char *name);
void UnloadFileContents(char *);

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

Shader AppLoadShader(const char *vsPath, const char *fsPath) {
  Shader shader = {0};
  int glStatus = 0;
  char *vsSource = NULL;
  char *fsSource = NULL;
  unsigned vsId = 0;
  unsigned fsId = 0;
  char shaderLog[512] = {0};

  vsSource = LoadFileContents(vsPath);
  if (vsSource == NULL) {
    // TODO(cedmundo): Add log messages
    shader.status = E_CANNOT_LOAD_FILE;
    goto terminate;
  }

  fsSource = LoadFileContents(fsPath);
  if (fsSource == NULL) {
    // TODO(cedmundo): Add log messages
    shader.status = E_CANNOT_LOAD_FILE;
    goto terminate;
  }

  glStatus = 0;
  vsId = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vsId, 1, (const char **)&vsSource, NULL);
  glCompileShader(vsId);
  glGetShaderiv(vsId, GL_COMPILE_STATUS, &glStatus);
  if (!glStatus) {
    // TODO(cedmundo): Add log messages
    shader.status = E_SHADER_COMPILE_ERROR;
    glGetShaderInfoLog(vsId, 512, NULL, shaderLog);
    fprintf(stderr, "cannot compile vertex shader: %s\n", shaderLog);
    goto terminate;
  }

  glStatus = 0;
  fsId = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fsId, 1, (const char **)&fsSource, NULL);
  glCompileShader(fsId);
  glGetShaderiv(fsId, GL_COMPILE_STATUS, &glStatus);
  if (!glStatus) {
    // TODO(cedmundo): Add log messages
    shader.status = E_SHADER_COMPILE_ERROR;
    glGetShaderInfoLog(fsId, 512, NULL, shaderLog);
    fprintf(stderr, "cannot compile fragment shader: %s\n", shaderLog);
    goto terminate;
  }

  shader.spId = glCreateProgram();
  glAttachShader(shader.spId, vsId);
  glAttachShader(shader.spId, fsId);
  glLinkProgram(shader.spId);
  glGetShaderiv(shader.spId, GL_LINK_STATUS, &glStatus);
  if (!glStatus) {
    // TODO(cedmundo): Add log messages
    shader.status = E_SHADER_LINK_ERROR;
    glGetShaderInfoLog(shader.spId, 512, NULL, shaderLog);
    fprintf(stderr, "cannot link shader program: %s\n", shaderLog);
    goto terminate;
  }
  shader.status = SUCCESS;

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

  if (vsSource != NULL) {
    free(vsSource);
  }

  if (fsSource != NULL) {
    free(fsSource);
  }

  return shader;
}

void AppDestroyShader(Shader shader) {
  if (shader.spId != 0) {
    glDeleteProgram(shader.spId);
  }
}

Texture AppLoadTexture(const char *texPath) {
  Texture texture = {0};
  KTX_error_code result = KTX_SUCCESS;

  result = ktxTexture_CreateFromNamedFile(
      texPath, KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, &texture.src);
  if (result != KTX_SUCCESS) {
    // TODO(cedmundo): Handle KTX_* errors
    // TODO(cedmundo): Report log error
    texture.status = E_CANNOT_CREATE_TEXTURE;
    return texture;
  }

  // upload to GPU
  GLenum glError = 0;

  glGenTextures(1, &texture.id);
  result =
      ktxTexture_GLUpload(texture.src, &texture.id, &texture.format, &glError);
  if (result != KTX_SUCCESS) {
    // TODO(cedmundo): Handle KTX_* errors and glError
    // TODO(cedmundo): Report log error
    texture.status = E_CANNOT_UPLOAD_TEXTURE;
    return texture;
  }

  return texture;
}

void AppDestroyTexture(Texture texture) {
  if (texture.id != 0) {
    glDeleteTextures(1, &texture.id);
  }

  if (texture.src != NULL) {
    ktxTexture_Destroy(texture.src);
  }
}

Model AppMakePlane(float dim) {
  Model model = {0};

  // a plane
  float vertices[] = {
      // POSITIONS      COLORS            UVS
      dim,  dim,  0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, // top right
      dim,  -dim, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, // bottom right
      -dim, -dim, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, // bottom left
      -dim, dim,  0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, // top left
  };
  unsigned indices[] = {
      0, 1, 3, // first triangle
      1, 2, 3, // second triangle
  };

  // upload model
  glGenVertexArrays(1, &model.vao);
  glGenBuffers(1, &model.vbo);
  glGenBuffers(1, &model.ebo);

  glBindVertexArray(model.vao);
  glBindBuffer(GL_ARRAY_BUFFER, model.vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model.ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  // position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  // color attribute
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  // texture coord attribute
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
  return model;
}

void AppRenderModel(Model model) {
  assert(model.vao != 0 && "invalid arg model.vao: uninitialized vertex array");
  assert(model.vbo != 0 && "invalid arg model.vbo: uninitialized array buffer");
  assert(model.ebo != 0 && "invalid arg model.ebo: uninitialized element buf");
  // assert(model.texture.id != 0 &&
  //        "invalid arg model.texture: uninitialized texture");

  // glBindTexture(GL_TEXTURE_2D, model.texture.id);
  { // draw vertex
    glUseProgram(model.shader.spId);
    glBindVertexArray(model.vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model.ebo);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  }

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

void AppDestroyModel(Model model) {
  if (model.vao != 0) {
    glDeleteVertexArrays(1, &model.vao);
  }

  if (model.vbo != 0) {
    glDeleteBuffers(1, &model.vbo);
  }

  if (model.ebo != 0) {
    glDeleteBuffers(1, &model.ebo);
  }
}

char *LoadFileContents(const char *name) {
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

void UnloadFileContents(char *data) {
  assert(data != NULL && "invalid arg data: cannot be NULL");
  if (data != NULL) {
    free(data);
  }
}
