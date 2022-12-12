#ifndef PLAYER_HPP_
#define PLAYER_HPP_

#include "abcgOpenGL.hpp"
#include <glm/glm.hpp>
#include "camera.hpp"
#include "gamedata.hpp"

class Player {
public:
  void create(GLuint program);
  void paint(glm::vec3 scale, glm::vec3 rotation);
  void update(GameData m_gameData);
  void destroy();

  glm::vec3 m_pos{0.f, -1.2f, -2.f};
 
private:
  GLuint m_VAO{};
  
  GLuint m_program{};
  Camera m_camera;
  
  unsigned int m_texture;

  void setVAO();

  void loadTexture();

  void setMovement(GameData m_gameData);
};

#endif