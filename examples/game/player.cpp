#include "player.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//trocar pra model
void Player::paint(glm::vec3 scale, glm::vec3 rotation) {
	
  glm::mat4 projection = glm::perspective(glm::radians(45.f), 1280.f/720.f, 0.1f, 100.0f);
  glm::mat4 view = glm::translate(glm::mat4(1.0f), m_camera.pos);
  glm::mat4 model = glm::mat4(1.0f);
  model = glm::translate(model, m_pos);
  model = glm::scale(model, scale);
  model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1,0,0));
  model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0,1,0));
  model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0,0,1));

  glUseProgram(m_program);
  auto const viewMatrixLoc{glGetUniformLocation(m_program, "proj")};
  auto const projMatrixLoc{glGetUniformLocation(m_program, "view")};
  auto const modelMatrixLoc{glGetUniformLocation(m_program, "model")};

  glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, glm::value_ptr(projection));
  glUniformMatrix4fv(projMatrixLoc, 1, GL_FALSE, glm::value_ptr(view));
  glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, glm::value_ptr(model));

	glBindVertexArray(m_VAO);
  glBindTexture(GL_TEXTURE_2D, m_texture);
	glDrawArrays(GL_TRIANGLES, 0, 36);
  glBindVertexArray(0);
  glUseProgram(0);
}

void Player::update(GameData m_gameData){
  float newXPosition = m_pos.x;
  float newYPosition = m_pos.y;
  float step = 0.1f;

  if (m_gameData.m_direction[gsl::narrow<size_t>(Direction::Left)]){
    newXPosition = newXPosition - step < -4.5f ? -4.5f : newXPosition - step;
  }
  if (m_gameData.m_direction[gsl::narrow<size_t>(Direction::Right)]){
    newXPosition =newXPosition + step > 4.5f ? 4.5f :  newXPosition + step;
  }
  if (m_gameData.m_direction[gsl::narrow<size_t>(Direction::Up)]){
    newYPosition = newYPosition + step > 3.2f ? 3.2f :  newYPosition + step;
  }
  if (m_gameData.m_direction[gsl::narrow<size_t>(Direction::Down)]){
    newYPosition = newYPosition - step < -1.2f ? -1.2f :  newYPosition - step;
  }
  m_pos = glm::vec3(newXPosition, newYPosition, -2.f);
}

void Player::create(GLuint program) {
  m_program = program;
  setVAO();
  loadTexture();
}

void Player::destroy(){
  glDeleteProgram(m_program);
  glDeleteBuffers(1, &m_VBOPositions);
  glDeleteBuffers(1, &m_VBOColors);
  glDeleteVertexArrays(1, &m_VAO);
}

//trocar pra usar model
void Player::setVAO() {

    float v[] = {
        // Back face
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,        
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 
        // Front face
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 
        // Left face
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 
        -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 
        -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 
        // Right face
         0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,     
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 
         0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,   
        // Bottom face
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 
        // Top face
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f  
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

//trocar a textura pra model
void Player::loadTexture(){
  auto const m_assetsPath{abcg::Application::getAssetsPath()};
  
  GLuint data = abcg::loadOpenGLTexture({.path = m_assetsPath + "./texture/wall.jpg"});
  if(data){
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    m_texture = data;
  }
}

