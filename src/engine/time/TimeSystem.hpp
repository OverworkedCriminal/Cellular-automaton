#ifndef ENGINE_TIME_TIME_SYSTEM_HPP
#define ENGINE_TIME_TIME_SYSTEM_HPP

#include "engine/time/ITimeSystem.hpp"

namespace engine::time {

class TimeSystem :public ITimeSystem {
public:
  static auto create() -> TimeSystem;

  TimeSystem(const TimeSystem&) = delete;
  TimeSystem(TimeSystem&&) = default;

  auto operator=(const TimeSystem&) -> TimeSystem& = delete;
  auto operator=(TimeSystem&&) -> TimeSystem& = default;

  auto getTime() const -> double override;
  auto getTimeDelta() const -> double override;

  auto onUpdate() -> void;

private:
  TimeSystem(double time, double timeDelta);

  double m_time;
  double m_timeDelta;
};

}

#endif