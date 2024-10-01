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
	: m_currentLineColour(vec3(0.0f)), cube()
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

	view_matrix = glm::mat4();

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

	view_matrix = glm::mat4({rightVector.x, upVector.x, forwardVector.x, 0,
							 rightVector.y, upVector.y, forwardVector.y, 0,
							 rightVector.z, upVector.z, forwardVector.z, 0,
							 -translationX, -translationY, -translationZ, 1});
	std::cout << view_matrix << std::endl;

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

	auto verts = cube.applyMatrix(projection_matrix * view_matrix);

	for (auto point : verts)
	{
		// std::cout << point << std::endl;
	}

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
	drawLine(vec2(verts[1].x, verts[1].y), vec2(verts[5].x, verts[5].y));
	drawLine(vec2(verts[3].x, verts[3].y), vec2(verts[7].x, verts[7].y));
	drawLine(vec2(verts[2].x, verts[2].y), vec2(verts[6].x, verts[6].y));
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

	// Fill in with event handling code...

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

	// Fill in with event handling code...

	return eventHandled;
}

Model::Model()
{
	transformation_matrix = glm::mat4(0.25f);
	transformation_matrix[3][3] = 1.f;
	std::cout << transformation_matrix << std::endl;
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

Cube::Cube() : Model()
{
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
}