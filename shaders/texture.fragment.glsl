#version 460 core

layout(location = 0) out vec4 outFragColor;
layout(location = 0) in  vec2 inTexCoord;

uniform sampler2D simulationTexture;

void main() {
  outFragColor = texture(simulationTexture, inTexCoord);
}