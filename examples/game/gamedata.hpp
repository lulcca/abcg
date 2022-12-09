#ifndef GAMEDATA_HPP_
#define GAMEDATA_HPP_

#include <bitset>
#include "obstacle.hpp"

using std::vector;

enum class State { Playing, GameOver };
enum class Direction { Left, Right, Up, Down };

struct GameData {
  State m_state{State::Playing};
  std::bitset<4> m_direction; 
  vector<glm::vec3> m_obstaclesPositions;
  int m_obstaclesCount{0};
};

#endif