#include "window.hpp"
#include <glm/glm.hpp>

void Window::onEvent(SDL_Event const &event) {
  if (event.type == SDL_KEYDOWN) {
    if (event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_a)
      m_gameData.m_direction.set(gsl::narrow<size_t>(Direction::Left));
    if (event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_d)
      m_gameData.m_direction.set(gsl::narrow<size_t>(Direction::Right));
    if (event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_w)
      m_gameData.m_direction.set(gsl::narrow<size_t>(Direction::Up));
    if (event.key.keysym.sym == SDLK_DOWN || event.key.keysym.sym == SDLK_s)
      m_gameData.m_direction.set(gsl::narrow<size_t>(Direction::Down));
  }
  if (event.type == SDL_KEYUP) {
    if (event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_a)
      m_gameData.m_direction.reset(gsl::narrow<size_t>(Direction::Left));
    if (event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_d)
      m_gameData.m_direction.reset(gsl::narrow<size_t>(Direction::Right));
    if (event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_w)
      m_gameData.m_direction.reset(gsl::narrow<size_t>(Direction::Up));
    if (event.key.keysym.sym == SDLK_DOWN || event.key.keysym.sym == SDLK_s)
      m_gameData.m_direction.reset(gsl::narrow<size_t>(Direction::Down));
  }
}

void Window::onCreate() {
  auto const m_assetsPath{abcg::Application::getAssetsPath()};

  m_starsProgram = abcg::createOpenGLProgram({{.source = m_assetsPath + "./shaders/stars.vert", .stage = abcg::ShaderStage::Vertex}, {.source = m_assetsPath + "./shaders/stars.frag", .stage = abcg::ShaderStage::Fragment}});
  m_program = abcg::createOpenGLProgram({{.source = m_assetsPath + "./shaders/main.vert", .stage = abcg::ShaderStage::Vertex}, {.source = m_assetsPath + "./shaders/main.frag", .stage = abcg::ShaderStage::Fragment}});
  m_playerProgram = abcg::createOpenGLProgram({{.source = m_assetsPath + "./shaders/player.vert", .stage = abcg::ShaderStage::Vertex}, {.source = m_assetsPath + "./shaders/player.frag", .stage = abcg::ShaderStage::Fragment}});
  m_obstacleProgram = abcg::createOpenGLProgram({{.source = m_assetsPath + "./shaders/obstacles.vert", .stage = abcg::ShaderStage::Vertex}, {.source = m_assetsPath + "./shaders/obstacles.frag", .stage = abcg::ShaderStage::Fragment}});

  glClearColor(17.0f/255.0f, 21.0f/255.0f, 28.0f/255.0f, 0);
  
  abcg::glEnable(GL_DEPTH_TEST);
  
  m_starLayers.create(m_starsProgram, 100);
  m_player.create(m_playerProgram);
  m_obstacle.create(m_obstacleProgram);
}

void Window::onPaint() {
  // interface update are based on time elapsed instead of hardware
  if (m_deltaTime.elapsed() > 1) {
    return;
  }

  // restart timer when entered
  m_deltaTime.restart();

  // clear window and set the viewport
  glClearColor(17.0f/255.0f, 21.0f/255.0f, 28.0f/255.0f, 0);
  abcg::glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
  abcg::glViewport(0, 0, m_viewportSize.x, m_viewportSize.y);

  //dps temos q trocar o paint etc do layer pro modelo escolhido!!
  m_player.paint(glm::vec3(0.8f), glm::vec3(0, 0, 0));  
  m_starLayers.paint();

  // obstacle creation has its own timer, create obstacles every 1 seconds
  if (m_obstacleTime.elapsed() > 1) {
    createObstacle();
    m_obstacleTime.restart();
  }

  //renderizacao dos obstaculos e incremento da pos z para avançar pro player  
  for(int i = 0; i < m_gameData.m_obstaclesCount; i++){
    m_gameData.m_obstaclesPositions[i].z += 0.01;
    m_obstacle.paint(m_gameData.m_obstaclesPositions[i], glm::vec3(1.f), glm::vec3(0.f));
  }
}

void Window::onUpdate() {
  auto const deltaTime{gsl::narrow_cast<float>(getDeltaTime())};
  m_starLayers.update(deltaTime);
  
  // interface update are based on time elapsed instead of hardware
  if (m_updateTime.elapsed() > 1) {
    return;
  }

  // restart timer when entered
  m_updateTime.restart();
  m_player.update(m_gameData);
}

void Window::onResize(glm::ivec2 const &size) {
  m_viewportSize = size;
}

void Window::onDestroy() {
  abcg::glDeleteProgram(m_program);
  abcg::glDeleteProgram(m_playerProgram);
  abcg::glDeleteProgram(m_starsProgram);
  abcg::glDeleteProgram(m_obstacleProgram);
  m_starLayers.destroy();
  m_player.destroy();
  m_obstacle.destroy();
}

void Window::createObstacle(){
  float x = -5 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (5 + 5)));
  m_gameData.m_obstaclesPositions.push_back(glm::vec3(x, -1.2f, -70.f));
  m_gameData.m_obstaclesCount++;
}