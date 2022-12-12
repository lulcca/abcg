#include "camera.hpp"
#include <glm/glm.hpp>

Camera::Camera() {
	//Posição default da camera
	pos = glm::vec3(0.f, -1.f, -5.f);
}
