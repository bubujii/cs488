// Termm--Fall 2024

#include "A1.hpp"
#include "cs488-framework/GlErrorCheck.hpp"

#include <iostream>

#include <sys/types.h>
#include <unistd.h>
#include <vector>

#include <imgui/imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;
using namespace std;

static const size_t DIM = 16;

//----------------------------------------------------------------------------------------
// Constructor
A1::A1() : current_col(0),
		   maze(DIM),
		   m_cube_color({1.0f, 0.f, 0.f}),
		   m_floor_color({1.0f, 1.0f, 0.0f}),
		   m_avatar_color({0.0f, 1.0f, 0.0f}),
		   m_shape_rotation(0.0f),
		   avatar_position({0.5f, 0.5f}),
		   height(1),
		   scale(0),
		   with_force(false)
{
	colour[0] = m_cube_color.r;
	colour[1] = m_cube_color.g;
	colour[2] = m_cube_color.b;
}

//----------------------------------------------------------------------------------------
// Destructor
A1::~A1()
{
}

//----------------------------------------------------------------------------------------
/*
 * Called once, at program start.
 */
void A1::init()
{
	// Initialize random number generator
	int rseed = getpid();
	srandom(rseed);
	// Print random number seed in case we want to rerun with
	// same random numbers
	cout << "Random number seed = " << rseed << endl;

	// DELETE FROM HERE...
	maze.reset();

	// ...TO HERE

	// Set the background colour.
	glClearColor(0.3, 0.5, 0.7, 1.0);

	// Build the shader
	m_shader.generateProgramObject();
	m_shader.attachVertexShader(
		getAssetFilePath("VertexShader.vs").c_str());
	m_shader.attachFragmentShader(
		getAssetFilePath("FragmentShader.fs").c_str());
	m_shader.link();

	// Set up the uniforms
	P_uni = m_shader.getUniformLocation("P");
	V_uni = m_shader.getUniformLocation("V");
	M_uni = m_shader.getUniformLocation("M");
	col_uni = m_shader.getUniformLocation("colour");

	initGrid();
	initCubes();
	initCubeIbo();
	initAvatar();

	// Set up initial view and projection matrices (need to do this here,
	// since it depends on the GLFW window being set up correctly).
	view = glm::lookAt(
		glm::vec3(0.0f, 2. * float(DIM) * 2.0 * M_SQRT1_2, float(DIM) * 2.0 * M_SQRT1_2),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f));

	proj = glm::perspective(
		glm::radians(30.0f),
		float(m_framebufferWidth) / float(m_framebufferHeight),
		1.0f, 1000.0f);
}

void A1::initCubeIbo()
{

	GLushort indices[] = {
		0, 1, 2, 2, 3, 0,

		0, 1, 5, 5, 4, 0,

		4, 5, 6, 6, 7, 4,

		7, 6, 2, 2, 3, 7,

		1, 5, 6, 6, 2, 1,

		0, 4, 7, 7, 3, 0

	};
	GLushort *save_indices = new GLushort[36];
	for (int i = 0; i < 36; ++i)
	{
		save_indices[i] = indices[i];
	}

	glBindVertexArray(m_cube_vao);
	glGenBuffers(1, &m_cube_ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_cube_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 36 * sizeof(GLushort), save_indices, GL_STATIC_DRAW);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	delete[] save_indices;
	CHECK_GL_ERRORS;
}

void A1::initCubes()
{

	GLfloat sidelength = 1;

	GLfloat vertices[] = {
		-0.5f * sidelength,
		-0.5f * sidelength,
		-0.5f * sidelength,
		-0.5f * sidelength,
		0.5f * sidelength,
		-0.5f * sidelength,
		0.5f * sidelength,
		0.5f * sidelength,
		-0.5f * sidelength,
		0.5f * sidelength,
		-0.5f * sidelength,
		-0.5f * sidelength,
		-0.5f * sidelength,
		-0.5f * sidelength,
		0.5f * sidelength,
		-0.5f * sidelength,
		0.5f * sidelength,
		0.5f * sidelength,
		0.5f * sidelength,
		0.5f * sidelength,
		0.5f * sidelength,
		0.5f * sidelength,
		-0.5f * sidelength,
		0.5f * sidelength,
	};

	glGenVertexArrays(1, &m_cube_vao);
	glBindVertexArray(m_cube_vao);
	glGenBuffers(1, &m_cube_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_cube_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	GLint posAttrib = m_shader.getAttribLocation("position");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	CHECK_GL_ERRORS;
}

void A1::initGrid()
{
	size_t sz = 3 * 2 * 2 * (DIM + 3);

	float *verts = new float[sz];
	size_t ct = 0;
	for (int idx = 0; idx < DIM + 3; ++idx)
	{
		verts[ct] = -1;
		verts[ct + 1] = 0;
		verts[ct + 2] = idx - 1;
		verts[ct + 3] = DIM + 1;
		verts[ct + 4] = 0;
		verts[ct + 5] = idx - 1;
		ct += 6;

		verts[ct] = idx - 1;
		verts[ct + 1] = 0;
		verts[ct + 2] = -1;
		verts[ct + 3] = idx - 1;
		verts[ct + 4] = 0;
		verts[ct + 5] = DIM + 1;
		ct += 6;
	}

	// Create the vertex array to record buffer assignments.
	glGenVertexArrays(1, &m_grid_vao);
	glBindVertexArray(m_grid_vao);

	// Create the cube vertex buffer
	glGenBuffers(1, &m_grid_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_grid_vbo);
	glBufferData(GL_ARRAY_BUFFER, sz * sizeof(float),
				 verts, GL_STATIC_DRAW);

	// Specify the means of extracting the position values properly.
	GLint posAttrib = m_shader.getAttribLocation("position");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	glGenVertexArrays(1, &m_floor_vao);
	glBindVertexArray(m_floor_vao);

	GLfloat floor_vertices[] = {
		0,
		0,
		0,
		0,
		0,
		DIM,
		DIM,
		0,
		DIM,
		DIM,
		0,
		DIM,
		DIM,
		0,
		0,
		0,
		0,
		0,

	};
	glGenBuffers(1, &m_floor_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_floor_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(floor_vertices), floor_vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	// Reset state to prevent rogue code from messing with *my*
	// stuff!
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// OpenGL has the buffer now, there's no need for us to keep a copy.
	delete[] verts;

	CHECK_GL_ERRORS;
}

void A1::initAvatar()
{
	int longitude_count = 180;
	int latitude_count = 90;
	float radius = 0.4;
	std::vector<float> vertices;

	float x, y, z;

	float longitude_increment = 2 * M_PI / longitude_count;
	float latitude_increment = M_PI / latitude_count;
	float horizontal_angle, vertical_angle;

	for (int i = 0; i <= latitude_count; ++i)
	{
		vertical_angle = M_PI / 2 - i * latitude_increment;
		z = radius * sinf(vertical_angle);

		for (int j = 0; j <= longitude_count; ++j)
		{
			horizontal_angle = j * longitude_increment;

			x = radius * cosf(vertical_angle) * cosf(horizontal_angle); // r * cos(u) * cos(v)
			y = radius * cosf(vertical_angle) * sinf(horizontal_angle); // r * cos(u) * sin(v)
			vertices.push_back(x);
			vertices.push_back(y);
			vertices.push_back(z);
		}
	}

	std::vector<int> indices;
	int k1, k2;
	for (int i = 0; i < latitude_count; ++i)
	{
		k1 = i * (longitude_count + 1); // add one to account for pole
		k2 = k1 + longitude_count + 1;	// add one to account for pole

		for (int j = 0; j < longitude_count; ++j, ++k1, ++k2)
		{

			if (i != 0) // called once if at bottom
			{
				indices.push_back(k1);
				indices.push_back(k2);
				indices.push_back(k1 + 1);
			}

			if (i != (latitude_count - 1)) // called once if at top
			{
				indices.push_back(k1 + 1);
				indices.push_back(k2);
				indices.push_back(k2 + 1);
			}
		}
	}

	int vertices_sz = vertices.size();
	GLfloat vertices_save[vertices_sz];

	for (int i = 0; i < vertices_sz; ++i)
	{
		vertices_save[i] = vertices[i];
	}

	int indices_sz = indices.size();
	GLuint indices_save[indices_sz];

	number_of_indices = indices_sz;

	for (int i = 0; i < indices_sz; ++i)
	{
		indices_save[i] = indices[i];
	}

	glGenVertexArrays(1, &m_avatar_vao);
	glBindVertexArray(m_avatar_vao);

	glGenBuffers(1, &m_avatar_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_avatar_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_save), vertices_save, GL_STATIC_DRAW);
	GLint posAttrib = m_shader.getAttribLocation("position");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	glGenBuffers(1, &m_avatar_ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_avatar_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices_save), indices_save, GL_STATIC_DRAW);

	glBindVertexArray(0);
	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, before guiLogic().
 */
void A1::appLogic()
{
	// Place per frame, application logic here ...
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after appLogic(), but before the draw() method.
 */
void A1::guiLogic()
{
	// We already know there's only going to be one window, so for
	// simplicity we'll store button states in static local variables.
	// If there was ever a possibility of having multiple instances of
	// A1 running simultaneously, this would break; you'd want to make
	// this into instance fields of A1.
	static bool showTestWindow(false);
	static bool showDebugWindow(true);

	ImGuiWindowFlags windowFlags(ImGuiWindowFlags_AlwaysAutoResize);
	float opacity(0.5f);

	ImGui::Begin("Debug Window", &showDebugWindow, ImVec2(100, 100), opacity, windowFlags);
	if (ImGui::Button("Quit Application"))
	{
		glfwSetWindowShouldClose(m_window, GL_TRUE);
	}
	if (ImGui::Button("Dig Maze"))
	{
		maze.reset();
		maze.digMaze();
		maze.printMaze();
		positionAvatar();
	}
	if (ImGui::Button("Reset"))
	{
		maze.reset();
	}
	switch (current_col)
	{
	case 0:
		m_cube_color.r = colour[0];
		m_cube_color.g = colour[1];
		m_cube_color.b = colour[2];
		break;
	case 1:
		m_floor_color.r = colour[0];
		m_floor_color.g = colour[1];
		m_floor_color.b = colour[2];
		break;
	case 2:
		m_avatar_color.r = colour[0];
		m_avatar_color.g = colour[1];
		m_avatar_color.b = colour[2];
		break;

	default:
		break;
	}

	// Eventually you'll create multiple colour widgets with
	// radio buttons.  If you use PushID/PopID to give them all
	// unique IDs, then ImGui will be able to keep them separate.
	// This is unnecessary with a single colour selector and
	// radio button, but I'm leaving it in as an example.

	// Prefixing a widget name with "##" keeps it from being
	// displayed.
	ImGui::PushID(0);
	ImGui::ColorEditMode(ImGuiColorEditMode_RGB);
	ImGui::ColorEdit3("Colour", colour);
	ImGui::SameLine();
	if (ImGui::RadioButton("Walls", &current_col, 0))
	{
		colour[0] = m_cube_color.r;
		colour[1] = m_cube_color.g;
		colour[2] = m_cube_color.b;
	}
	if (ImGui::RadioButton("Floors", &current_col, 1))
	{
		colour[0] = m_floor_color.r;
		colour[1] = m_floor_color.g;
		colour[2] = m_floor_color.b;
	}
	if (ImGui::RadioButton("Avatar", &current_col, 2))
	{
		colour[0] = m_avatar_color.r;
		colour[1] = m_avatar_color.g;
		colour[2] = m_avatar_color.b;
	}
	ImGui::PopID();

	/*
			// For convenience, you can uncomment this to show ImGui's massive
			// demonstration window right in your application.  Very handy for
			// browsing around to get the widget you want.  Then look in
			// shared/imgui/imgui_demo.cpp to see how it's done.
			if( ImGui::Button( "Test Window" ) ) {
				showTestWindow = !showTestWindow;
			}
	*/

	ImGui::Text("Framerate: %.1f FPS", ImGui::GetIO().Framerate);

	ImGui::End();

	if (showTestWindow)
	{
		ImGui::ShowTestWindow(&showTestWindow);
	}
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after guiLogic().
 */
void A1::draw()
{
	// Create a global transformation for the model (centre it).
	mat4 W;
	mat4 cube_position;
	W = glm::translate(W, vec3(-float(DIM) / 2.0f, 0, -float(DIM) / 2.0f));
	mat4 scale_mat = glm::scale(view, vec3{1.f + scale, 1.f + scale, 1.f + scale});
	vec3 z_axis(0.0f, 1.0f, 0.0f);
	mat4 rotate = glm::rotate(mat4(), m_shape_rotation, z_axis);

	mat4 view_modified = scale_mat * rotate;
	m_shader.enable();
	glEnable(GL_DEPTH_TEST);

	glUniformMatrix4fv(P_uni, 1, GL_FALSE, value_ptr(proj));
	glUniformMatrix4fv(V_uni, 1, GL_FALSE, value_ptr(view_modified));
	glUniformMatrix4fv(M_uni, 1, GL_FALSE, value_ptr(W));

	glUniform3f(col_uni, m_floor_color.r, m_floor_color.g, m_floor_color.b);
	glBindVertexArray(m_floor_vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	// Just draw the grid for now.
	glBindVertexArray(m_grid_vao);

	glUniform3f(col_uni, 1, 1, 1);
	glDrawArrays(GL_LINES, 0, (3 + DIM) * 4);

	mat4 avatar_transform = glm::translate(W, vec3(avatar_position.x, 0.5f, avatar_position.y));
	glUniformMatrix4fv(M_uni, 1, GL_FALSE, value_ptr(avatar_transform));
	glUniform3f(col_uni, m_avatar_color.r, m_avatar_color.g, m_avatar_color.b);
	glBindVertexArray(m_avatar_vao);
	glDrawElements(GL_TRIANGLES, number_of_indices, GL_UNSIGNED_INT, 0);

	glm::vec2 test_arr[] = {
		{0, 0},
		{0, 1},
		{1, 0}};
	glBindVertexArray(m_cube_vao);
	glm::vec3 base = {0.5f, 0.5f, 0.5f};

	GLint uniformLocation_colour = m_shader.getUniformLocation("colour");

	// Set the uniform's value.
	for (int x = 0; x < DIM; ++x)
	{
		for (int z = 0; z < DIM; ++z)
		{
			if (maze.getValue(x, z) == 1)
			{
				for (int y = 0; y < height; ++y)
				{
					glUniform3f(uniformLocation_colour, m_cube_color.r, m_cube_color.g,
								m_cube_color.b);
					cube_position = glm::translate(W, vec3(x + base.x, y + base.y, z + base.z));
					glUniformMatrix4fv(M_uni, 1, GL_FALSE, value_ptr(cube_position));
					glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0);
				}
			}
		}
	}

	// Draw the cubes
	// Highlight the active square.
	m_shader.disable();

	// Restore defaults
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
/*
 * Called once, after program is signaled to terminate.
 */
void A1::cleanup()
{
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles cursor entering the window area events.
 */
bool A1::cursorEnterWindowEvent(
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
bool A1::mouseMoveEvent(double xPos, double yPos)
{
	bool eventHandled(false);

	if (!ImGui::IsMouseHoveringAnyWindow())
	{
		// Put some code here to handle rotations.  Probably need to
		// check whether we're *dragging*, not just moving the mouse.
		// Probably need some instance variables to track the current
		// rotation amount, and maybe the previous X position (so
		// that you can rotate relative to the *change* in X.
	}

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse button events.
 */
bool A1::mouseButtonInputEvent(int button, int actions, int mods)
{
	bool eventHandled(false);

	if (!ImGui::IsMouseHoveringAnyWindow())
	{
		// The user clicked in the window.  If it's the left
		// mouse button, initiate a rotation.
	}

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse scroll wheel events.
 */
bool A1::mouseScrollEvent(double xOffSet, double yOffSet)
{
	bool eventHandled(false);

	scale += yOffSet / 20;

	scale = std::min(std::max(scale, -0.7f), 1.3f);

	m_shape_rotation += xOffSet / 20;

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles window resize events.
 */
bool A1::windowResizeEvent(int width, int height)
{
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles key input events.
 */
bool A1::keyInputEvent(int key, int action, int mods)
{
	bool eventHandled(false);

	// Fill in with event handling code...
	if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_Q)
		{
			glfwSetWindowShouldClose(m_window, GL_TRUE);
		}
		if (key == GLFW_KEY_D)
		{
			maze.reset();
			maze.digMaze();
			maze.printMaze();
			positionAvatar();
		}
		if (key == GLFW_KEY_R)
		{
			maze.reset();
		}
		if (key == GLFW_KEY_SPACE)
		{
			height = std::min(height + 1, 6);
		}
		if (key == GLFW_KEY_BACKSPACE)
		{
			height = std::max(height - 1, 0);
		}
		if (key == GLFW_KEY_Y)
		{
			m_shape_rotation += 0.01f;
		}
		if (key == GLFW_KEY_RIGHT)
		{
			moveAvatar(1, 0, with_force);
		}
		if (key == GLFW_KEY_LEFT)
		{
			moveAvatar(-1, 0, with_force);
		}
		if (key == GLFW_KEY_UP)
		{
			moveAvatar(0, -1, with_force);
		}
		if (key == GLFW_KEY_DOWN)
		{
			moveAvatar(0, 1, with_force);
		}
		if (key == GLFW_KEY_LEFT_SHIFT)
		{
			with_force = true;
		}
		// Respond to some key events.
	}
	if (action == GLFW_RELEASE)
	{
		if (key == GLFW_KEY_LEFT_SHIFT)
		{
			with_force = false;
		}
	}

	return eventHandled;
}

void A1::positionAvatar()
{
	for (int row = 0; row < DIM; ++row)
	{
		if (maze.getValue(0, row) == 0)
		{
			avatar_position.x = 0.5f;
			avatar_position.y = row + 0.5f;
		}
	}
}

void A1::moveAvatar(int x, int y, bool with_force)
{
	if (maze.getValue(avatar_position.x + x, avatar_position.y + y))
	{
		if (with_force)
		{
			maze.setValue(avatar_position.x + x, avatar_position.y + y, 0);
			avatar_position.x += x;
			avatar_position.y += y;
		}
	}
	else
	{
		avatar_position.x += x;
		avatar_position.y += y;
	}
}