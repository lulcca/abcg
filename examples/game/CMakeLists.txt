project(game)
add_executable(${PROJECT_NAME} main.cpp window.cpp player.cpp camera.cpp obstacle.cpp)
enable_abcg(${PROJECT_NAME})
