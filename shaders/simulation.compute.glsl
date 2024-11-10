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
/**
 * Should be:
 * -1 LEFT
 *  1 RIGHT
 */
uniform int priorityDirection;

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
 * To access mask use: fallDownRule(cell)
 * Using: cell & fallDownRule(otherCell) says whether
 * otherCell can fall through cell
 */
const uint FALL_DOWN_RULES[] = uint[](
  0,  // 0b00000000 PADDING
  0,  // 0b00000000 AIR
  10, // 0b00001010 SAND
  2   // 0b00000010 WATER
);

/**
 * To access mask use: fallDiagRule(cell)
 * Using: cell & fallDiagRule(otherCell) says whether
 * otherCell can fall diagonally through cell
 */
const uint FALL_DIAG_RULES[] = uint[](
  0, // 0b00000000 PADDING
  0, // 0b00000000 AIR
  2, // 0b00000010 SAND
  2  // 0b00000010 WATER
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

uint fallDownRule(uint cell) {
  uint idx = uint(log2(cell));
  return FALL_DOWN_RULES[idx];
}

uint fallDiagRule(uint cell) {
  uint idx = uint(log2(cell));
  return FALL_DIAG_RULES[idx];
}

uint moveRule(uint cell) {
  uint idx = uint(log2(cell));
  return MOVE_RULES[idx];
}

uint fall_down(uint idx) {
  uint otherIdx;
  uint mask;

  uint cell = inputBuffer[idx];

  // move into this cell
  otherIdx = idx + gridWidth;
  mask = fallDownRule(inputBuffer[otherIdx]);
  if ((cell & mask) > 0) {
    // other can fall down (here)
    return otherIdx;
  }

  // move out of this cell
  otherIdx = idx - gridWidth;
  mask = fallDownRule(cell);
  if ((inputBuffer[otherIdx] & mask) > 0) {
    // cell can fall down
    return otherIdx;
  }

  return idx;
}

uint fall_diag(uint idx, uint direction) {
  uint otherIdx;
  uint mask;

  uint cell = inputBuffer[idx];

  // move into this cell
  otherIdx = idx + gridWidth - direction;
  mask = fallDiagRule(inputBuffer[otherIdx]);
  if ((cell & mask) > 0) {
    // other can fall here diagonally
    return otherIdx;
  }

  // move out of this cell
  otherIdx = idx - gridWidth + direction;
  mask = fallDiagRule(cell);
  if ((inputBuffer[otherIdx] & mask) > 0) {
    // can fall diagonally
    return otherIdx;
  }

  return idx;
}

/**
 * Returns index to inputBuffer of value that should
 * be placed at idx.
 * Returns idx when value should not be changed
 */
uint fall(uint idx) {
  // return idx;
  uint newIdx;

  newIdx = fall_down(idx);
  if (newIdx != idx) {
    return newIdx;
  }

  newIdx = fall_diag(idx, priorityDirection);
  if (newIdx != idx) {
    if (fall_down(newIdx) == newIdx) {
      return newIdx;
    }
  }

  newIdx = fall_diag(idx, -priorityDirection);
  if (newIdx != idx) {
    if (fall_down(newIdx) == newIdx && fall_diag(newIdx, priorityDirection) == newIdx) {
      return newIdx;
    }
  }

  return idx;
}

uint move_side(uint idx, uint direction) {
  uint otherIdx;
  uint mask;

  uint cell = inputBuffer[idx];

  // move into this cell
  otherIdx = idx - direction;
  mask = moveRule(inputBuffer[otherIdx]);
  if ((cell & mask) > 0) {
    // other can move (here)
    return otherIdx;
  }

  // move out of this cell

  otherIdx = idx + direction;
  mask = moveRule(cell);
  if ((inputBuffer[otherIdx] & mask) > 0) {
    // can move
    return otherIdx;
  }

  return idx;
}

/**
 * Returns index to inputBuffer of value that should
 * be placed at idx.
 * Returns idx when value should not be changed
 */
uint move(uint idx) {
  uint newIdx;

  newIdx = move_side(idx, priorityDirection);
  if (newIdx != idx) {
    if (fall(newIdx) == newIdx) {
      return newIdx;
    }
  }

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