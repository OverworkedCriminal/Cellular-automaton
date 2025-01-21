#ifndef ENGINE_TIME_ITIME_SYSTEM_HPP
#define ENGINE_TIME_ITIME_SYSTEM_HPP

namespace engine::time {

class ITimeSystem {
public:
  virtual ~ITimeSystem() {}

  virtual auto getTime() const -> double = 0;
  virtual auto getTimeDelta() const -> double = 0;
};

}

#endif