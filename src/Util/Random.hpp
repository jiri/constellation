#pragma once

#include <random>
#include <glm/glm.hpp>

static float randomFloat() {
  static thread_local std::default_random_engine generator;
  static thread_local std::uniform_real_distribution<float> distribution(0.0f, 1.0f);
  return distribution(generator);
}

static glm::vec3 randomColor() {
  return { randomFloat(), randomFloat(), randomFloat() };
}
