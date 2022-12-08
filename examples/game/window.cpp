#include "window.hpp"

void Window::onEvent(SDL_Event const &event) {
  //mantive apenas os eventos que vamos usar 
  if (event.type == SDL_KEYDOWN) {
    if (m_gameData.m_direction != Direction::Right && (event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_a))
      m_gameData.m_direction = Direction::Left;

    if (m_gameData.m_direction != Direction::Left && (event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_d))
      m_gameData.m_direction = Direction::Right;
  }
}

void Window::onCreate() {
  auto const m_assetsPath{abcg::Application::getAssetsPath()};

  m_program = abcg::createOpenGLProgram({{.source = m_assetsPath + "./shaders/main.vert", .stage = abcg::ShaderStage::Vertex}, {.source = m_assetsPath + "./shaders/main.frag", .stage = abcg::ShaderStage::Fragment}});
  m_playerProgram = abcg::createOpenGLProgram({{.source = m_assetsPath + "./shaders/player.vert", .stage = abcg::ShaderStage::Vertex}, {.source = m_assetsPath + "./shaders/player.frag", .stage = abcg::ShaderStage::Fragment}});

  //cor azul de fundo
  glClearColor(0.0f, 0.7f, 1.0f, 1.0f);
  
  abcg::glEnable(GL_DEPTH_TEST);

  //inicializa o VAO para criar 1 triangulo por enquanto, mas não é a renderização!!
  m_player.create(m_playerProgram);
}

void Window::onPaint() {
  abcg::glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
  abcg::glViewport(0, 0, m_viewportSize.x, m_viewportSize.y);

  //aqui faz aparecer o triangulo de vdd na tela, usa oq inicializamos no init()
  m_player.paint();  
}

void Window::onResize(glm::ivec2 const &size) {
  m_viewportSize = size;
}

void Window::onDestroy() {
  abcg::glDeleteProgram(m_program);
}