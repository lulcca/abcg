#ifndef OBSTACLE_HPP_
#define OBSTACLE_HPP_

#include "abcgOpenGL.hpp"
#include <glm/glm.hpp>
#include "camera.hpp"

class Obstacle {
public:
  void create(GLuint program);
  void paint(glm::vec3 pos, glm::vec3 scale, glm::vec3 rotation);
  void destroy();
 
private:
  GLuint m_VAO{};
  
  Camera m_camera;
  GLuint m_program{};
  
  unsigned int m_texture;

  void setVAO();
  void loadTexture();
};

#endif