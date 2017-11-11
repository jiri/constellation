#include <random>
#include <glm/glm.hpp>

float randomFloat() {
  static thread_local std::default_random_engine generator;
  static thread_local std::uniform_real_distribution<float> distribution(0.0f, 1.0);
  return distribution(generator);
}

glm::vec3 randomColor() {
  return { randomFloat(), randomFloat(), randomFloat() };
}
