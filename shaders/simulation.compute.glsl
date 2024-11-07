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

const uint PADDING = 1;

/**
 * To access color use: color(cell)
 */
const vec4 COLORS[] = vec4[](
  vec4(1.0, 1.0, 1.0, 1.0), // PADDING 1
  vec4(0.0, 0.0, 0.0, 1.0), // AIR     2
  vec4(1.0, 1.0, 0.0, 1.0), // SAND    4
  vec4(0.0, 0.0, 1.0, 1.0)  // WATER   8
);

/**
 * To access mask use: fallMask(cell)
 * To apply mask use: cell & fallMask(cell)
 */
const uint FALL_MASKS[] = uint[](
  0,  // 0b00000000 PADDING
  0,  // 0b00000000 AIR
  10, // 0b00001010 SAND
  2   // 0b00000010 WATER
);

/**
 * Utility function that calculates idx to input/output buffers
 */
uint bufferIdx(uvec2 coords) {
  return coords.y * gridWidth + coords.x;
}

/**
 * Utility function that simplifies selecting value from COLORS
 */
vec4 color(uint cell) {
  uint idx = uint(log2(cell));
  return COLORS[idx];
}

/**
 * Utility function that simplifies selecting value from FALL_MASKS
 */
uint fallMask(uint cell) {
  uint idx = uint(log2(cell));
  return FALL_MASKS[idx];
}

// Returns cellValue that fall into this idx
// Returns 0 if no cellValue can fall into this idx
uint fallInto(uint idx) {
  uint mask;
  uint checkedCell;

  uint cell = inputBuffer[idx];

  checkedCell = inputBuffer[idx + gridWidth];
  mask = fallMask(checkedCell);
  if ((cell & mask) > 0) {
    return checkedCell;
  }

  checkedCell = inputBuffer[idx + gridWidth + 1];
  mask = fallMask(checkedCell);
  if ((cell & mask) > 0 && (inputBuffer[idx + 1] & mask) == 0) {
    return checkedCell;
  }

  checkedCell = inputBuffer[idx + gridWidth - 1];
  mask = fallMask(checkedCell);
  if ((cell & mask) > 0 && (inputBuffer[idx - 1] & mask) == 0 && (inputBuffer[idx - 2] & mask) == 0) {
    return checkedCell;
  }

  return 0;
}

// Returns cellValue from below idx that replaces current cellValue
// Returns 0 if cellValue can't fall down
uint fallOut(uint idx) {
  uint checkedCell;

  uint mask = fallMask(inputBuffer[idx]);
  if (mask == 0) {
    return 0;
  }

  checkedCell = inputBuffer[idx - gridWidth];
  if ((checkedCell & mask) > 0) {
    return checkedCell;
  }

  checkedCell = inputBuffer[idx - gridWidth - 1];
  if ((checkedCell & mask) > 0 && (checkedCell & fallMask(inputBuffer[idx - 1])) == 0) {
    return checkedCell;
  }

  checkedCell = inputBuffer[idx - gridWidth + 1];
  if ((checkedCell & mask) > 0) {
    if ((checkedCell & fallMask(inputBuffer[idx + 1])) > 0) {
      return 0;
    }
    if ((checkedCell & fallMask(inputBuffer[idx + 2])) > 0 && (inputBuffer[idx - gridWidth + 2] & fallMask(inputBuffer[idx + 2])) == 0) {
      return 0;
    }
    return checkedCell;
  }

  return 0;
}

uint calculateCellValue(uint idx) {
  uint newCellValue;

  uint currentCellValue = inputBuffer[idx];
  if (currentCellValue == PADDING) {
    return PADDING;
  }

  newCellValue = fallInto(idx);
  if (newCellValue != 0) {
    return newCellValue;
  }

  newCellValue = fallOut(idx);
  if (newCellValue != 0) {
    return newCellValue;
  }

  return currentCellValue;
}


void main() {
  uvec2 coords = gl_GlobalInvocationID.xy + uvec2(gridPadding, gridPadding);
  uint idx = bufferIdx(coords);

  uint cellValue = calculateCellValue(idx);
  vec4 cellColor = color(cellValue);

  outputBuffer[idx] = cellValue;
  imageStore(simulationTexture, ivec2(coords), cellColor);
}