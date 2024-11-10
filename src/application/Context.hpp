#ifndef APPLICATION_CONTEXT_HPP
#define APPLICATION_CONTEXT_HPP

#include "application/simulation/Direction.hpp"
#include "engine/graphics/texture/Texture.hpp"
#include <memory>

struct Context {
  /**
   * Output texture used to draw simulation on screen
   */
  std::shared_ptr<engine::Texture> texturePtr;

  /**
   * Which direction should be processed first
   */
  Direction priorityDirection;
};

#endif