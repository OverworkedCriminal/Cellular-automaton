#ifndef APPLICATION_SIMULATION_CPU_COLOR_HPP
#define APPLICATION_SIMULATION_CPU_COLOR_HPP

union Color {
  struct {
    float r;
    float g;
    float b;
    float a;
  };

  float component[4];
};


#endif