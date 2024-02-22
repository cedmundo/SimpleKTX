#include "ktxView.h"

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define WINDOW_TITLE "ktxView"

int main() {
  StatusCode status = AppInit(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE);
  if (status != SUCCESS) {
    AppClose();
    return Exit(status);
  }

  Shader shader = AppLoadShader("data/plane.vs", "data/plane.fs");
  if (shader.status != SUCCESS) {
    AppClose();
    return Exit(shader.status);
  }

  // TODO(cedmundo): Create and upload plane.
  // TODO(cedmundo): Load and upload texture.

  // Main loop
  while (!AppShouldClose()) {
    AppBeginFrame();
    // TODO(cemdundo): Bind plane and render it.
    AppEndFrame();
  }

  AppClose();
  return Exit(status);
}
