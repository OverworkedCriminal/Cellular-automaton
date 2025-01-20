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
uniform int priorityDirection;


const vec4 COLORS[] = vec4[](
  vec4(0.0f, 0.0f, 0.0f, 1.0f), // PADDING
  vec4(0.0f, 0.0f, 0.0f, 1.0f), // AIR
  vec4(1.0f, 1.0f, 0.0f, 1.0f), // SAND
  vec4(0.0f, 0.0f, 1.0f, 1.0f), // WATER_L
  vec4(0.0f, 0.0f, 1.0f, 1.0f)  // WATER_R
);

const uint MOVE_DOWN_RULES[] = uint[](
  0,  // 0b00000000 PADDING
  0,  // 0b00000000 AIR
  26, // 0b00011010 SAND
  2,  // 0b00000010 WATER_L
  2   // 0b00000010 WATER_R
);

const uint MOVE_DOWN_DIAG_RULES[] = uint[](
  0, // 0b00000000 PADDING
  0, // 0b00000000 AIR
  2, // 0b00000010 SAND
  2, // 0b00000010 WATER_L
  2  // 0b00000010 WATER_R
);

const uint MOVE_HORIZONTALLY_RULES[] = uint[](
  0,  // 0b00000000 PADDING
  0,  // 0b00000000 AIR
  0,  // 0b00000000 SAND
  18, // 0b00010010 WATER_L
  10  // 0b00001010 WATER_R
);

const uint MOVE_HORIZONTALLY_DIRECTIONS[] = uint[](
   0, // PADDING
   0, // AIR
   0, // SAND
  -1, // WATER_L
   1  // WATER_R
);

const uint MOVE_HORIZONTALLY_OPPOSITE_CELL[] = uint[](
  1,  // PADDING
  2,  // AIR
  4,  // SAND
  16, // WATER_L
  8   // WATER_R
);

void main() {
  const ivec2 coords = ivec2(gl_GlobalInvocationID.xy + uvec2(gridPadding, gridPadding));
  
  const uint idx = coords.y * gridWidth + coords.x;
  const uint ruleIdx = uint(log2(inputBuffer[idx]));

  { // MOVE_VERTICALLY
    { // MOVE IN
      const uint otherIdx = idx + gridWidth;
      const uint otherRuleIdx = uint(log2(inputBuffer[otherIdx]));
      if ((inputBuffer[idx] & MOVE_DOWN_RULES[otherRuleIdx]) > 0) {
        outputBuffer[idx] = inputBuffer[otherIdx];
        imageStore(simulationTexture, coords, COLORS[otherRuleIdx]);
        return;
      }
    }
    { // MOVE OUT
      const uint otherIdx = idx - gridWidth;
      const uint otherRuleIdx = uint(log2(inputBuffer[otherIdx]));
      if ((inputBuffer[otherIdx] & MOVE_DOWN_RULES[ruleIdx]) > 0) {
        outputBuffer[idx] = inputBuffer[otherIdx];
        imageStore(simulationTexture, coords, COLORS[otherRuleIdx]);
        return;
      }
    }
  }

  { // MOVE_DIAGONALLY
    { // MOVE IN
      const uint otherIdx = idx + gridWidth - priorityDirection;
      const uint otherRuleIdx = uint(log2(inputBuffer[otherIdx]));
      if ((inputBuffer[idx] & MOVE_DOWN_DIAG_RULES[otherRuleIdx]) > 0) {
        const uint otherUpIdx = otherIdx + gridWidth;
        const uint otherUpRuleIdx = uint(log2(inputBuffer[otherUpIdx]));
        const uint otherDownIdx = otherIdx - gridWidth;
        if (
          (inputBuffer[otherIdx] & MOVE_DOWN_RULES[otherUpRuleIdx]) == 0 &&
          (inputBuffer[otherDownIdx] & MOVE_DOWN_RULES[otherRuleIdx]) == 0
        ) {
          outputBuffer[idx] = inputBuffer[otherIdx];
          imageStore(simulationTexture, coords, COLORS[otherRuleIdx]);
          return;
        }
      }
    }
    { // MOVE OUT
      const uint otherIdx = idx - gridWidth + priorityDirection;
      const uint otherRuleIdx = uint(log2(inputBuffer[otherIdx]));
      if ((inputBuffer[otherIdx] & MOVE_DOWN_DIAG_RULES[ruleIdx]) > 0) {
        const uint otherUpIdx = otherIdx + gridWidth;
        const uint otherUpRuleIdx = uint(log2(inputBuffer[otherUpIdx]));
        const uint otherDownIdx = otherIdx - gridWidth;
        if (
          (inputBuffer[otherDownIdx] & MOVE_DOWN_RULES[otherRuleIdx]) == 0 &&
          (inputBuffer[otherIdx] & MOVE_DOWN_RULES[otherUpRuleIdx]) == 0
        ) {
          outputBuffer[idx] = inputBuffer[otherIdx];
          imageStore(simulationTexture, coords, COLORS[otherRuleIdx]);
          return;
        }
      }
    }
  }

  { // MOVE_HORIZONTALLY
    { // MOVE IN
      const uint otherIdx = idx - priorityDirection;
      const uint otherRuleIdx = uint(log2(inputBuffer[otherIdx]));
      if (
        MOVE_HORIZONTALLY_DIRECTIONS[otherRuleIdx] == priorityDirection &&
        (inputBuffer[idx] & MOVE_HORIZONTALLY_RULES[otherRuleIdx]) > 0
      ) {
        const uint otherDownIdx = otherIdx - gridWidth;
        const uint otherDownDiagIdx = otherDownIdx + priorityDirection;
        const uint otherTopIdx = otherIdx + gridWidth;
        const uint otherTopRuleIdx = uint(log2(inputBuffer[otherTopIdx]));
        const uint otherTopDiagIdx = otherTopIdx - priorityDirection;
        const uint otherTopDiagRuleIdx = uint(log2(inputBuffer[otherTopDiagIdx]));
        
        if(
          (inputBuffer[otherDownIdx] & MOVE_DOWN_RULES[otherRuleIdx]) == 0 &&
          (inputBuffer[otherIdx] & MOVE_DOWN_RULES[otherTopRuleIdx]) == 0 &&
          (inputBuffer[otherDownDiagIdx] & MOVE_DOWN_DIAG_RULES[otherRuleIdx]) == 0 &&
          (inputBuffer[otherIdx] & MOVE_DOWN_DIAG_RULES[otherTopDiagRuleIdx]) == 0
        ) {
          outputBuffer[idx] = inputBuffer[otherIdx];
          imageStore(simulationTexture, coords, COLORS[otherRuleIdx]);
          return;
        }
      }
    }
    { // MOVE OUT
      const uint direction = MOVE_HORIZONTALLY_DIRECTIONS[ruleIdx];
      const uint otherIdx = idx + priorityDirection;
      if (direction == priorityDirection) {
        const uint otherRuleIdx = uint(log2(inputBuffer[otherIdx]));
        const uint otherDownIdx = otherIdx - gridWidth;
        const uint otherDownDiagIdx = otherIdx - gridWidth + priorityDirection;
        const uint otherUpIdx = otherIdx + gridWidth;
        const uint otherUpRuleIdx = uint(log2(inputBuffer[otherUpIdx]));
        const uint otherUpDiagIdx = otherIdx + gridWidth - priorityDirection;
        const uint otherUpDiagRuleIdx = uint(log2(inputBuffer[otherUpDiagIdx]));

        if (
          (inputBuffer[otherIdx] & MOVE_HORIZONTALLY_RULES[ruleIdx]) > 0 &&
          (inputBuffer[otherDownIdx] & MOVE_DOWN_RULES[otherRuleIdx]) == 0 &&
          (inputBuffer[otherIdx] & MOVE_DOWN_RULES[otherUpRuleIdx]) == 0 &&
          (inputBuffer[otherDownDiagIdx] & MOVE_DOWN_DIAG_RULES[otherRuleIdx]) == 0 &&
          (inputBuffer[otherIdx] & MOVE_DOWN_DIAG_RULES[otherUpDiagRuleIdx]) == 0
        ) {
          outputBuffer[idx] = inputBuffer[otherIdx];
        } else {
          outputBuffer[idx] = MOVE_HORIZONTALLY_OPPOSITE_CELL[ruleIdx];
        }
        return;
      }
    }
  }

  outputBuffer[idx] = inputBuffer[idx];
  imageStore(simulationTexture, coords, COLORS[ruleIdx]);
}