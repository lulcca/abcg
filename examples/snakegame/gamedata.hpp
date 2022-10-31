#ifndef GAMEDATA_HPP_
#define GAMEDATA_HPP_

#include <bitset>

enum class State { Playing, GameOver, Win };
enum class Direction { Up, Down, Left, Right };

struct GameData {
  State m_state{State::Playing};
  Direction m_direction{Direction::Right};
};

#endif