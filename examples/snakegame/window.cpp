#include "window.hpp"

void Window::onCreate() {

  // load font with bigger size
  auto const filename{abcg::Application::getAssetsPath() +  "Inconsolata-Medium.ttf"};
  m_font = ImGui::GetIO().Fonts->AddFontFromFileTTF(filename.c_str(), 60.0f);
  
  // throw error if it fails
  if (m_font == nullptr) {
    throw abcg::RuntimeError{"Cannot load font file"};
  }

  // initializa shaders and create shader program
  auto const *vertexShader {
    R"gl(#version 300 es
    layout(location = 0) in vec2 inPosition;
    layout(location = 1) in vec4 inColor;

    out vec4 fragColor;

    void main() {
      gl_Position = vec4 (inPosition, 0, 1);
      fragColor = inColor;
    })gl"
  };

  auto const *fragmentShader {
    R"gl(#version 300 es
    precision mediump float;

    in vec4 fragColor;
    out vec4 outColor;

    void main() { outColor = fragColor; })gl"
  };

  m_program = abcg::createOpenGLProgram({{.source = vertexShader, .stage = abcg::ShaderStage::Vertex}, {.source = fragmentShader, .stage = abcg::ShaderStage::Fragment}});

  // clear window
  abcg::glClearColor(0, 0, 0, 1);
  abcg::glClear(GL_COLOR_BUFFER_BIT);

  // generate random seed
  auto const seed{std::chrono::steady_clock::now().time_since_epoch().count()};
  m_randomEngine.seed(seed);

  // call play function
  play();
}

void Window::onDestroy() {

  // release opengl resources that were allocated during application
  abcg::glDeleteProgram(m_program);
  abcg::glDeleteBuffers(1, &m_VBOColors);
  abcg::glDeleteBuffers(1, &m_VBOPositions);
  abcg::glDeleteVertexArrays(1, &m_VAO);
}

void Window::onEvent(SDL_Event const &event) {

  // keyboard events
  if (event.type == SDL_KEYDOWN) {

    // when direction of the snake is up, you can't go down
    if (m_gameData.m_direction != Direction::Down && (event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_w))
      m_gameData.m_direction = Direction::Up;

    // when direction of the snake is down, you can't go up
    if (m_gameData.m_direction != Direction::Up && (event.key.keysym.sym == SDLK_DOWN || event.key.keysym.sym == SDLK_s))
      m_gameData.m_direction = Direction::Down;

    // when direction of the snake is left, you can't go right
    if (m_gameData.m_direction != Direction::Right && (event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_a))
      m_gameData.m_direction = Direction::Left;

    // when direction of the snake is right, you can't go left
    if (m_gameData.m_direction != Direction::Left && (event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_d))
      m_gameData.m_direction = Direction::Right;
  }
}

void Window::onPaint() {

  // don't paint if you're not playing
  if (m_gameData.m_state != State::Playing) 
    return;

  // interface update are based on time elapsed instead of hardware
  if (m_timer.elapsed() < 0.1)
    return;

  // restart timer when entered
  m_timer.restart();

  // clear window
  abcg::glClearColor(0, 0, 0, 1);
  abcg::glClear(GL_COLOR_BUFFER_BIT);

  // set the viewport
  abcg::glViewport(0, 0, m_viewportSize.x, m_viewportSize.y);

  // activate shaders
  abcg::glUseProgram(m_program);

  // draw grid and borders
  drawGrid();

  // draw apple
  drawApple();

  // draw snake
  drawSnake();
  
  // disable shaders
  abcg::glUseProgram(0);

  // validate board
  validate();
}

void Window::onPaintUI() {

  // print messages on the center of the screen when state changes
  {
    auto const size{ImVec2(300, 85)};
    auto const position{ImVec2((m_viewportSize.x - size.x) / 2.0f, (m_viewportSize.y - size.y) / 2.0f)};
    ImGui::SetNextWindowPos(position);
    ImGui::SetNextWindowSize(size);
    ImGuiWindowFlags const flags{ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoInputs};
    ImGui::Begin(" ", nullptr, flags);
    ImGui::PushFont(m_font);

    if (m_gameData.m_state == State::GameOver) {
      ImGui::Text("Game Over!");
    } else if (m_gameData.m_state == State::Win) {
      ImGui::Text("You Win!");
    }

    ImGui::PopFont();
    ImGui::End();
  }
}

void Window::onResize(glm::ivec2 const &size) {

  // set viewport based on window size and clear color buffer
  m_viewportSize = size;
  abcg::glClear(GL_COLOR_BUFFER_BIT);
}

void Window::onUpdate() {

  // wait 5 seconds and restart game
  if (m_gameData.m_state != State::Playing && m_timer.elapsed() > 5) {
    play();

    return;
  }
}

void Window::setupModel() {

  // release previous VBO and VAO
  abcg::glDeleteBuffers(1, &m_VBOColors);
  abcg::glDeleteBuffers(1, &m_VBOPositions);
  abcg::glDeleteVertexArrays(1, &m_VAO);

  // generate VBO of colors
  abcg::glGenBuffers(1, &m_VBOColors);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBOColors);
  abcg::glBufferData(GL_ARRAY_BUFFER, sizeof(m_colors), m_colors.data(), GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // generate VBO of positions
  abcg::glGenBuffers(1, &m_VBOPositions);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBOPositions);
  abcg::glBufferData(GL_ARRAY_BUFFER, sizeof(m_positions), m_positions.data(), GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // get location of attributes in the program
  auto const colorAttribute{abcg::glGetAttribLocation(m_program, "inColor")};
  auto const positionAttribute{abcg::glGetAttribLocation(m_program, "inPosition")};

  // create VAO and bind vertex attributes to current VAO
  abcg::glGenVertexArrays(1, &m_VAO);
  abcg::glBindVertexArray(m_VAO);
  abcg::glEnableVertexAttribArray(colorAttribute);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBOColors);
  abcg::glVertexAttribPointer(colorAttribute, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);
  abcg::glEnableVertexAttribArray(positionAttribute);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBOPositions);
  abcg::glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // end of binding to current VAO
  abcg::glBindVertexArray(0);
}

void Window::drawGrid() {

  // vertex x
  for (double i = -1; i < (1 + m_unit); i = i + m_unit) {

    // vertex y
    for (double j = -1; j < (1 + m_unit); j = j + m_unit) {

      // draw white blocks on borders
      if (i < (-1 + m_unit) || j < (-1 + m_unit) || i > (1 - 2 * m_unit) || j > (1 - 2 * m_unit)) {

        // set color and position of the block
        m_colors = {{{0.05, 0.05, 0.05, 1}, {0.05, 0.05, 0.05, 1}, {0.05, 0.05, 0.05, 1}, {0.05, 0.05, 0.05, 1}}};
        m_positions = {{{i, j}, {(i + m_unit), j}, {i, (j + m_unit)}, {(i + m_unit), (j + m_unit)}}};
        setupModel();
        
        // draw two triangles to form a square
        abcg::glBindVertexArray(m_VAO);
        abcg::glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        abcg::glBindVertexArray(0);
      }

      // draw gray lines on remaining grid
      else {

        // set color and position of the lines
        m_colors = {{{0.25, 0.25, 0.25, 1}, {0.25, 0.25, 0.25, 1}, {0.25, 0.25, 0.25, 1}, {0.25, 0.25, 0.25, 1}}};
        m_positions = {{{i, j}, {(i + m_unit), j}, {(i + m_unit), (j + m_unit)}, {i, (j + m_unit)}}};
        setupModel();

        // draw lines between our four points
        abcg::glBindVertexArray(m_VAO);
        abcg::glDrawArrays(GL_LINE_LOOP, 0, 4);
        abcg::glBindVertexArray(0);
      }
    }
  }
}

void Window::drawApple() {

  // if generate apple is true, we have to create apple on new random position of the grid 
  if(generateApple) {

    // we have a 10x10 grid, so apple must be on it
    std::uniform_real_distribution rd(1.0f, 10.0f);

    // math to make apple fit correctly on grid, set apple vertices accordingly
    mx_apple = (-1 + floor(rd(m_randomEngine) + m_unit) * m_unit);
    my_apple = (-1 + floor(rd(m_randomEngine) + m_unit) * m_unit);

    // we have successfully generated new apple, so let's make the variable false again
    generateApple = false;
  }

  // set color and position of the apple based on apple vertices
  m_colors = {{{0.78, 0.22, 0.18, 1}, {0.78, 0.22, 0.18, 1}, {0.78, 0.22, 0.18, 1}, {0.78, 0.22, 0.18, 1}}};
  m_positions = {{{mx_apple, my_apple}, {(mx_apple + m_unit), my_apple}, {mx_apple, (my_apple + m_unit)}, {(mx_apple + m_unit), (my_apple + m_unit)}}};
  setupModel();
  
  // draw two triangles to form a square
  abcg::glBindVertexArray(m_VAO);
  abcg::glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  abcg::glBindVertexArray(0);
}

void Window::drawSnake() {

  // set array of vertices, each position of the array is another block of the snake body
  // in each frame, snake [i] body will replicate [i-1] position, mimicking the behavior of its head
  for(int i = ml_snake - 1; i > 0; i--) {
    mx_snake[i] = mx_snake[i-1];
    my_snake[i] = my_snake[i-1];
  }

  // based on our current direction, add one grid unit to the snake position vertices accordingly
  if (m_gameData.m_direction == Direction::Up) {

    // add one unit to snake y vertex if direction is up
    my_snake[0] = my_snake[0] + m_unit;
  } else if (m_gameData.m_direction == Direction::Down) {

    // subtract one unit to snake y vertex if direction is down
    my_snake[0] = my_snake[0] - m_unit;
  } else if (m_gameData.m_direction == Direction::Left) {

    // subtract one unit to snake x vertex if direction is left
    mx_snake[0] = mx_snake[0] - m_unit;
  } else if (m_gameData.m_direction == Direction::Right) {

    // add one unit to snake x vertex if direction is right
    mx_snake[0] = mx_snake[0] + m_unit;
  }

  // draw snake body
  for(int i = 0; i < ml_snake; i++) {

    // set color of the snake, the head of the snake ([i] == 0) is a little clearer
    if(i == 0)
      m_colors = {{{0, 0.75, 0, 1}, {0, 0.75, 0, 1}, {0, 0.75, 0, 1}, {0, 0.75, 0, 1}}};
    else
      m_colors = {{{0, 0.50, 0, 1}, {0, 0.50, 0, 1}, {0, 0.50, 0, 1}, {0, 0.50, 0, 1}}};
    
    // position of the snake based on snake vertices on the array
    m_positions = {{{mx_snake[i], my_snake[i]}, {(mx_snake[i] + m_unit), my_snake[i]}, {mx_snake[i], (my_snake[i] + m_unit)}, {(mx_snake[i] + m_unit), (my_snake[i] + m_unit)}}};
    setupModel();

    // draw two triangles to form a square
    abcg::glBindVertexArray(m_VAO);
    abcg::glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    abcg::glBindVertexArray(0);
  }
}

void Window::play() {

  // reset snake position
  mx_snake = {0, 0, 0, 0, 0};
  my_snake = {0, (0 - (1 * m_unit)), (0 - (2 * m_unit)), (0 - (3 * m_unit)), (0 - (4 * m_unit)), (0 - (5 * m_unit))};
  ml_snake = 5;

  // snake always spawn going right, so it doesn't hit it's own body on spawn
  m_gameData.m_direction = Direction::Right;

  // set apple generation true
  generateApple = true;

  // set game state to playing
  m_gameData.m_state = State::Playing;
}

void Window::validate() {

  // if snake head is on top of apple, we must generate another apple and increase snake size
  if ((std::abs(mx_snake[0] - mx_apple) < 0.00001) && (std::abs(my_snake[0] - my_apple) < 0.00001)) {
    generateApple = true;
    ml_snake++;
  }

  // if snake head is on top of border, we lost :(
  if (mx_snake[0] <= (-1 + m_unit) || my_snake[0] <= (-1 + m_unit) || mx_snake[0] >= (1 - 2 * m_unit) || my_snake[0] >= (1 - 2 * m_unit)) 
    m_gameData.m_state = State::GameOver;

  // if snake head is on top of its body, we also lost :(
  for (int i = 1; i < ml_snake; i++) {
    if (mx_snake[0] == mx_snake[i] && my_snake[0] == my_snake [i])
      m_gameData.m_state = State::GameOver;
  }

  // if we have eaten 10 apples, we won :)
  if (ml_snake == 12) {
    m_gameData.m_state = State::Win;
  }
}