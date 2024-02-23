#include "app.h"

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define WINDOW_TITLE "SimpleKTX"

#define PLANE_VS_PATH "assets/plane_vs.glsl"
#define PLANE_FS_PATH "assets/plane_fs.glsl"
#define PLANE_TEXTURE "assets/plane_tex.ktx"

int main() {
  StatusCode status = AppInit(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE);
  if (status != SUCCESS) {
    AppClose();
    return Exit(status);
  }

  Shader shader = AppLoadShader(PLANE_VS_PATH, PLANE_FS_PATH);
  if (shader.status != SUCCESS) {
    AppClose();
    return Exit(shader.status);
  }

  // Make and upload model & texture
  Model model = AppMakePlane(0.5f);
  Texture texture = AppLoadTexture(PLANE_TEXTURE);
  if (texture.status != SUCCESS) {
    AppClose();
    return Exit(texture.status);
  }

  // Assign the shader and the texture to the model
  model.shader = shader;
  model.texture = texture;

  // Main loop
  while (!AppShouldClose()) {
    AppBeginFrame();
    { // Render entities
      AppRenderModel(model);
    }
    AppEndFrame();
  }

  AppDestroyModel(model);
  AppClose();
  return Exit(status);
}
