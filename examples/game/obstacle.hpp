#ifndef Obstacle_HPP_
#define Obstacle_HPP_

#include "abcgOpenGL.hpp"
#include <glm/glm.hpp>

class Obstacle {
public:
  Obstacle(glm::vec3 pos);
  glm::vec3 pos;
};

#endif