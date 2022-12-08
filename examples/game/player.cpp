#include "player.hpp"

void Player::paint() {
	abcg::glUseProgram(m_program);
	glBindVertexArray(m_VAO);
  glBindTexture(GL_TEXTURE_2D, m_texture);
	glDrawArrays(GL_TRIANGLES, 0, 3);
  glBindVertexArray(0);
  abcg::glUseProgram(0);
}

void Player::create(GLuint program) {
  m_program = program;
  setVAO();
  loadTexture();
}

void Player::destroy(){
  abcg::glDeleteProgram(m_program);
  abcg::glDeleteBuffers(1, &m_VBOPositions);
  abcg::glDeleteBuffers(1, &m_VBOColors);
  abcg::glDeleteVertexArrays(1, &m_VAO);
}

void Player::setVAO() {

   float v[] = {
    -0.5f, -0.5f, 0.0f, 0, 0,
     0.5f, -0.5f, 0.0f, 1, 0,
     0.0f,  0.5f, 0.0f, 0.5, 1,
  };

  unsigned int VBO;
	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(v), v, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);
}

void Player::loadTexture(){
  auto const m_assetsPath{abcg::Application::getAssetsPath()};
  
  GLuint data = abcg::loadOpenGLTexture({.path = m_assetsPath + "./texture/wall.jpg"});
  if(data){
    abcg::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    abcg::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    abcg::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    abcg::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    m_texture = data;
  }
}

