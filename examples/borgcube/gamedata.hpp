#ifndef GAMEDATA_HPP_
#define GAMEDATA_HPP_

#include <bitset>
#include "obstacle.hpp"

using std::vector;

//Estados possíveis para o jogo
enum class State { Playing, GameOver };

//Direções possíveis para realização de movimentos durante o jogo
enum class Direction { Left, Right, Up, Down };

struct GameData {
  //O estado do jogo é iniciado como "playing"
  State m_state{State::Playing};

  //Vetor das 4 possíveis direções do jogo
  std::bitset<4> m_direction; 

  //Vetor das posições de cada obstáculo do jogo
  vector<glm::vec3> m_obstaclesPositions;

  //Quantidade de obstaculos criados em jogo, inicialmente o valor é 0
  int m_obstaclesCount{0};

  //Quantidade de colisões que o player já recebeu, inicialmente o valor é 0
  int m_hit{0};

  //Índice do último obstáculo que colidiu com o player, inicialmente o valor é -1 pois trata-se de um índice inválido
  int m_lastHitIndex{-1};
};

#endif