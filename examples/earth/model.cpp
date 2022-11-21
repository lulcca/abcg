#include "model.hpp"

#include <filesystem>
#include <unordered_map>
#include <iostream>

template <> struct std::hash<Vertex> {

  // specialization of std::hash for vertex
  size_t operator()(Vertex const &vertex) const noexcept {
    auto const h1{std::hash<glm::vec3>()(vertex.position)};
    auto const h2{std::hash<glm::vec3>()(vertex.normal)};
    auto const h3{std::hash<glm::vec2>()(vertex.texCoord)};

    return abcg::hashCombine(h1, h2, h3);
  }
};

void Model::loadDiffuseTexture(std::string_view path) {

  // if file on path does not exist, return
  if (!std::filesystem::exists(path))
    return;

  // release previous texture
  abcg::glDeleteTextures(1, &m_diffuseTexture);

  // load new texture with given path
  m_diffuseTexture = abcg::loadOpenGLTexture({.path = path});
}

void Model::loadObj(std::string_view path, bool standardize) {

  // get path from object
  auto const basePath{std::filesystem::path{path}.parent_path().string() + "/"};

  // load material file
  tinyobj::ObjReaderConfig readerConfig;
  readerConfig.mtl_search_path = basePath;

  // load object
  tinyobj::ObjReader reader;

  // print error when file not found
  if (!reader.ParseFromFile(path.data(), readerConfig)) {
    if (!reader.Error().empty()) {
      throw abcg::RuntimeError(fmt::format("Failed to load model {} ({})", path, reader.Error()));
    }
    throw abcg::RuntimeError(fmt::format("Failed to load model {}", path));
  }

  // print warning if existent
  if (!reader.Warning().empty()) {
    fmt::print("Warning: {}\n", reader.Warning());
  }

  // load attributes, shapes and materials from object
  auto const &attrib{reader.GetAttrib()};
  auto const &shapes{reader.GetShapes()};
  auto const &materials{reader.GetMaterials()};

  // clear vertices and indices
  m_vertices.clear(); m_indices.clear();

  // initiate hashmap with key=vertex and value=index
  std::unordered_map<Vertex, GLuint> hash{};

  // loop through shapes
  for (auto const &shape : shapes) {

    // loop through indices
    for (auto const offset : iter::range(shape.mesh.indices.size())) {

      // access vertex
      auto const index{shape.mesh.indices.at(offset)};

      // vertex position
      auto const startIndex{3 * index.vertex_index};
      glm::vec3 position{attrib.vertices.at(startIndex + 0), attrib.vertices.at(startIndex + 1), attrib.vertices.at(startIndex + 2)};

      // vertex normal
      glm::vec3 normal{};
      if (index.normal_index >= 0) {
        auto const normalStartIndex{3 * index.normal_index};
        normal = {attrib.normals.at(normalStartIndex + 0), attrib.normals.at(normalStartIndex + 1), attrib.normals.at(normalStartIndex + 2)};
      }

      // texture coordinates
      glm::vec2 texCoord{};
      if (index.texcoord_index >= 0) {
        auto const texCoordsStartIndex{2 * index.texcoord_index};
        texCoord = {attrib.texcoords.at(texCoordsStartIndex + 0), attrib.texcoords.at(texCoordsStartIndex + 1)};
      }

      // set vertex
      Vertex const vertex{.position = position, .normal = normal, .texCoord = texCoord};

      // if hash does not contain actual vertex, add index and push this vertex
      if (!hash.contains(vertex)) {
        hash[vertex] = m_vertices.size();
        m_vertices.push_back(vertex);
      }

      // add hash value to index vector
      m_indices.push_back(hash[vertex]);
    }
  }

  // use properties of first material
  if (!materials.empty()) {
    auto const &mat{materials.at(0)};
    m_Ka = {mat.ambient[0], mat.ambient[1], mat.ambient[2], 1};
    m_Kd = {mat.diffuse[0], mat.diffuse[1], mat.diffuse[2], 1};
    m_Ks = {mat.specular[0], mat.specular[1], mat.specular[2], 1};
    m_shininess = mat.shininess;

    // load texture from material
    if (!mat.diffuse_texname.empty()) {
      loadDiffuseTexture(basePath + mat.diffuse_texname);
    }
  } 

  // default values if material does not exist
  else {
    m_Ka = {0.1f, 0.1f, 0.1f, 1.0f};
    m_Kd = {0.7f, 0.7f, 0.7f, 1.0f};
    m_Ks = {1.0f, 1.0f, 1.0f, 1.0f};
    m_shininess = 25.0f;
  }

  // standardize our object based on our pipeline
  if (standardize) {
    Model::standardize();
  }

  // compute normal values from object
  computeNormals();

  // create VBO and EBO buffers
  createBuffers();
}

void Model::computeNormals() {

  // clear previous vertices
  for (auto &vertex : m_vertices) {
    vertex.normal = glm::vec3(0.0f);
  }

  // compute face normals
  for (auto const offset : iter::range(0ULL, m_indices.size() , 3ULL)) {

    // get face vertices
    auto &a{m_vertices.at(m_indices.at(offset + 0))};
    auto &b{m_vertices.at(m_indices.at(offset + 1))};
    auto &c{m_vertices.at(m_indices.at(offset + 2))};

    // compute normal
    auto const edge1{b.position - a.position};
    auto const edge2{c.position - b.position};
    auto const normal{glm::cross(edge1, edge2)};

    // accumulate our three vertices
    a.normal += normal;
    b.normal += normal;
    c.normal += normal;
  }

  // normalize vertices
  for (auto &vertex : m_vertices) {
    vertex.normal = glm::normalize(vertex.normal);
  }
}

void Model::createBuffers() {

  // release previous element and array buffer
  abcg::glDeleteBuffers(1, &m_EBO);
  abcg::glDeleteBuffers(1, &m_VBO);

  // generate VBO
  abcg::glGenBuffers(1, &m_VBO);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
  abcg::glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices.at(0)) * m_vertices.size(), m_vertices.data(), GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // generate EBO
  abcg::glGenBuffers(1, &m_EBO);
  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
  abcg::glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_indices.at(0)) * m_indices.size(), m_indices.data(), GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Model::render() const {

  // create VAO and bind vertex attributes and texture to current VAO
  abcg::glBindVertexArray(m_VAO);
  abcg::glActiveTexture(GL_TEXTURE0);
  abcg::glBindTexture(GL_TEXTURE_2D, m_diffuseTexture);

  // set minification and magnification parameters
  abcg::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  abcg::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // set texture wrapping parameters
  abcg::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  abcg::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  // draw elements
  abcg::glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, nullptr);

  // end of binding to current VAO
  abcg::glBindVertexArray(0);
}

void Model::setupVAO(GLuint program) {

  // release previous VAO
  abcg::glDeleteVertexArrays(1, &m_VAO);

  // create VAO
  abcg::glGenVertexArrays(1, &m_VAO);
  abcg::glBindVertexArray(m_VAO);

  // bind EBO and VBO
  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

  // bind vertex attributes
  auto const positionAttribute{ abcg::glGetAttribLocation(program, "inPosition") };
  if (positionAttribute >= 0) {
    abcg::glEnableVertexAttribArray(positionAttribute);
    abcg::glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
  }

  auto const normalAttribute{ abcg::glGetAttribLocation(program, "inNormal") };
  if (normalAttribute >= 0) {
    abcg::glEnableVertexAttribArray(normalAttribute);
    auto const offset{offsetof(Vertex, normal)};
    abcg::glVertexAttribPointer(normalAttribute, 3, GL_FLOAT, GL_FALSE,  sizeof(Vertex), reinterpret_cast<void *>(offset));
  }

  auto const texCoordAttribute{ abcg::glGetAttribLocation(program, "inTexCoord") };
  if (texCoordAttribute >= 0) {
    abcg::glEnableVertexAttribArray(texCoordAttribute);
    auto const offset{offsetof(Vertex, texCoord)};
    abcg::glVertexAttribPointer(texCoordAttribute, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void *>(offset));
  }

  // end of binding
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);
  abcg::glBindVertexArray(0);
}

void Model::standardize() {
  glm::vec3 max(std::numeric_limits<float>::lowest());
  glm::vec3 min(std::numeric_limits<float>::max());
  for (auto const &vertex : m_vertices) {
    max = glm::max(max, vertex.position);
    min = glm::min(min, vertex.position);
  }

  auto const center{(min + max) / 2.0f};
  auto const scaling{2.0f / glm::length(max - min)};
  for (auto &vertex : m_vertices) {
    vertex.position = (vertex.position - center) * scaling;
  }
}

void Model::destroy() const {

  // release opengl resources that were allocated during application
  abcg::glDeleteTextures(1, &m_diffuseTexture);
  abcg::glDeleteBuffers(1, &m_EBO);
  abcg::glDeleteBuffers(1, &m_VBO);
  abcg::glDeleteVertexArrays(1, &m_VAO);
}