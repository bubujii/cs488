// Termm--Fall 2024

#pragma once

#include <glm/glm.hpp>

#include "SceneNode.hpp"
#include "Light.hpp"
#include "Image.hpp"

glm::dvec3 get_color(
    SceneNode *root,
    std::pair<glm::dvec3, glm::dvec3> ray,
    const glm::dvec3 &ambient,
    const std::list<Light *> &lights,
    int depth,
    const glm::dvec3 &up);

void A4_Render(
    // What to render
    SceneNode *root,

    // Image to write to, set to a given width and height
    Image &image,

    // Viewing parameters
    const glm::dvec3 &eye,
    const glm::dvec3 &view,
    const glm::dvec3 &up,
    double fovy,

    // Lighting parameters
    const glm::dvec3 &ambient,
    const std::list<Light *> &lights);
