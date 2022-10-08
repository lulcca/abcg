#ifndef WINDOW_HPP_
#define WINDOW_HPP_

#include "abcgOpenGL.hpp"

class Window : public abcg::OpenGLWindow {
protected:
  void onCreate() override;
  void onPaintUI() override;

public:
  int m_N;
  std::vector<char> m_board;

private:
  std::vector<char> easy_sequence{'1', '2', '3', '4', '5', '6', '7', '8', ' '};
  std::vector<char> hard_sequence{'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', ' '};
  enum class GameState { Play, Won };
  GameState m_gameState;

  ImFont *m_font{};

  void winCheck();
  void shuffleBoard();
};

#endif
