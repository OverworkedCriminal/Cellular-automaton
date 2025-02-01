#ifndef ENGINE_CALLBACK_CALLBACK_HPP
#define ENGINE_CALLBACK_CALLBACK_HPP

#include <memory>
#include <vector>

namespace engine {

template<typename T>
auto removeDeadCallbacks(std::vector<std::weak_ptr<T>>& callbacks) -> void {
  const uint32_t callbackCount = callbacks.size();
  unsigned deadCallbackCount = 0;

  for (uint32_t i = 0; i < callbackCount - deadCallbackCount; ++i) {
    const auto& callbackWeakPtr = callbacks[i];
    if (callbackWeakPtr.expired()) {
      std::swap(
        callbacks[i],
        callbacks[callbackCount - 1 - deadCallbackCount]
      );

      ++deadCallbackCount;
      --i;
    }
  }

  callbacks.resize(callbackCount - deadCallbackCount);
}

}


#endif