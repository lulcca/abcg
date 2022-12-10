#ifndef WINDOW_HPP_
#define WINDOW_HPP_

#include "abcgOpenGL.hpp"
#include "gamedata.hpp"
#include "player.hpp"
#include "obstacle.hpp"
#include "starlayers.hpp"
#include <string>

class Window : public abcg::OpenGLWindow {
protected:
  void onEvent(SDL_Event const &event) override;
  void onCreate() override;
  void onPaint() override;
  void onPaintUI() override;
  void onUpdate() override;
  void onResize(glm::ivec2 const &size) override;
  void onDestroy() override;

private:  
  Player m_player;
  Obstacle m_obstacle;
  StarLayers m_starLayers;
  GameData m_gameData;

  float obstacleTime;
  int m_obstacleCount{2};
  
  glm::ivec2 m_viewportSize{};

  ImFont *m_font{};
  
  abcg::Timer m_deltaTime;
  abcg::Timer m_updateTime;
  abcg::Timer m_obstacleTime;

  GLuint m_VAO{};
  GLuint m_VBOColors{};
  GLuint m_VBOPositions{};

  GLuint m_program{};
  GLuint m_playerProgram{};
  GLuint m_obstacleProgram{};
  GLuint m_starsProgram{};

  void createObstacle();
  void restart();
};

#endif