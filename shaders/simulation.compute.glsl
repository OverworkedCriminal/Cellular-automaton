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
  vec4(0.5, 0.5, 0.5, 1.0), // BLANK
  vec4(0.0, 0.0, 0.0, 1.0), // AIR
  vec4(1.0, 1.0, 0.0, 1.0)  // SAND
);

const uint BLANK = 0;
const uint AIR   = 1;
const uint SAND  = 2;

uint bufferIdx(uvec2 coords) {
  return coords.y * gridWidth + coords.x;
}

bool shouldBeSand(uint idx) {
  // Sand falls down
  // When it's impossible it falls left
  // When it's impossible it falls right
  // When it's impossible it stays in place

  if (inputBuffer[idx] == AIR) {
    if (inputBuffer[idx + gridWidth] == SAND) {
      return true;
    }
    if (inputBuffer[idx + gridWidth + 1] == SAND && inputBuffer[idx + 1] == SAND) {
      return true;
    }
    if (inputBuffer[idx + gridWidth - 1] == SAND && inputBuffer[idx - 1] == SAND && inputBuffer[idx - 2] == SAND) {
      return true;
    }

    return false;
  } else {
    // is bottom empty
    if (inputBuffer[idx - gridWidth] == AIR) {
      return false;
    }

    // is left bottom empty
    if (inputBuffer[idx - gridWidth - 1] == AIR) {
      if (inputBuffer[idx - 1] == AIR) {
        return false;
      }
    }

    // is right bottom empty
    if (inputBuffer[idx - gridWidth + 1] == AIR) {
      if (inputBuffer[idx + 1] == SAND) {
        return true;
      }
      if (inputBuffer[idx + 2] == SAND && inputBuffer[idx - gridWidth + 2] == SAND) {
        return true;
      }
      return false;
    }

    return true;
  }
}

uint calculateCellValue(uint idx) {
  uint currentCellValue = inputBuffer[idx];
  if (currentCellValue == BLANK) {
    return BLANK;
  }

  if (shouldBeSand(idx)) {
    return SAND;
  }

  return AIR;
}


void main() {
  uvec2 coords = gl_GlobalInvocationID.xy + uvec2(gridPadding, gridPadding);
  uint idx = bufferIdx(coords);

  uint cellValue = calculateCellValue(idx);
  vec4 color = colors[cellValue];

  outputBuffer[idx] = cellValue;
  imageStore(simulationTexture, ivec2(coords), color);
}