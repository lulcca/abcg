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
  
  glm::ivec2 m_viewportSize{};

  ImFont *m_font{};
  
  abcg::Timer m_gameTime;
  abcg::Timer m_deltaTime;
  abcg::Timer m_updateTime; 
  abcg::Timer m_obstacleTime;
  abcg::Timer m_starTime;
  abcg::Timer m_collisionTime;

  GLuint m_program{};
  GLuint m_playerProgram{};
  GLuint m_obstacleProgram{};
  GLuint m_starsProgram{};

  void createObstacle();
  void checkCollision();
  void checkDeath();
  void restart();
};

#endif