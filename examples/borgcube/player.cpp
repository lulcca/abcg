#include "player.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//Realiza a renderização do player na tela
void Player::paint(glm::vec3 scale, glm::vec3 rotation) {
	

  glm::mat4 projection = glm::perspective(glm::radians(45.f), 1280.f/720.f, 0.1f, 100.0f);
  glm::mat4 view = glm::translate(glm::mat4(1.0f), m_camera.pos);
  glm::mat4 model = glm::mat4(1.0f);

  //Renderiza o player na posição atual
  model = glm::translate(model, m_pos);

  //Utilização da escala recebida para renderização do player
  model = glm::scale(model, scale);

  //Rotação para criar o player em formato de cubo
  model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1,0,0));
  model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0,1,0));
  model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0,0,1));

  //Ativa os shaders 
  glUseProgram(m_program);

  auto const viewMatrixLoc{glGetUniformLocation(m_program, "proj")};
  auto const projMatrixLoc{glGetUniformLocation(m_program, "view")};
  auto const modelMatrixLoc{glGetUniformLocation(m_program, "model")};


  glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, glm::value_ptr(projection));
  glUniformMatrix4fv(projMatrixLoc, 1, GL_FALSE, glm::value_ptr(view));
  glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, glm::value_ptr(model));

	glBindVertexArray(m_VAO);

  //Utiliza a textura carregada e atribuída na variável m_texture
  glBindTexture(GL_TEXTURE_2D, m_texture);

  //Renderiza de acordo com a quantidade de triângulos
	glDrawArrays(GL_TRIANGLES, 0, 36);

  //Desativa os shaders
  glBindVertexArray(0);
  glUseProgram(0);
}

void Player::update(GameData m_gameData){
  //Chamada para validar e atualizar o movimento do player
  setMovement(m_gameData);
}

void Player::setMovement(GameData m_gameData){
  //Valor utilizado para incrementar cada movimento do player
  float step = 0.1f;

  //Posição no eixo x
  float newXPosition = m_pos.x;

  //Posição no eixo y
  float newZPosition = m_pos.z;

  //Cada movimento só é realizado se não ultrapassar os limites correspondentes da tela

  //Movimento para a esquerda
  if (m_gameData.m_direction[gsl::narrow<size_t>(Direction::Left)]){
    newXPosition = newXPosition - step < -4.5f ? -4.5f : newXPosition - step;
  }
  //Movimento para a direita
  if (m_gameData.m_direction[gsl::narrow<size_t>(Direction::Right)]){
    newXPosition =newXPosition + step > 4.5f ? 4.5f :  newXPosition + step;
  }
  //Movimento para frente
  if (m_gameData.m_direction[gsl::narrow<size_t>(Direction::Up)]){
    newZPosition = newZPosition - 2 * step < -10.f ? -10.f :  newZPosition - 2 * step;
  }
  //Movimento para trás
  if (m_gameData.m_direction[gsl::narrow<size_t>(Direction::Down)]){
    newZPosition = newZPosition + 2 * step > -2.f ? -2.f :  newZPosition + 2 * step;
  }

  //Atribuição da nova posição do player
  m_pos = glm::vec3(newXPosition, m_pos.y, newZPosition);
}

void Player::create(GLuint program) {
  //Atribuição da variável m_program de acordo com o parâmetro recebido já com os shaders necessários
  m_program = program;

  //Atribuição da variável m_VAO
  setVAO();

  //Carregamento da textura
  loadTexture();
}

//Liberação dos recursos alocados durante a aplicação
void Player::destroy(){
  glDeleteProgram(m_program);
  glDeleteBuffers(1, &m_VBOPositions);
  glDeleteBuffers(1, &m_VBOColors);
  glDeleteVertexArrays(1, &m_VAO);
}

void Player::setVAO() {
    //Vetor dos vértices que serão utilizados para criar um cubo
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

//Carregamento da textura
void Player::loadTexture(){
  auto const m_assetsPath{abcg::Application::getAssetsPath()};
  
  GLuint data = abcg::loadOpenGLTexture({.path = m_assetsPath + "./texture/cubo.jpg"});
  if(data){
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    //Atribuição da variável m_textura que posterioremente será utilizada para renderizar o player
    m_texture = data;
  }
}

