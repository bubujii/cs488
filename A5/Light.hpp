// Termm--Fall 2024

#pragma once

#include <iosfwd>

#include <glm/glm.hpp>

// Represents a simple point light.
struct Light
{
  Light();

  glm::dvec3 colour;
  glm::dvec3 position;
  double falloff[3];
};

std::ostream &operator<<(std::ostream &out, const Light &l);
