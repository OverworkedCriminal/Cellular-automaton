#ifndef ENGINE_WINDOW_WINDOW_HPP
#define ENGINE_WINDOW_WINDOW_HPP

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include <expected>
#include <string>

namespace engine {

class Window {
public:
  /**
   * @brief
   * Initialize window and make it current.
   * Window is automatically closed in destructor.
   * 
   * @param title 
   * @param width 
   * @param height 
   * @return std::expected<Window, std::string> 
   */
  static auto open(
    const std::string& title,
    int width,
    int height
  ) -> std::expected<Window, std::string>;

  Window(const Window&) = delete;
  Window(Window&&);
  ~Window();

  auto operator=(const Window&) -> Window& = delete;
  auto operator=(Window&&) -> Window&;

  auto operator*() -> GLFWwindow*;

private:
  Window(GLFWwindow* window);

  GLFWwindow* m_window;
};

}


#endif