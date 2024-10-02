// Termm--Fall 2024

#include "A2.hpp"
#include "cs488-framework/GlErrorCheck.hpp"

#include <iostream>
using namespace std;

#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>
using namespace glm;

static const int ROTATE_VIEW = 0;
static const int TRANSLATE_VIEW = 1;
static const int PERSPECTIVE = 2;
static const int ROTATE_MODEL = 3;
static const int TRANSLATE_MODEL = 4;
static const int SCALE_MODEL = 5;
static const int VIEWPORT = 6;

//----------------------------------------------------------------------------------------
// Constructor
VertexData::VertexData()
	: numVertices(0),
	  index(0)
{
	positions.resize(kMaxVertices);
	colours.resize(kMaxVertices);
}

//----------------------------------------------------------------------------------------
// Constructor
A2::A2()
	: m_currentLineColour(vec3(0.0f)), cube(), active_buttons({0, 0, 0}), view_matrix()
{
}

//----------------------------------------------------------------------------------------
// Destructor
A2::~A2()
{
}

//----------------------------------------------------------------------------------------
/*
 * Called once, at program start.
 */
void A2::init()
{
	// Set the background colour.
	glClearColor(0.2, 0.5, 0.3, 1.0);

	createShaderProgram();

	glGenVertexArrays(1, &m_vao);

	enableVertexAttribIndices();

	generateVertexBuffers();

	mapVboDataToVertexAttributeLocation();

	initScene();
}

void A2::initScene()
{

	glm::mat4 universal_scale = glm::mat4(0.25f);
	universal_scale[3][3] = 1.f;
	cube.reset(universal_scale);
	worldGnomon.reset(universal_scale);

	auto view_matrix_mat = glm::mat4();
	glm::vec3 forwardVector = glm::vec3();
	glm::vec3 positionOfCamera = {1, 3, 4};
	glm::vec3 positionOfTarget = {0, 0, 0};
	forwardVector = glm::normalize(positionOfCamera - positionOfTarget);

	glm::vec3 rightVector = glm::vec3();
	glm::vec3 tempUpVector = {0, 1, 0};
	rightVector = glm::normalize(glm::cross(tempUpVector, forwardVector));

	glm::vec3 upVector = glm::vec3();
	upVector = glm::normalize(glm::cross(forwardVector, rightVector));

	float translationX = glm::dot(positionOfCamera, rightVector);
	float translationY = glm::dot(positionOfCamera, upVector);
	float translationZ = glm::dot(positionOfCamera, forwardVector);

	view_matrix_mat = glm::mat4({rightVector.x, upVector.x, forwardVector.x, 0,
								 rightVector.y, upVector.y, forwardVector.y, 0,
								 rightVector.z, upVector.z, forwardVector.z, 0,
								 -translationX, -translationY, -translationZ, 1});

	view_matrix.reset(view_matrix_mat);

	projection_matrix = glm::mat4(1.f); // orthographic for now
	projection_matrix[2][2] = 0;
}

//----------------------------------------------------------------------------------------
void A2::createShaderProgram()
{
	m_shader.generateProgramObject();
	m_shader.attachVertexShader(getAssetFilePath("VertexShader.vs").c_str());
	m_shader.attachFragmentShader(getAssetFilePath("FragmentShader.fs").c_str());
	m_shader.link();
}

//---------------------------------------------------------------------------------------- Fall 2022
void A2::enableVertexAttribIndices()
{
	glBindVertexArray(m_vao);

	// Enable the attribute index location for "position" when rendering.
	GLint positionAttribLocation = m_shader.getAttribLocation("position");
	glEnableVertexAttribArray(positionAttribLocation);

	// Enable the attribute index location for "colour" when rendering.
	GLint colourAttribLocation = m_shader.getAttribLocation("colour");
	glEnableVertexAttribArray(colourAttribLocation);

	// Restore defaults
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
void A2::generateVertexBuffers()
{
	// Generate a vertex buffer to store line vertex positions
	{
		glGenBuffers(1, &m_vbo_positions);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_positions);

		// Set to GL_DYNAMIC_DRAW because the data store will be modified frequently.
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * kMaxVertices, nullptr,
					 GL_DYNAMIC_DRAW);

		// Unbind the target GL_ARRAY_BUFFER, now that we are finished using it.
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		CHECK_GL_ERRORS;
	}

	// Generate a vertex buffer to store line colors
	{
		glGenBuffers(1, &m_vbo_colours);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_colours);

		// Set to GL_DYNAMIC_DRAW because the data store will be modified frequently.
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * kMaxVertices, nullptr,
					 GL_DYNAMIC_DRAW);

		// Unbind the target GL_ARRAY_BUFFER, now that we are finished using it.
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		CHECK_GL_ERRORS;
	}
}

//----------------------------------------------------------------------------------------
void A2::mapVboDataToVertexAttributeLocation()
{
	// Bind VAO in order to record the data mapping.
	glBindVertexArray(m_vao);

	// Tell GL how to map data from the vertex buffer "m_vbo_positions" into the
	// "position" vertex attribute index for any bound shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_positions);
	GLint positionAttribLocation = m_shader.getAttribLocation("position");
	glVertexAttribPointer(positionAttribLocation, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

	// Tell GL how to map data from the vertex buffer "m_vbo_colours" into the
	// "colour" vertex attribute index for any bound shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_colours);
	GLint colorAttribLocation = m_shader.getAttribLocation("colour");
	glVertexAttribPointer(colorAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	//-- Unbind target, and restore default values:
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//---------------------------------------------------------------------------------------
void A2::initLineData()
{
	m_vertexData.numVertices = 0;
	m_vertexData.index = 0;
}

//---------------------------------------------------------------------------------------
void A2::setLineColour(
	const glm::vec3 &colour)
{
	m_currentLineColour = colour;
}

//---------------------------------------------------------------------------------------
void A2::drawLine(
	const glm::vec2 &V0, // Line Start (NDC coordinate)
	const glm::vec2 &V1	 // Line End (NDC coordinate)
)
{

	m_vertexData.positions[m_vertexData.index] = V0;
	m_vertexData.colours[m_vertexData.index] = m_currentLineColour;
	++m_vertexData.index;
	m_vertexData.positions[m_vertexData.index] = V1;
	m_vertexData.colours[m_vertexData.index] = m_currentLineColour;
	++m_vertexData.index;

	m_vertexData.numVertices += 2;
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, before guiLogic().
 */
void A2::appLogic()
{
	// Place per frame, application logic here ...

	// Call at the beginning of frame, before drawing lines:
	initLineData();

	// Draw outer square:
	setLineColour(vec3(1.0f, 0.7f, 0.8f));
	drawLine(vec2(-0.5f, -0.5f), vec2(0.5f, -0.5f));
	drawLine(vec2(0.5f, -0.5f), vec2(0.5f, 0.5f));
	drawLine(vec2(0.5f, 0.5f), vec2(-0.5f, 0.5f));
	drawLine(vec2(-0.5f, 0.5f), vec2(-0.5f, -0.5f));

	// Draw inner square:
	// setLineColour(vec3(0.2f, 1.0f, 1.0f));
	// drawLine(vec2(-0.25f, -0.25f), vec2(0.25f, -0.25f));
	// drawLine(vec2(0.25f, -0.25f), vec2(0.25f, 0.25f));
	// drawLine(vec2(0.25f, 0.25f), vec2(-0.25f, 0.25f));
	// drawLine(vec2(-0.25f, 0.25f), vec2(-0.25f, -0.25f));

	auto verts = cube.applyMatrix(projection_matrix * view_matrix.getTransform());

	// std::cout << "------------" << std::endl;

	// std::cout << verts[4] << std::endl;
	// std::cout << verts[0] << std::endl;
	// std::cout << verts[1] << std::endl;
	// std::cout << verts[5] << std::endl;

	setLineColour(vec3(0.0f, 1.0f, 1.0f)); // cyan
	drawLine(vec2(verts[0].x, verts[0].y), vec2(verts[3].x, verts[3].y));
	drawLine(vec2(verts[3].x, verts[3].y), vec2(verts[2].x, verts[2].y));
	drawLine(vec2(verts[2].x, verts[2].y), vec2(verts[1].x, verts[1].y));
	drawLine(vec2(verts[1].x, verts[1].y), vec2(verts[0].x, verts[0].y));

	setLineColour(vec3(1.0f, 1.0f, 0.0f)); // yellow
	drawLine(vec2(verts[4].x, verts[4].y), vec2(verts[5].x, verts[5].y));
	drawLine(vec2(verts[5].x, verts[5].y), vec2(verts[6].x, verts[6].y));
	drawLine(vec2(verts[6].x, verts[6].y), vec2(verts[7].x, verts[7].y));
	drawLine(vec2(verts[7].x, verts[7].y), vec2(verts[4].x, verts[4].y));

	setLineColour(vec3(1.0f, 0.0f, 1.0f)); // magenta
	drawLine(vec2(verts[0].x, verts[0].y), vec2(verts[4].x, verts[4].y));
	setLineColour(vec3(1.0f, 0.0f, 0.0f)); // red
	drawLine(vec2(verts[1].x, verts[1].y), vec2(verts[5].x, verts[5].y));
	setLineColour(vec3(0.0f, 0.0f, 1.0f)); // blue
	drawLine(vec2(verts[3].x, verts[3].y), vec2(verts[7].x, verts[7].y));
	setLineColour(vec3(0.0f, 1.0f, 0.0f)); // green
	drawLine(vec2(verts[2].x, verts[2].y), vec2(verts[6].x, verts[6].y));

	auto gnomon_cube_points = cube.applyMatrixSubModel(projection_matrix * view_matrix.getTransform());
	auto gnomon_world_points = worldGnomon.applyMatrix(projection_matrix * view_matrix.getTransform());

	setLineColour(vec3(1.0f, 0.0f, 0.0f)); // red
	drawLine({gnomon_cube_points[0].x, gnomon_cube_points[0].y}, {gnomon_cube_points[1].x, gnomon_cube_points[1].y});
	drawLine({gnomon_world_points[0].x, gnomon_world_points[0].y}, {gnomon_world_points[1].x, gnomon_world_points[1].y});
	setLineColour(vec3(0.0f, 1.0f, 0.0f)); // green
	drawLine({gnomon_cube_points[0].x, gnomon_cube_points[0].y}, {gnomon_cube_points[2].x, gnomon_cube_points[2].y});
	drawLine({gnomon_world_points[0].x, gnomon_world_points[0].y}, {gnomon_world_points[2].x, gnomon_world_points[2].y});
	setLineColour(vec3(0.0f, 0.0f, 1.0f)); // blue
	drawLine({gnomon_cube_points[0].x, gnomon_cube_points[0].y}, {gnomon_cube_points[3].x, gnomon_cube_points[3].y});
	drawLine({gnomon_world_points[0].x, gnomon_world_points[0].y}, {gnomon_world_points[3].x, gnomon_world_points[3].y});
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after appLogic(), but before the draw() method.
 */
void A2::guiLogic()
{
	static bool firstRun(true);
	if (firstRun)
	{
		ImGui::SetNextWindowPos(ImVec2(50, 50));
		firstRun = false;
	}

	static bool showDebugWindow(true);
	ImGuiWindowFlags windowFlags(ImGuiWindowFlags_AlwaysAutoResize);
	float opacity(0.5f);

	ImGui::Begin("Properties", &showDebugWindow, ImVec2(100, 100), opacity,
				 windowFlags);

	// Add more gui elements here here ...

	if (ImGui::RadioButton("Rotate View (o)", &current_change, ROTATE_VIEW))
	{
	}
	if (ImGui::RadioButton("Translate View (e)", &current_change, TRANSLATE_VIEW))
	{
	}
	if (ImGui::RadioButton("Perspective (p)", &current_change, PERSPECTIVE))
	{
	}
	if (ImGui::RadioButton("Rotate Model (r)", &current_change, ROTATE_MODEL))
	{
	}
	if (ImGui::RadioButton("Translate Model (t)", &current_change, TRANSLATE_MODEL))
	{
	}
	if (ImGui::RadioButton("Scale Model (s)", &current_change, SCALE_MODEL))
	{
	}
	if (ImGui::RadioButton("Viewport (v)", &current_change, VIEWPORT))
	{
	}

	if (ImGui::Button("Reset"))
	{
		initScene();
	}
	// Create Button, and check if it was clicked:
	if (ImGui::Button("Quit Application"))
	{
		glfwSetWindowShouldClose(m_window, GL_TRUE);
	}

	ImGui::Text("Framerate: %.1f FPS", ImGui::GetIO().Framerate);

	ImGui::End();
}

//----------------------------------------------------------------------------------------
void A2::uploadVertexDataToVbos()
{

	//-- Copy vertex position data into VBO, m_vbo_positions:
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_positions);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec2) * m_vertexData.numVertices,
						m_vertexData.positions.data());
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		CHECK_GL_ERRORS;
	}

	//-- Copy vertex colour data into VBO, m_vbo_colours:
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_colours);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec3) * m_vertexData.numVertices,
						m_vertexData.colours.data());
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		CHECK_GL_ERRORS;
	}
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after guiLogic().
 */
void A2::draw()
{
	uploadVertexDataToVbos();

	glBindVertexArray(m_vao);

	m_shader.enable();
	glDrawArrays(GL_LINES, 0, m_vertexData.numVertices);
	m_shader.disable();

	// Restore defaults
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
/*
 * Called once, after program is signaled to terminate.
 */
void A2::cleanup()
{
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles cursor entering the window area events.
 */
bool A2::cursorEnterWindowEvent(
	int entered)
{
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse cursor movement events.
 */
bool A2::mouseMoveEvent(
	double xPos,
	double yPos)
{
	bool eventHandled(false);
	// Fill in with event handling code...
	if (!ImGui::IsMouseHoveringAnyWindow())
	{
		if (active_buttons.x != 0 || active_buttons.y != 0 || active_buttons.z != 0)
		{
			float change = float((xPos - mouse_position.x) / 10);
			switch (current_change)
			{
			case TRANSLATE_MODEL:
				cube.translate(active_buttons * change);
				break;

			case SCALE_MODEL:
				cube.scale(glm::vec3(1.f) + active_buttons * change);
				break;

			case ROTATE_MODEL:
				change /= 15;
				cube.rotate(active_buttons * change);
				break;

			case TRANSLATE_VIEW:
				view_matrix.translate(active_buttons * change);
				break;

			case ROTATE_VIEW:
				change /= 5;
				view_matrix.rotate(active_buttons * change);
				break;

			default:
				break;
			}
		}
		mouse_position.x = float(xPos);
	}

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse button events.
 */
bool A2::mouseButtonInputEvent(
	int button,
	int actions,
	int mods)
{
	bool eventHandled(false);

	if (!ImGui::IsMouseHoveringAnyWindow())
	{
		if (actions == GLFW_PRESS)
		{
			if (button == GLFW_MOUSE_BUTTON_LEFT)
			{
				active_buttons.x = 1;
			}
			if (button == GLFW_MOUSE_BUTTON_MIDDLE)
			{
				active_buttons.y = 1;
			}
			if (button == GLFW_MOUSE_BUTTON_RIGHT)
			{
				active_buttons.z = 1;
			}
			eventHandled = true;
		}
		if (actions == GLFW_RELEASE)
		{
			if (button == GLFW_MOUSE_BUTTON_LEFT)
			{
				active_buttons.x = 0;
			}
			if (button == GLFW_MOUSE_BUTTON_MIDDLE)
			{
				active_buttons.y = 0;
			}
			if (button == GLFW_MOUSE_BUTTON_RIGHT)
			{
				active_buttons.z = 0;
			}
			eventHandled = true;
		}
	}

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse scroll wheel events.
 */
bool A2::mouseScrollEvent(
	double xOffSet,
	double yOffSet)
{
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles window resize events.
 */
bool A2::windowResizeEvent(
	int width,
	int height)
{
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles key input events.
 */
bool A2::keyInputEvent(
	int key,
	int action,
	int mods)
{
	bool eventHandled(false);

	if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_O)
		{
			current_change = ROTATE_VIEW;
		}
		if (key == GLFW_KEY_E)
		{
			current_change = TRANSLATE_VIEW;
		}
		if (key == GLFW_KEY_P)
		{
			current_change = PERSPECTIVE;
		}
		if (key == GLFW_KEY_R)
		{
			current_change = ROTATE_MODEL;
		}
		if (key == GLFW_KEY_T)
		{
			current_change = TRANSLATE_MODEL;
		}
		if (key == GLFW_KEY_S)
		{
			current_change = SCALE_MODEL;
		}
		if (key == GLFW_KEY_V)
		{
			current_change = VIEWPORT;
		}
		if (key == GLFW_KEY_A)
		{
			initScene();
		}
		if (key == GLFW_KEY_LEFT_SHIFT)
		{
			active_buttons.y = 1.0f;
		}
	}
	if (action == GLFW_RELEASE)
	{
		if (key == GLFW_KEY_LEFT_SHIFT)
		{
			active_buttons.y = 0.0f;
		}
	}

	return eventHandled;
}

Model::Model() : subModel(nullptr)
{
	transformation_matrix = glm::mat4(1.f);
}

Model::~Model()
{
}

std::vector<glm::vec4> Model::getVertices()
{
	std::vector<glm::vec4> ret_vertices;
	for (auto point : this->vertices)
	{
		ret_vertices.push_back(this->transformation_matrix * point);
	}
	return ret_vertices;
}

void Model::transform(glm::mat4 transform_by)
{
	this->transformation_matrix *= transform_by;
}

std::vector<glm::vec4> Model::applyMatrix(glm::mat4 matrix)
{
	auto points = this->getVertices();
	std::vector<glm::vec4> ret_points;
	for (auto point : points)
	{
		ret_points.push_back(matrix * point);
	}
	return ret_points;
}

void Model::translate(glm::vec3 translate_vector)
{
	if (subModel)
	{
		subModel->translate(translate_vector);
	}
	glm::mat4 translation_matrix = glm::mat4(1.f);
	translation_matrix[3][0] = translate_vector.x;
	translation_matrix[3][1] = translate_vector.y;
	translation_matrix[3][2] = translate_vector.z;
	transform(translation_matrix);
}

void Model::rotate(glm::vec3 angles)
{

	if (subModel)
	{
		subModel->rotate(angles);
	}

	glm::mat4 rotationX = glm::mat4(1.f);
	rotationX[1][1] = cosf(angles.x);
	rotationX[1][2] = sinf(angles.x);
	rotationX[2][1] = -sinf(angles.x);
	rotationX[2][2] = cosf(angles.x);
	glm::mat4 rotationY = glm::mat4(1.f);
	rotationY[0][0] = cosf(angles.y);
	rotationY[2][0] = sinf(angles.y);
	rotationY[0][2] = -sinf(angles.y);
	rotationY[2][2] = cosf(angles.y);
	glm::mat4 rotationZ = glm::mat4(1.f);
	rotationZ[0][0] = cosf(angles.z);
	rotationZ[0][1] = sinf(angles.z);
	rotationZ[1][0] = -sinf(angles.z);
	rotationZ[1][1] = cosf(angles.z);

	transform(rotationX * rotationY * rotationZ);
}

std::vector<glm::vec4> Model::applyMatrixSubModel(glm::mat4 matrix)
{
	if (subModel)
	{
		return subModel->applyMatrix(matrix);
	}
}

void Model::reset(glm::mat4 with)
{
	transformation_matrix = with;
	if (subModel)
	{
		subModel->reset(with);
	}
}

void Cube::scale(glm::vec3 scale_vector)
{
	scale_vector.x = std::max(scale_vector.x, 0.01f);
	scale_vector.y = std::max(scale_vector.y, 0.01f);
	scale_vector.z = std::max(scale_vector.z, 0.01f);
	if (scale_amount.x < 0.01)
	{
		scale_vector.x = std::max(1.f, scale_vector.x);
	}
	if (scale_amount.y < 0.01)
	{
		scale_vector.y = std::max(1.f, scale_vector.y);
	}
	if (scale_amount.z < 0.01)
	{
		scale_vector.z = std::max(1.f, scale_vector.z);
	}
	scale_amount.x *= scale_vector.x;
	scale_amount.y *= scale_vector.y;
	scale_amount.z *= scale_vector.z;

	glm::mat4 scale_matrix = glm::mat4(1.f);
	scale_matrix[0][0] = scale_vector.x;
	scale_matrix[1][1] = scale_vector.y;
	scale_matrix[2][2] = scale_vector.z;

	transform(scale_matrix);
}

Cube::Cube() : Model(), scale_amount({1, 1, 1})
{
	this->subModel = new Gnomon();
	this->vertices = {
		{-1.f, -1.f, -1.f, 1.f},
		{-1.f, 1.f, -1.f, 1.f},
		{1.f, 1.f, -1.f, 1.f},
		{1.f, -1.f, -1.f, 1.f},
		{-1.f, -1.f, 1.f, 1.f},
		{-1.f, 1.f, 1.f, 1.f},
		{1.f, 1.f, 1.f, 1.f},
		{1.f, -1.f, 1.f, 1.f},
	};
}

Cube::~Cube()
{
	delete this->subModel;
}

Gnomon::Gnomon()
{
	this->vertices = {
		{0.f, 0.f, 0.f, 1.f},
		{1.f, 0.f, 0.f, 1.f},
		{0.f, 1.f, 0.f, 1.f},
		{0.f, 0.f, 1.f, 1.f},
	};
}

Gnomon::~Gnomon()
{
}

ViewMatrix::ViewMatrix(glm::mat4 initial_matrix)
{
	transformation_matrix = initial_matrix;
}
ViewMatrix::ViewMatrix()
{
}

ViewMatrix::~ViewMatrix() {}

glm::mat4 ViewMatrix::getTransform()
{
	return transformation_matrix;
}
