#include "window.hpp"
#include <glm/gtx/fast_trigonometry.hpp>

void Window::onEvent(SDL_Event const &event) {
  // mouse events
  if (event.type == SDL_MOUSEWHEEL) {
    // zoom in or zoom out after scroll
    m_zoom += (event.wheel.y > 0 ? -1.0f : 1.0f) / 5.0f;
    m_zoom = glm::clamp(m_zoom, -1.5f, 1.0f);
  }
}

void Window::onCreate() {
  // set assets path
  m_assetsPath = abcg::Application::getAssetsPath();

  // clear window
  abcg::glClearColor(0, 0, 0, 1);

  // enable depth buffering
  abcg::glEnable(GL_DEPTH_TEST);

  // create an OpenGl program using earth.vert and earth.frag
  m_program = abcg::createOpenGLProgram({{.source = m_assetsPath + "earth.vert", .stage = abcg::ShaderStage::Vertex}, {.source = m_assetsPath + "earth.frag", .stage = abcg::ShaderStage::Fragment}});
  
  // load model using earth.obj
  loadModel(m_assetsPath + "earth.obj");
}

void Window::loadModel(std::string_view path) {
  // call destroy method to release opengl resources that were allocated during application
  m_model.destroy();

  // load texture (earth.jpg)
  m_model.loadDiffuseTexture(m_assetsPath + "earth.jpg");

  // load object (earth.obj)
  m_model.loadObj(path);
  m_model.setupVAO(m_program);

  // use material properties from the loaded model
  m_Ka = m_model.getKa();
  m_Kd = m_model.getKd();
  m_Ks = m_model.getKs();
  m_shininess = m_model.getShininess();
}

void Window::onUpdate() {
  auto const deltaTime{gsl::narrow_cast<float>(getDeltaTime())};

  // m_angle is incremented by 5 radians and current deltaTime
  m_angle = glm::wrapAngle(m_angle + glm::radians(5.0f) * deltaTime);

  // Compute model matrix of the earth using m_angle, m_axis and m_rotation
  m_modelMatrix = glm::rotate(glm::mat4(1.0f), m_angle, m_axis) * m_rotation;

  // Compute view matrix of the earth using m_zoom
  m_viewMatrix = glm::lookAt(glm::vec3(0.0f, 0.0f, 2.0f + m_zoom), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
}

void Window::onPaint() {
  // clear window
  abcg::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // set viewport with m_viewportSize value
  abcg::glViewport(0, 0, m_viewportSize.x, m_viewportSize.y);

  // enable face culling
  abcg::glEnable(GL_CULL_FACE);

  // indicate that the front side has counterclockwise vertices
  abcg::glFrontFace(GL_CCW);

  // set m_projMatrix as perspective projection 
  auto const aspect{gsl::narrow<float>(m_viewportSize.x) / gsl::narrow<float>(m_viewportSize.y)};
  m_projMatrix = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 5.0f);

  // get location of uniform variables
  auto const viewMatrixLoc{abcg::glGetUniformLocation(m_program, "viewMatrix")};
  auto const projMatrixLoc{abcg::glGetUniformLocation(m_program, "projMatrix")};
  auto const modelMatrixLoc{abcg::glGetUniformLocation(m_program, "modelMatrix")};
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
  abcg::glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, &m_viewMatrix[0][0]);
  abcg::glUniformMatrix4fv(projMatrixLoc, 1, GL_FALSE, &m_projMatrix[0][0]);
  abcg::glUniform1i(diffuseTexLoc, 0);
  abcg::glUniform1i(mappingModeLoc, 3);

  auto const lightDirRotated{m_modelMatrix * m_lightDir};
  abcg::glUniform4fv(lightDirLoc, 1, &lightDirRotated.x);
  abcg::glUniform4fv(IaLoc, 1, &m_Ia.x);
  abcg::glUniform4fv(IdLoc, 1, &m_Id.x);
  abcg::glUniform4fv(IsLoc, 1, &m_Is.x);

  // set uniform variables for the current model
  abcg::glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &m_modelMatrix[0][0]);
  abcg::glUniform4fv(KaLoc, 1, &m_Ka.x);
  abcg::glUniform4fv(KdLoc, 1, &m_Kd.x);
  abcg::glUniform4fv(KsLoc, 1, &m_Ks.x);
  abcg::glUniform1f(shininessLoc, m_shininess);

  auto const modelViewMatrix{glm::mat3(m_viewMatrix * m_modelMatrix)};
  auto const normalMatrix{glm::inverseTranspose(modelViewMatrix)};
  abcg::glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, &normalMatrix[0][0]);

  // rendering the model
  m_model.render();

  // activate shaders
  abcg::glUseProgram(m_program);
}

void Window::onResize(glm::ivec2 const &size) {
  // set viewport based on window size
  m_viewportSize = size;
}

void Window::onDestroy() {
  // release opengl resources that were allocated during application
  m_model.destroy();
  abcg::glDeleteProgram(m_program);
}