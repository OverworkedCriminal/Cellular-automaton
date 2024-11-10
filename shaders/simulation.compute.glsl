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
  vec4(1.0, 1.0, 1.0, 1.0), // PADDING  1
  vec4(0.0, 0.0, 0.0, 1.0), // AIR      2
  vec4(1.0, 1.0, 0.0, 1.0), // SAND     4
  vec4(0.0, 0.0, 1.0, 1.0), // WATER_L  8
  vec4(0.2, 0.2, 1.0, 1.0)  // WATER_R 16
);

/**
 * To access mask use: fallDownRule(cell)
 * Using: cell & fallDownRule(otherCell) says whether
 * otherCell can fall through cell
 */
const uint FALL_DOWN_RULES[] = uint[](
  0,  // 0b00000000 PADDING
  0,  // 0b00000000 AIR
  26, // 0b00011010 SAND
  2,  // 0b00000010 WATER_L
  2   // 0b00000010 WATER_R
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
  2, // 0b00000010 WATER_L
  2  // 0b00000010 WATER_R
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
  2, // 0b00000010 WATER_L
  2  // 0b00000010 WATER_R
);

/**
 * To access value use: moveDirection(cell)
 * Value says in which direction cell should move
 */
const int MOVE_DIRECTIONS[] = int[](
   0, // PADDING
   0, // AIR
   0, // SAND
  -1, // WATER_L
   1  // WATER_R
);

/**
 * To access value use: moveOppositeDirectionCell(cell)
 * Value says what is the same cell that moves in opposite direction
 */
const uint MOVE_OPPOSITE_DIRECTION_CELL[] = uint[](
  1,  // PADDING
  2,  // AIR
  4,  // SAND
  16, // WATER_L
  8   // WATER_R
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

int moveDirection(uint cell) {
  uint idx = uint(log2(cell));
  return MOVE_DIRECTIONS[idx];
}

uint moveOppositeDirectionCell(uint cell) {
  uint idx = uint(log2(cell));
  return MOVE_OPPOSITE_DIRECTION_CELL[idx];
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

uint move_side(uint idx) {
  uint otherIdx;
  uint otherCell;
  uint mask;

  uint cell = inputBuffer[idx];

  // move into this cell

  otherIdx = idx - priorityDirection;
  otherCell = inputBuffer[otherIdx];
  if (moveDirection(otherCell) == priorityDirection) {
    // otherCell moves in this direction
    mask = moveRule(otherCell);
    if ((cell & mask) > 0) {
      // otherCell can move here
      return otherIdx;
    }
  }

  otherIdx = idx + priorityDirection;
  otherCell = inputBuffer[otherIdx];
  if (moveDirection(otherCell) == -priorityDirection) {
    // otherCell moves in this direction
    mask = moveRule(otherCell);
    if ((cell & mask) > 0) {
      return otherIdx;
    }
  }

  // move out of this cell

  uint cellDirection = moveDirection(cell);
  uint cellMask = moveRule(cell);

  otherIdx = idx + cellDirection;
  otherCell = inputBuffer[otherIdx];

  if ((otherCell & cellMask) > 0) {
    // can move
    if (cellDirection == priorityDirection) {
      // this direction has priority
      return otherIdx;
    }

    // this direction does not have priority
    // It's necessary to check whether other
    // cell has priority to move into otherIdx

    uint farIdx = idx - 2 * priorityDirection;
    uint farCell = inputBuffer[farIdx];
    uint farDirection = moveDirection(farCell);
    uint farMask = moveRule(farCell);
    if ((otherCell & farMask) > 0 && farDirection == priorityDirection) {
      // farCell can move into otherIdx AND it has priority to move into otherIdx
      return idx;
    } else {
      // farCell can't move into otherIdx OR farCell does move in other side
      return otherIdx;
    }
  }

  return idx;
}

/**
 * Returns value that should be placed at idx.
 */
uint move(uint idx) {
  uint newIdx;

  newIdx = move_side(idx);
  if (newIdx != idx) {
    if (fall(newIdx) == newIdx) {
      return inputBuffer[newIdx];
    } else {
      return moveOppositeDirectionCell(inputBuffer[idx]);
    }
  } else {
    return moveOppositeDirectionCell(inputBuffer[idx]);
  }

  return inputBuffer[idx];
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

  return move(idx);
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