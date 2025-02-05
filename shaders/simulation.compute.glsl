#version 460 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(std430, binding = 0) readonly buffer InputBuffer {
  uint inputBuffer[];
};
layout(std430, binding = 1) writeonly buffer OutputBuffer {
  uint outputBuffer[];
};

uniform writeonly image2D simulationTexture;

uniform vec4 COLORS[32];
uniform uint RULE_VERTICAL[32];
uniform uint RULE_DIAGONAL[32];
uniform uint RULE_HORIZONTAL[32];
uniform int RULE_HORIZONTAL_DIRECTIONS[32];
uniform uint RULE_HORIZONTAL_OPPOSITE_DIRECTION_CELL[32];

uniform uint gridWidth;
uniform uint gridPadding;
uniform int priorityDirection;


void main() {
  const ivec2 coords = ivec2(gl_GlobalInvocationID.xy + uvec2(gridPadding, gridPadding));
  
  const uint idx = coords.y * gridWidth + coords.x;
  const uint ruleIdx = uint(log2(inputBuffer[idx]));

  { // MOVE_VERTICALLY
    { // MOVE OUT
      const uint otherIdx = idx - gridWidth;
      const uint otherRuleIdx = uint(log2(inputBuffer[otherIdx]));
      if ((inputBuffer[otherIdx] & RULE_VERTICAL[ruleIdx]) > 0) {             // can move down
        const uint otherDownIdx = otherIdx - gridWidth;
        if ((inputBuffer[otherDownIdx] & RULE_VERTICAL[otherRuleIdx]) == 0) { // other can't move down
          outputBuffer[idx] = inputBuffer[otherIdx];
          imageStore(simulationTexture, coords, COLORS[otherRuleIdx]);
          return;
        }
      }
    }
    { // MOVE IN
      const uint otherIdx = idx + gridWidth;
      const uint otherRuleIdx = uint(log2(inputBuffer[otherIdx]));
      if ((inputBuffer[idx] & RULE_VERTICAL[otherRuleIdx]) > 0) { // other can move down
        outputBuffer[idx] = inputBuffer[otherIdx];
        imageStore(simulationTexture, coords, COLORS[otherRuleIdx]);
        return;
      }
    }
  }

  { // MOVE_DIAGONALLY
    { // MOVE OUT
      const uint otherIdx = idx - gridWidth + priorityDirection;
      if ((inputBuffer[otherIdx] & RULE_DIAGONAL[ruleIdx]) > 0) { // can move diagonally
        const uint otherRuleIdx = uint(log2(inputBuffer[otherIdx]));
        const uint otherTopIdx = otherIdx + gridWidth;
        const uint otherTopRuleIdx = uint(log2(inputBuffer[otherTopIdx]));
        const uint otherDownIdx = otherIdx - gridWidth;
        const uint otherDiagonalIdx = otherIdx - gridWidth + priorityDirection;
        if (
          (inputBuffer[otherIdx] & RULE_VERTICAL[otherTopRuleIdx]) == 0 &&   // other top can't move vertically
          (inputBuffer[otherDownIdx] & RULE_VERTICAL[otherRuleIdx]) == 0 &&  // other can't move vertically
          (inputBuffer[otherDiagonalIdx] & RULE_DIAGONAL[otherRuleIdx]) == 0 // other can't move diagonally
        ) {
          outputBuffer[idx] = inputBuffer[otherIdx];
          imageStore(simulationTexture, coords, COLORS[otherRuleIdx]);
          return;
        }
      }
    }
    { // MOVE IN
      const uint otherIdx = idx + gridWidth - priorityDirection;
      const uint otherRuleIdx = uint(log2(inputBuffer[otherIdx]));
      if ((inputBuffer[idx] & RULE_DIAGONAL[otherRuleIdx]) > 0) { // other can move diagonally
        const uint otherTopIdx = otherIdx + gridWidth;
        const uint otherTopRuleIdx = uint(log2(inputBuffer[otherTopIdx]));
        const uint otherDownIdx = otherIdx - gridWidth;
        if (
          (inputBuffer[otherIdx] & RULE_VERTICAL[otherTopRuleIdx]) == 0 && // other top can't move vertically
          (inputBuffer[otherDownIdx] & RULE_VERTICAL[otherRuleIdx]) == 0   // other can't move vertically
        ) {
          outputBuffer[idx] = inputBuffer[otherIdx];
          imageStore(simulationTexture, coords, COLORS[otherRuleIdx]);
          return;
        }
      }
    }
  }

  { // MOVE_HORIZONTALLY
    bool shouldChangeDirection = false;
    uint directionToChangeTo;

    { // MOVE OUT
      const int direction = RULE_HORIZONTAL_DIRECTIONS[ruleIdx];
      const uint otherIdx = idx + priorityDirection;
      if (direction == priorityDirection) { // simulation processes right direction
        const uint otherRuleIdx = uint(log2(inputBuffer[otherIdx]));
        const uint otherDirectionIdx = otherIdx + priorityDirection;
        const uint otherDownIdx = otherIdx - gridWidth;
        const uint otherTopIdx = otherIdx + gridWidth;
        const uint otherTopRuleIdx = uint(log2(inputBuffer[otherTopIdx]));
        const uint otherDownDiagIdx = otherIdx - gridWidth + priorityDirection;
        const uint otherTopDiagIdx = otherIdx + gridWidth - priorityDirection;
        const uint otherTopDiagRuleIdx = uint(log2(inputBuffer[otherTopDiagIdx]));
        if (
          (inputBuffer[otherIdx] & RULE_HORIZONTAL[ruleIdx]) > 0 &&                // can move horizontally
          (inputBuffer[otherDirectionIdx] & RULE_HORIZONTAL[otherRuleIdx]) == 0 && // other can't move horizontally
          (inputBuffer[otherDownIdx] & RULE_VERTICAL[otherRuleIdx]) == 0 &&        // other can't move vertically
          (inputBuffer[otherIdx] & RULE_VERTICAL[otherTopRuleIdx]) == 0 &&         // other top can't move vertically
          (inputBuffer[otherDownDiagIdx] & RULE_DIAGONAL[otherRuleIdx]) == 0 &&    // other can't move diagonally
          (inputBuffer[otherIdx] & RULE_DIAGONAL[otherTopDiagRuleIdx]) == 0        // other top diag can't move diagonally
        ) {
          outputBuffer[idx] = inputBuffer[otherIdx];
          imageStore(simulationTexture, coords, COLORS[otherRuleIdx]);
          return;
        }

        shouldChangeDirection = true;
        directionToChangeTo = RULE_HORIZONTAL_OPPOSITE_DIRECTION_CELL[ruleIdx];
      }
    }
    { // MOVE IN
      const uint otherIdx = idx - priorityDirection;
      const uint otherRuleIdx = uint(log2(inputBuffer[otherIdx]));
      const int otherDirection = RULE_HORIZONTAL_DIRECTIONS[otherRuleIdx];
      if (otherDirection == priorityDirection) { // simulation processes right direction
        const uint otherDownIdx = otherIdx - gridWidth;
        const uint otherTopIdx = otherIdx + gridWidth;
        const uint otherTopRuleIdx = uint(log2(inputBuffer[otherTopIdx]));
        const uint otherDownDiagIdx = otherIdx - gridWidth + priorityDirection;
        const uint otherTopDiagIdx = otherIdx + gridWidth - priorityDirection;
        const uint otherTopDiagRuleIdx = uint(log2(inputBuffer[otherTopDiagIdx]));
        if (
          (inputBuffer[idx] & RULE_HORIZONTAL[otherRuleIdx]) > 0 &&             // other can move horizontally
          (inputBuffer[otherDownIdx] & RULE_VERTICAL[otherRuleIdx]) == 0 &&     // other can't move vertically
          (inputBuffer[otherIdx] & RULE_VERTICAL[otherTopRuleIdx]) == 0 &&      // other top can't move vertically
          (inputBuffer[otherDownDiagIdx] & RULE_DIAGONAL[otherRuleIdx]) == 0 && // other can't move diagonally
          (inputBuffer[otherIdx] & RULE_DIAGONAL[otherTopDiagRuleIdx]) == 0     // other top diag can't move diagonally
        ) {
          outputBuffer[idx] = inputBuffer[otherIdx];
          imageStore(simulationTexture, coords, COLORS[otherRuleIdx]);
          return;
        }
      }
    }
    { // WANTS TO MOVE BUT CAN'T
      if (shouldChangeDirection) {
        const uint directionToChangeToRuleIdx = uint(log2(directionToChangeTo));
        outputBuffer[idx] = directionToChangeTo;
        imageStore(simulationTexture, coords, COLORS[directionToChangeToRuleIdx]);
        return;
      }
    }
  }

  outputBuffer[idx] = inputBuffer[idx];
  imageStore(simulationTexture, coords, COLORS[ruleIdx]);
}