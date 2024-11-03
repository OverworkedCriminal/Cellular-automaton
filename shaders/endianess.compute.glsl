#version 460 core

//
// The only goal of this shader is writing number 1
// to shader storage buffer object (SSBO)
//
// SSBO's are used as byte buffers but GLSL
// works on minimum 4 byte types. This shader
// allows to check endianess of GPU.
//

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(std430, binding = 0) writeonly buffer OutputBuffer {
  uint outputBuffer[];
};


void main() {
  outputBuffer[0] = 1;
}
