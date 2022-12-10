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
  auto const viewMatrixLoc{glGetUniformLocation(m_program, "projMatrix")};
  auto const projMatrixLoc{glGetUniformLocation(m_program, "viewMatrix")};
  auto const modelMatrixLoc{glGetUniformLocation(m_program, "modelMatrix")};

  glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, glm::value_ptr(projection));
  glUniformMatrix4fv(projMatrixLoc, 1, GL_FALSE, glm::value_ptr(view));
  glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, glm::value_ptr(model));

	// glBindVertexArray(m_VAO);
  // glBindTexture(GL_TEXTURE_2D, m_texture);
	// glDrawArrays(GL_TRIANGLES, 0, 36);
  // glBindVertexArray(0);
  // glUseProgram(0);


  auto const normalMatrixLoc{abcg::glGetUniformLocation(m_program, "normalMatrix")};
  auto const lightDirLoc{abcg::glGetUniformLocation(m_program, "lightDirWorldSpace")};
  auto const shininessLoc{abcg::glGetUniformLocation(m_program, "shininess")};
  auto const IaLoc{abcg::glGetUniformLocation(m_program, "Ia")};
  auto const IdLoc{abcg::glGetUniformLocation(m_program, "Id")};
  auto const IsLoc{abcg::glGetUniformLocation(m_program, "Is")};
  auto const KaLoc{abcg::glGetUniformLocation(m_program, "Ka")};
  auto const KdLoc{abcg::glGetUniformLocation(m_program, "Kd")};
  auto const KsLoc{abcg::glGetUniformLocation(m_program, "Ks")};
  auto const diffuseTexLoc{abcg::glGetUniformLocation(m_program, "diffuseTex")};
  auto const mappingModeLoc{abcg::glGetUniformLocation(m_program, "mappingMode")};

  // set uniform variables that have the same value for every model
  abcg::glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, &view[0][0]);
  abcg::glUniformMatrix4fv(projMatrixLoc, 1, GL_FALSE, &projection[0][0]);
  abcg::glUniform1i(diffuseTexLoc, 0);
  abcg::glUniform1i(mappingModeLoc, 3);

  auto const lightDirRotated{m_modelMatrix * m_lightDir};
  abcg::glUniform4fv(lightDirLoc, 1, &lightDirRotated.x);
  abcg::glUniform4fv(IaLoc, 1, &m_Ia.x);
  abcg::glUniform4fv(IdLoc, 1, &m_Id.x);
  abcg::glUniform4fv(IsLoc, 1, &m_Is.x);

  // set uniform variables for the current model
  abcg::glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &model[0][0]);
  abcg::glUniform4fv(KaLoc, 1, &m_Ka.x);
  abcg::glUniform4fv(KdLoc, 1, &m_Kd.x);
  abcg::glUniform4fv(KsLoc, 1, &m_Ks.x);
  abcg::glUniform1f(shininessLoc, m_shininess);

  auto const modelViewMatrix{glm::mat3(m_viewMatrix * model)};
  auto const normalMatrix{glm::inverseTranspose(modelViewMatrix)};
  abcg::glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, &normalMatrix[0][0]);

  // rendering the model
  m_model.render();
}

void Player::update(GameData m_gameData){
  setMovement(m_gameData);
}

void Player::setMovement(GameData m_gameData){
  float step = 0.1f;
  float newXPosition = m_pos.x;
  float newZPosition = m_pos.z;

  if (m_gameData.m_direction[gsl::narrow<size_t>(Direction::Left)]){
    newXPosition = newXPosition - step < -4.5f ? -4.5f : newXPosition - step;
  }
  if (m_gameData.m_direction[gsl::narrow<size_t>(Direction::Right)]){
    newXPosition =newXPosition + step > 4.5f ? 4.5f :  newXPosition + step;
  }
  if (m_gameData.m_direction[gsl::narrow<size_t>(Direction::Up)]){
    newZPosition = newZPosition - 2 * step < -10.f ? -10.f :  newZPosition - 2 * step;
  }
  if (m_gameData.m_direction[gsl::narrow<size_t>(Direction::Down)]){
    newZPosition = newZPosition + 2 * step > -2.f ? -2.f :  newZPosition + 2 * step;
  }
  m_pos = glm::vec3(newXPosition, m_pos.y, newZPosition);
}

void Player::create(GLuint program) {
  m_program = program;
  loadModel();
}

void Player::destroy(){
  glDeleteProgram(m_program);
  m_model.destroy();
}

void Player::loadModel(){
  auto const m_assetsPath{abcg::Application::getAssetsPath()};
  m_model.destroy();

  // load texture (earth.jpg)
  m_model.loadDiffuseTexture(m_assetsPath + "./textures/earth.jpg");

  // load object (earth.obj)
  m_model.loadObj(m_assetsPath + "./models/earth.obj");
  m_model.setupVAO(m_program);

  // use material properties from the loaded model
  m_Ka = m_model.getKa();
  m_Kd = m_model.getKd();
  m_Ks = m_model.getKs();
  m_shininess = m_model.getShininess();
}

