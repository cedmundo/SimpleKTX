#include <stdio.h>

#include "ktxView.h"

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define WINDOW_TITLE "ktxView"

int main() {
  StatusCode status;

  status = AppInit(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE);
  if (status != SUCCESS) {
    AppClose();
    return Exit(status);
  }

  // TODO(cedmundo): Load and compile shaders.
  // TODO(cedmundo): Create and load plane.
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
