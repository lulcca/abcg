#ifndef RENDER_HPP_
#define RENDER_HPP_

#include "abcgOpenGL.hpp"
#include <glm/glm.hpp>

using std::string;
using std::vector;

class Render {
public:
  void drawCube(GLuint m_program, GLuint m_VAO, glm::vec3 pos, glm::vec3 scale, glm::vec3 rotation, glm::vec3 cameraPos, GLuint m_texture);
  void createCube(GLuint m_VAO, string m_texturePath, vector<GLuint> m_textures);
  void destroy(GLuint m_program, GLuint m_VAO, GLuint m_VBOPositions, GLuint m_VBOColors);
 
private:
  void setVAO(GLuint m_VAO);
  GLuint loadTexture(string path);
  void renderCube(GLuint m_program, GLuint m_VAO, glm::vec3 pos, glm::vec3 scale, glm::vec3 rotation, GLuint m_texture, glm::vec3 cameraPos);
};

#endif