#version 460 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(std430, binding = 0) readonly buffer InputBuffer {
  uint inputBuffer[];
};
layout(std430, binding = 1) writeonly buffer OutputBuffer {
  uint outputBuffer[];
};

uniform writeonly image2D simulationTexture;
uniform uint gridWidth;
uniform uint gridHeight;
uniform uint gridPadding;

const vec4 colors[] = vec4[](
  vec4(1.0, 1.0, 1.0, 1.0)
);


uint bufferIdx(uvec2 coords) {
  return coords.y * gridWidth + coords.x;
}

uint calculateCell(uvec2 coords) {
  return 0;
}


void main() {
  uvec2 coords = gl_GlobalInvocationID.xy + uvec2(gridPadding, gridPadding);

  uint cell = calculateCell(coords);
  vec4 color = colors[cell];

  imageStore(simulationTexture, ivec2(coords), color);
}