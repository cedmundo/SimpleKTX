#version 330 core

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inCol;
layout (location = 2) in vec2 inUvs;

out vec3 col;
out vec2 uvs;

void main() {
  gl_Position = vec4(inPos, 1.0);
  col = inCol;
  uvs = inUvs;
}
