#ifndef PLAYER_HPP_
#define PLAYER_HPP_

#include "abcgOpenGL.hpp"

class Player {
public:
  void create(GLuint program);
  void paint();
  void destroy();
 
private:
  GLuint m_VAO{};
  GLuint m_VBOPositions{};
  GLuint m_VBOColors{};
  
  GLuint m_program{};
  
  unsigned int m_texture;

  void setVAO();
  void loadTexture();
};

#endif