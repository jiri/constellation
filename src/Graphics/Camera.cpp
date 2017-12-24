#include <Graphics/Camera.hpp>

#include <glm/gtx/transform.hpp>

GCamera::GCamera(std::string name, const glm::vec3 &p, const glm::vec3 &f)
  : name { std::move(name) }
  , position { p }
  , front { f }
{ }

glm::mat4 GCamera::matrix() const {
  return glm::perspective(glm::radians(75.0f), 4.0f / 3.0f, 0.01f, 100.0f)
         * glm::lookAt(position, position + front, up);
}
