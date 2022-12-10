#ifndef PLAYER_HPP_
#define PLAYER_HPP_

#include "abcgOpenGL.hpp"
#include <glm/glm.hpp>
#include "camera.hpp"
#include "gamedata.hpp"
#include "model.hpp"

class Player {
public:
  void create(GLuint program);
  void paint(glm::vec3 scale, glm::vec3 rotation);
  void update(GameData m_gameData);
  void destroy();

  glm::vec3 m_pos{0.f, -1.2f, -2.f};
 
private:
  GLuint m_VAO{};
  GLuint m_VBOPositions{};
  GLuint m_VBOColors{};
  Model m_model;
  GLuint m_program{};
  Camera m_camera;
  
  unsigned int m_texture;

  glm::mat4 m_modelMatrix{1.0f};
  glm::mat4 m_viewMatrix{1.0f};
  glm::mat4 m_projMatrix{1.0f};

  glm::vec4 m_lightDir{-1.0f, -1.0f, -1.0f, 0.0f};
  glm::vec4 m_Ia{1.0f};
  glm::vec4 m_Id{1.0f};
  glm::vec4 m_Is{1.0f};
  glm::vec4 m_Ka{0.1f, 0.1f, 0.1f, 1.0f};
  glm::vec4 m_Kd{0.7f, 0.7f, 0.7f, 1.0f};
  glm::vec4 m_Ks{1.0f};
  float m_shininess{25.0f};

  void loadModel();
  void setMovement(GameData m_gameData);
};

#endif