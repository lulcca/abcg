#include "window.hpp"
#include <glm/glm.hpp>

using std::string;

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

  auto const filename{m_assetsPath + "Inconsolata-Medium.ttf"};
  m_font = ImGui::GetIO().Fonts->AddFontFromFileTTF(filename.c_str(), 30.0f);
  if (m_font == nullptr) {
    throw abcg::RuntimeError{"Cannot load font file"};
  }

  m_program = abcg::createOpenGLProgram({{.source = m_assetsPath + "./shaders/main.vert", .stage = abcg::ShaderStage::Vertex}, {.source = m_assetsPath + "./shaders/main.frag", .stage = abcg::ShaderStage::Fragment}});
  m_playerProgram = abcg::createOpenGLProgram({{.source = m_assetsPath + "./shaders/obj.vert", .stage = abcg::ShaderStage::Vertex}, {.source = m_assetsPath + "./shaders/obj.frag", .stage = abcg::ShaderStage::Fragment}});
  m_obstacleProgram = abcg::createOpenGLProgram({{.source = m_assetsPath + "./shaders/obj.vert", .stage = abcg::ShaderStage::Vertex}, {.source = m_assetsPath + "./shaders/obj.frag", .stage = abcg::ShaderStage::Fragment}});

  glClearColor(17.0f/255.0f, 21.0f/255.0f, 28.0f/255.0f, 0);
  
  abcg::glEnable(GL_DEPTH_TEST);

  m_player.create(m_playerProgram);
  m_obstacle.create(m_obstacleProgram);
}

void Window::onPaint() {
  if (m_gameData.m_state == State::Playing) {
    
    // interface update are based on time elapsed instead of hardware
    if (m_deltaTime.elapsed() < 0.01) {
      return;
    }

    // restart timer when entered
    m_deltaTime.restart();

      // clear window and set the viewport
    glClearColor(17.0f/255.0f, 21.0f/255.0f, 28.0f/255.0f, 0);
    abcg::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    abcg::glViewport(0, 0, m_viewportSize.x, m_viewportSize.y);

    // paint player if its not colliding
    if(
      (m_collisionTime.elapsed() >= 0.0 && m_collisionTime.elapsed() < 0.1) || 
      (m_collisionTime.elapsed() >= 0.2 && m_collisionTime.elapsed() < 0.3) ||
      (m_collisionTime.elapsed() >= 0.4 && m_collisionTime.elapsed() < 0.5)
    ) {
      // we do not do anything in this case
    } else {

      // we paint the player
      m_player.paint(glm::vec3(0.8f), glm::vec3(0, 0, 0));  
    }
    
    // obstacle creation has its own timer, create obstacles every 1 seconds
    if (m_obstacleTime.elapsed() > 1) {
      createObstacle();
      m_obstacleTime.restart();
    }

    //renderizacao dos obstaculos e incremento da pos z para avançar pro player  
    for(int i = 0; i < m_gameData.m_obstaclesCount; i++){
      m_gameData.m_obstaclesPositions[i].z += 0.5;
      m_obstacle.paint(m_gameData.m_obstaclesPositions[i], glm::vec3(1.f), glm::vec3(1000 * m_gameTime.elapsed()));
    }
  } else if (m_gameData.m_state == State::GameOver){
    // clear window and set the viewport
    glClearColor(17.0f/255.0f, 21.0f/255.0f, 28.0f/255.0f, 0);
    abcg::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    abcg::glViewport(0, 0, m_viewportSize.x, m_viewportSize.y);

    if(m_deltaTime.elapsed() > 3){
      restart();
    }
  }
}

void Window::restart() {
  m_gameData.m_hit = 0;
  m_gameData.m_obstaclesPositions.clear();
  m_gameData.m_obstaclesCount = 0;
  m_gameData.m_lastHitIndex = -1;
  m_gameData.m_state = State::Playing;
  m_deltaTime.restart();
  m_obstacleTime.restart();
}

void Window::onUpdate() {
  if (m_gameData.m_state == State::Playing){
      
  // interface update are based on time elapsed instead of hardware
    if (m_updateTime.elapsed() < 0.01) {
      return;
    }

    m_player.update(m_gameData);
    m_updateTime.restart();
    checkCollision();
    checkDeath();

  } else if (m_gameData.m_state == State::GameOver){
    m_updateTime.restart();
  }
}


void Window::onPaintUI() {
  abcg::OpenGLWindow::onPaintUI();

  {
    auto const size{ImVec2(300, 85)};
    ImGui::SetNextWindowSize(size);
    ImGuiWindowFlags const flags{ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoInputs};
    ImGui::PushFont(m_font);
        
    if(m_gameData.m_state == State::GameOver){
      string status = "GameOver";

      auto const position{ImVec2((m_viewportSize.x - size.x) / 2.0f, (m_viewportSize.y - size.y) / 2.0f)};
      ImGui::SetNextWindowPos(position);
      float font_size = ImGui::GetFontSize() * status.size() / 2;
      ImGui::Begin(" ", nullptr, flags);
      ImGui::SameLine(ImGui::GetWindowSize().x / 2 - font_size + (font_size / 2));
      ImGui::Text("%s", status.c_str());
      ImGui::PopFont();
      ImGui::End();
    
    }
  }
}

void Window::onResize(glm::ivec2 const &size) {
  m_viewportSize = size;
  abcg::glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
}

//Liberação dos recursos alocados durante a aplicação
void Window::onDestroy() {
  abcg::glDeleteProgram(m_program);
  abcg::glDeleteProgram(m_playerProgram);
  abcg::glDeleteProgram(m_obstacleProgram);
  m_player.destroy();
  m_obstacle.destroy();
}

//Criação de obstáculos, a posição x é definida de forma aleatória em uma faixa de valores de -5 a 5
void Window::createObstacle() {
  float x = -5 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (5 + 5)));
  m_gameData.m_obstaclesPositions.push_back(glm::vec3(x, -1.2f, -70.f));
  m_gameData.m_obstaclesCount++;
}

//Valida se houve colisão entre o player ou algum objeto, para isso percorremos o vetor de posição dos obstáculos e comparamos as respectivas posições x e z 
//com as do player. Logo em seguida, incrementamos a quantidade de colisões e reiniciamos o timer da colisão, que será utilizado futuramente no método paint() para demonstrar que o player foi acertado
void Window::checkCollision() {
  for (int i = 0; i < m_gameData.m_obstaclesCount; i++){
    glm::vec3 currentPosition = m_gameData.m_obstaclesPositions[i];
    if(currentPosition.x < m_player.m_pos.x + 1.f && currentPosition.x > m_player.m_pos.x - 1.f && currentPosition.z < m_player.m_pos.z + 1.f && currentPosition.z > m_player.m_pos.z - 1.f &&  m_gameData.m_lastHitIndex != i){
      m_gameData.m_hit++;
      m_gameData.m_lastHitIndex = i;
      m_collisionTime.restart();
    }
  }
}

//Valida a quantidade de colisões que o player sofreu, caso seja maior do que 2 então o estado do jogo será alterado para GameOver e sua posição resetada
void Window::checkDeath() {
  if(m_gameData.m_hit > 2){
    m_gameData.m_state = State::GameOver;
    m_player.m_pos = glm::vec3({0.f, -1.2f, -2.f});
  }
}