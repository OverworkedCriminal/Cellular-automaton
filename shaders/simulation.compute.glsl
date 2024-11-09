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
 * To access mask use: fallRule(cell)
 * Using: cell & fallRule(otherCell) says whether
 * otherCell can fall through cell
 */
const uint FALL_RULES[] = uint[](
  0,  // 0b00000000 PADDING
  0,  // 0b00000000 AIR
  10, // 0b00001010 SAND
  2   // 0b00000010 WATER
);

/**
 * To access mask use: moveRule(cell)
 * Using: cell & moveRule(otherCell) says whether
 * otherCell can move through cell
 */
const uint MOVE_RULES[] = uint[](
  0, // 0b00000000 PADDING
  0, // 0b00000000 AIR
  0, // 0b00000000 SAND
  2  // 0b00000010 WATER
);

uint bufferIdx(uvec2 coords) {
  return coords.y * gridWidth + coords.x;
}

vec4 color(uint cell) {
  uint idx = uint(log2(cell));
  return COLORS[idx];
}

uint fallRule(uint cell) {
  uint idx = uint(log2(cell));
  return FALL_RULES[idx];
}

uint moveRule(uint cell) {
  uint idx = uint(log2(cell));
  return MOVE_RULES[idx];
}

/**
 * Returns idx of cellValue that should be placed at idx
 * Returns passed idx if cellValue should remain unchanged
 */
uint fall(uint idx) {
  uint otherIdx;
  uint otherCell;
  uint mask;

  uint cell = inputBuffer[idx];

  // Falling into this cell

  otherIdx = idx + gridWidth;
  mask = fallRule(inputBuffer[otherIdx]);
  if ((cell & mask) > 0) {
    // other can fall down (here)
    return otherIdx;
  }

  otherIdx = idx + gridWidth + 1;
  mask = fallRule(inputBuffer[otherIdx]);
  if ((cell & mask) > 0) {
    // other can fall left (here)
    if ((inputBuffer[idx + 1] & mask) == 0) {
      // other can't fall down
      return otherIdx;
    }
  }

  otherIdx = idx + gridWidth - 1;
  mask = fallRule(inputBuffer[otherIdx]);
  if ((cell & mask) > 0) {
    // other can fall right (here)
    if ((inputBuffer[idx - 1] & mask) == 0 && (inputBuffer[idx - 2] & mask) == 0) {
      // other can't fall down AND other can't fall left
      return otherIdx;
    }
  }

  // Falling out of this cell

  mask = fallRule(cell);

  otherIdx = idx - gridWidth;
  otherCell = inputBuffer[otherIdx];
  if ((otherCell & mask) > 0) {
    // can fall down
    return otherIdx;
  }

  otherIdx = idx - gridWidth - 1;
  otherCell = inputBuffer[otherIdx];
  if ((otherCell & mask) > 0) {
    // can fall left
    uint otherMask = fallRule(inputBuffer[idx - 1]);
    if ((otherCell & otherMask) == 0) {
      // other can't fall down
      return otherIdx;
    }
  }

  otherIdx = idx - gridWidth + 1;
  otherCell = inputBuffer[otherIdx];
  if ((otherCell & mask) > 0) {
    // can fall right
    uint rightMask = fallRule(inputBuffer[idx + 1]);
    if ((otherCell & rightMask) > 0) {
      // right can fall down and has priority to do so
      return idx;
    }
    uint farRightMask = fallRule(inputBuffer[idx + 2]);
    if ((otherCell & farRightMask) > 0 && (inputBuffer[idx - gridWidth + 2] & farRightMask) == 0) {
      // far right can fall left AND far right can't fall down
      return idx;
    }
    // right can't fall down AND far right can't fall left
    return otherIdx;
  }

  return idx;
}

/**
 * Returns idx of cellValue that should be placed at idx
 * Returns passed idx if cellValue should remain unchanged
 */
uint move(uint idx) {
  return idx;
}

uint calculateCellValue(uint idx) {
  uint newCellValueIdx;

  uint currentCellValue = inputBuffer[idx];
  if (currentCellValue == PADDING) {
    return PADDING;
  }

  newCellValueIdx = fall(idx);
  if (newCellValueIdx != idx) {
    return inputBuffer[newCellValueIdx];
  }

  newCellValueIdx = move(idx);
  if (newCellValueIdx != idx) {
    return inputBuffer[newCellValueIdx];
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