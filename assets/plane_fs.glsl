#version 330 core

out vec4 FragColor;

in vec3 col;
in vec2 uvs;
// uniform sampler2D tex0;

void main() {
  FragColor = vec4(col, 1.0); // texture(tex0, uvs) * vec4(col, 1.0);
}
