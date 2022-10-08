#include "window.hpp"
#include <iostream>

void Window::onCreate() {
  // Load font with bigger size
  auto const filename{abcg::Application::getAssetsPath() +  "Inconsolata-Medium.ttf"};
  m_font = ImGui::GetIO().Fonts->AddFontFromFileTTF(filename.c_str(), 60.0f);
  // Throw error if it fails
  if (m_font == nullptr) {
    throw abcg::RuntimeError{"Cannot load font file"};
  }

  // Define default difficulty
  m_N = 3;
  m_board = easy_sequence;

  // Shuffle the array
  shuffleBoard();
}

void Window::onPaintUI() {
  // Get window size
  auto const appWindowWidth{gsl::narrow<float>(getWindowSettings().width)};
  auto const appWindowHeight{gsl::narrow<float>(getWindowSettings().height)};

  { 
    // Create window
    ImGui::SetNextWindowSize(ImVec2(appWindowWidth, appWindowHeight));
    ImGui::SetNextWindowPos(ImVec2(0, 0));

    // Create top bar
    auto const flags{ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse};
    ImGui::Begin("Slidin'Puzzle", nullptr, flags);

    
    {
      // Create menu bar
      bool easy{}, hard{}, shuffle{};
      if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("Difficulty")) {
          ImGui::MenuItem("Easy", nullptr, &easy);
          ImGui::MenuItem("Hard", nullptr, &hard);
          ImGui::EndMenu();
        }
        ImGui::MenuItem("Restart", nullptr, &shuffle);
        ImGui::EndMenuBar();
      }

      // Press restart should give you a new board and change game state to play
      if (shuffle) {
        shuffleBoard();
      }

      // Easy difficulty gets the 'easy' sequence defined on window.hpp and set column and row size to 3
      if(easy) {
        m_N = 3;
        m_board = easy_sequence;
        shuffleBoard();
      }
      // Hard difficulty also gets the 'hard' sequence and set column and row size to 5
      if (hard) {
        m_N = 5;
        m_board = hard_sequence;
        shuffleBoard();
      }
    }

    {
      // Static text that tells you which game state you are
      std::string text;
      switch (m_gameState) {
        case GameState::Play:
          text = "Move the pieces!";
          break;
        case GameState::Won:
          text = "YAY, YOU DID IT!";
          break;
      }
      ImGui::SetCursorPosX((appWindowWidth - ImGui::CalcTextSize(text.c_str()).x) / 2);
      ImGui::Text("%s", text.c_str());
    }

    ImGui::Spacing();

    {
      // Calculate board size
      auto const gridHeight{appWindowHeight - 22 - 60 - (m_N * 10)};

      // Define button size based on columns and rows
      auto const buttonHeight{gridHeight / m_N};

      // Use previously defined font
      ImGui::PushFont(m_font);

      // If game state is 'won', the background should be green
      // If game state is 'play', it can be blue or red, based on difficulty
      m_gameState == GameState::Won ? 
      ImGui::PushStyleColor(ImGuiCol_WindowBg, (ImVec4)ImColor(0, 153, 0)) :
      m_N == 3 ?
      ImGui::PushStyleColor(ImGuiCol_WindowBg, (ImVec4)ImColor(0, 102, 102)) :
      ImGui::PushStyleColor(ImGuiCol_WindowBg, (ImVec4)ImColor(102, 0, 0));

      // Create table based on our global variable m_N
      if (ImGui::BeginTable("Board", m_N)) {
        for (auto i : iter::range(m_N)) {
          ImGui::TableNextRow();
          for (auto j : iter::range(m_N)) {
            ImGui::TableSetColumnIndex(j);

            // Set button text and colors based on their location
            auto const offset{i * m_N + j};
            auto ch{m_board.at(offset)};
            auto buttonText{fmt::format("{}##{}{}", ch, i, j)};
            if(m_gameState == GameState::Won || m_board.at(offset) == ' ') {
              ImGui::PushStyleColor(ImGuiCol_Button, ImGuiColorEditFlags_None);
              ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImGuiColorEditFlags_None);
              ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGuiColorEditFlags_None);
            } else if(m_N == 3) {
              ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor(0, 153, 153));
              ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor(0, 255, 255));
              ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor(0, 255, 255));
            } else {
              ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor(204, 0, 0));
              ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor(255, 51, 51));
              ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor(255, 51, 51));
            };

            // Create buttons with configuration above, if clicked, stuff happens...
            if (ImGui::Button(buttonText.c_str(), ImVec2(-1, buttonHeight))) {
              
              // But stuff only happens if you're playing!
              if(m_gameState == GameState::Play) {

                // Buttons can only be moved if they're next to the empty square:

                // This compare the clicked button with the button above it, but taking care to not compare with buttons outside of our array (offset - m_N) >= 0
                // If the button above is the empty block, they trade content
                if(((offset - m_N) >= 0) && m_board.at(offset - m_N) == ' ') {
                  auto temp = m_board.at(offset);
                  m_board.at(offset) = m_board.at(offset - m_N);
                  m_board.at(offset - m_N) = temp;
                }

                // Same logic, but with the left button, we just need to subtract 1 here
                else if(((offset - 1) >= 0) && m_board.at(offset - 1) == ' ') {
                  auto temp = m_board.at(offset);
                  m_board.at(offset) = m_board.at(offset - 1);
                  m_board.at(offset - 1) = temp;
                }

                // Same logic, but with the right button, we just need to sum 1 here
                else if(((offset + 1) <= ((m_N * m_N) - 1)) && m_board.at(offset + 1) == ' ') {
                  auto temp = m_board.at(offset);
                  m_board.at(offset) = m_board.at(offset + 1);
                  m_board.at(offset + 1) = temp;
                }

                // Same logic, but the button below, se sum the numbers of columns to get it
                else if(((offset + m_N) <= ((m_N * m_N) - 1)) && m_board.at(offset + m_N) == ' ') {
                  auto temp = m_board.at(offset);
                  m_board.at(offset) = m_board.at(offset + m_N);
                  m_board.at(offset + m_N) = temp;
                }
              
                // After our play, we gotta see if we won!
                winCheck();
              }
            }
          }
        }

        ImGui::EndTable();
      }

      ImGui::PopFont();
    }

    ImGui::End();
  }
}

void Window::shuffleBoard() {
  // Shuffle the array
  std::random_shuffle(m_board.begin(), m_board.end());

  // Start the game
  m_gameState = GameState::Play;
}

void Window::winCheck() {
  // Can't validate if you already won
  if (m_gameState == GameState::Won) {
    return;
  }

  // If our global variable m_N is 3, than we are playing the easy mode, so we gotta compare our array with the easy_sequence
  if (m_N == 3 && m_board == easy_sequence) {
    m_gameState = GameState::Won;
    return;
  };

  // If our global variable is 5, we were in hard mode, so lets compare with the hard_sequence
  if (m_N == 5 && m_board == hard_sequence) {
    m_gameState = GameState::Won;
    return;
  };
}