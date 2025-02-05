#include "engine/time/TimeSystem.hpp"
#include "GLFW/glfw3.h"

namespace engine::time {

auto TimeSystem::create() -> TimeSystem {
  const double time = glfwGetTime();
  const double timeDelta = 0;
  
  return TimeSystem(time, timeDelta);
}

TimeSystem::TimeSystem(double time, double timeDelta)
  :m_time(time)
  ,m_timeDelta(timeDelta)
{}

auto TimeSystem::getTime() const -> double {
  return m_time;
}

auto TimeSystem::getTimeDelta() const -> double {
  return m_timeDelta;
}

auto TimeSystem::onUpdate() -> void {
  const double newTime = glfwGetTime();

  m_timeDelta = newTime - m_time;
  m_time = newTime;
}

}