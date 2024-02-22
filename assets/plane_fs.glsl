#version 330 core

out vec4 FragColor;

in vec3 col;
in vec2 uvs;

uniform sampler2D tex0;

void main() {
  FragColor = texture(tex0, uvs);
}
