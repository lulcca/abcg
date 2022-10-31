#ifndef WINDOW_HPP_
#define WINDOW_HPP_

#include "abcgOpenGL.hpp"
#include "gamedata.hpp"

#include <random>

class Window : public abcg::OpenGLWindow {
  protected:
    void onCreate() override;
    void onDestroy() override;
    void onEvent(SDL_Event const &event) override;
    void onPaint() override;
    void onPaintUI() override;
    void onResize(glm::ivec2 const &size) override;
    void onUpdate() override;

  private:
    ImFont *m_font{};

    abcg::Timer m_timer;

    glm::ivec2 m_viewportSize{};

    GameData m_gameData;

    GLuint m_VAO{};
    GLuint m_VBOColors{};
    GLuint m_VBOPositions{};
    GLuint m_program{};

    double m_unit = 0.1;

    bool generateApple;
    double mx_apple;
    double my_apple;

    int ml_snake;
    std::array<double, 12> mx_snake;
    std::array<double, 12> my_snake;

    std::array<glm::vec4, 4> m_colors;
    std::array<glm::vec2, 4> m_positions;

    std::default_random_engine m_randomEngine;

    void drawGrid();
    void drawApple();
    void drawSnake();
    void setupModel();
    void play();
    void validate();
};

#endif