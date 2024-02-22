#pragma once
#include <stdbool.h>

typedef enum {
  SUCCESS,
  E_CANNOT_INIT_GLFW,
  E_CANNOT_CREATE_WINDOW,
  E_CANNOT_LOAD_GL,
  E_CANNOT_LOAD_FILE,
  E_SHADER_COMPILE_ERROR,
  E_SHADER_LINK_ERROR,
  E_ERROR_COUNT,
} StatusCode;

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

// LoadTextFile: load text contents from file.
char *LoadTextFile(const char *name);

// UnloadTextFile: unload text contents.
void UnloadTextFile(char *);

#define CHECK_TEXT_FILE_ERROR(data, file)                                      \
  do {                                                                         \
    if ((data) == NULL) {                                                      \
      fprintf(stderr, "cannot load file: " file "\n");                         \
      status = E_CANNOT_LOAD_FILE;                                             \
      goto terminate;                                                          \
    }                                                                          \
  } while (0)

#define CHECK_SHADER_COMPILE_ERROR(shader_id)                                  \
  do {                                                                         \
    GLint success = 0;                                                         \
    char compile_log[512] = {0};                                               \
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);                     \
    if (!success) {                                                            \
      glGetShaderInfoLog(shader_id, 512, NULL, compile_log);                   \
      fprintf(stderr, "error compiling shader: %s\n", compile_log);            \
      status = E_SHADER_COMPILE_ERROR;                                         \
      goto terminate;                                                          \
    }                                                                          \
  } while (0)

#define CHECK_SHADER_LINK_ERROR(program_id)                                    \
  do {                                                                         \
    GLint success = 0;                                                         \
    char link_log[512] = {0};                                                  \
    glGetProgramiv(program_id, GL_LINK_STATUS, &success);                      \
    if (!success) {                                                            \
      glGetShaderInfoLog(program_id, 512, NULL, link_log);                     \
      fprintf(stderr, "error linking shader: %s\n", link_log);                 \
      status = E_SHADER_LINK_ERROR;                                            \
      goto terminate;                                                          \
    }                                                                          \
  } while (0)

#define CHECK_ERROR(cond, e)                                                   \
  do {                                                                         \
    if ((cond)) {                                                              \
      status = e;                                                              \
      goto terminate;                                                          \
    }                                                                          \
  } while (0)
