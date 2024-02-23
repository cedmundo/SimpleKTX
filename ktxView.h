#pragma once
#include <ktx.h>
#include <stdbool.h>

typedef enum {
  SUCCESS,
  E_CANNOT_INIT_GLFW,
  E_CANNOT_CREATE_WINDOW,
  E_CANNOT_LOAD_GL,
  E_CANNOT_LOAD_FILE,
  E_SHADER_COMPILE_ERROR,
  E_SHADER_LINK_ERROR,
  E_CANNOT_CREATE_TEXTURE,
  E_CANNOT_UPLOAD_TEXTURE,
  E_ERROR_COUNT,
} StatusCode;

typedef struct {
  StatusCode status;
  unsigned spId;
} Shader;

typedef struct {
  StatusCode status;
  unsigned id;
  ktxTexture *src;
  GLenum format;
} Texture;

typedef struct {
  unsigned vao;
  unsigned vbo;
  unsigned ebo;
  Shader shader;
  Texture texture;
} Model;

// Exit: Call system exit and prints error if needed.
int Exit(StatusCode status);

// Startup app window and initial setup.
StatusCode AppInit(int window_width, int window_height,
                   const char *window_title);

// Return true if app should be closed.
bool AppShouldClose();

// Setup the next frame.
void AppBeginFrame();

// Commit frame and poll events.
void AppEndFrame();

// Closes the app window and clears all internal data.
void AppClose();

// Load, compile and link a shader program using a fragment and vertex shaders.
Shader AppLoadShader(const char *vsPath, const char *fsPath);

// Load and decode an image as texture
Texture AppLoadTexture(const char *texPath);

// Release all resources linked to a texture
void AppDestroyTexture(Texture texture);

// Make a plane using the size as vertex positions and attach a texture (KTX2)
Model AppMakePlane(float dim);

// Render a model
void AppRenderModel(Model model);

// Release all resources linked to a model
void AppDestroyModel(Model model);

// Destroy a shader if needed.
void AppDestroyShader(Shader shader);
