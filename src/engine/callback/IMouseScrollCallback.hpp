#ifndef ENGINE_CALLBACK_IMOUSE_SCROLL_CALLBACK_HPP
#define ENGINE_CALLBACK_IMOUSE_SCROLL_CALLBACK_HPP

namespace engine {

class IMouseScrollCallback {
public:
  virtual ~IMouseScrollCallback() {}

  virtual auto onScrollEvent(double offsetX, double offsetY) -> void = 0;
};

}

#endif