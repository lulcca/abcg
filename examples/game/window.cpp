#include "window.hpp"
#include <glm/glm.hpp>

void Window::onEvent(SDL_Event const &event) {
  if (event.type == SDL_KEYDOWN) {
    if (event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_a)
      m_gameData.m_direction.set(gsl::narrow<size_t>(Direction::Left));
    if (event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_d)
      m_gameData.m_direction.set(gsl::narrow<size_t>(Direction::Right));
  }
  if (event.type == SDL_KEYUP) {
    if (event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_a)
      m_gameData.m_direction.reset(gsl::narrow<size_t>(Direction::Left));
    if (event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_d)
      m_gameData.m_direction.reset(gsl::narrow<size_t>(Direction::Right));
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
  abcg::glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
  abcg::glViewport(0, 0, m_viewportSize.x, m_viewportSize.y);

  //dps ao temos q trocar o paint etc do layer pro modelo escolhido!!
  m_player.paint(glm::vec3(0.8f), glm::vec3(0, 0, 0));  
  m_starLayers.paint();
  lastObstacleCreated += m_deltaTime.elapsed();
  if(lastObstacleCreated >= 10.f && m_obstacleCount < 3){
    createObstacle();
    lastObstacleCreated = 0.f;
  }

  for(int i =0; i < m_obstacleCount; i++){
    // fmt::print("{}\n",i);
     //m_player.paint(glm::vec3(0.f, -2.f, -2.f), glm::vec3(1.f), glm::vec3(0, m_deltaTime.elapsed()*10*i, 0));  
     //m_obstacle.paint(glm::vec3(0.f, -2.f, -2.f), glm::vec3(1.f), glm::vec3(0, m_deltaTime.elapsed()*100*i, 0)); 
  }

}

void Window::onUpdate() {
  auto const deltaTime{gsl::narrow_cast<float>(getDeltaTime())};
  m_starLayers.update(deltaTime);
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
  m_obstacleCount = m_obstacleCount + 1;
}