#ifndef WINDOW_HPP_
#define WINDOW_HPP_

#include "abcgOpenGL.hpp"
#include "gamedata.hpp"
#include "player.hpp"
#include <string>

class Window : public abcg::OpenGLWindow {
protected:
  void onEvent(SDL_Event const &event) override;
  void onCreate() override;
  void onPaint() override;
  void onResize(glm::ivec2 const &size) override;
  void onDestroy() override;

private:
  GameData m_gameData;
  
  Player m_player;

  glm::ivec2 m_viewportSize{};

  GLuint m_VAO{};
  GLuint m_VBOColors{};
  GLuint m_VBOPositions{};

  GLuint m_program{};
  GLuint m_playerProgram{};
};

#endif