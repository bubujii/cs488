// Termm--Fall 2024

#pragma once

#include <glm/glm.hpp>

#include "SceneNode.hpp"
#include "Light.hpp"
#include "Image.hpp"

glm::vec3 get_color(SceneNode *root, std::pair<glm::vec3, glm::vec3> ray, const glm::vec3 &ambient, const std::list<Light *> &lights, int depth, float ior, std::vector<glm::vec3> refract_stack);

void A4_Render(
	// What to render
	SceneNode *root,

	// Image to write to, set to a given width and height
	Image &image,

	// Viewing parameters
	const glm::vec3 &eye,
	const glm::vec3 &view,
	const glm::vec3 &up,
	double fovy,

	// Lighting parameters
	const glm::vec3 &ambient,
	const std::list<Light *> &lights);
