// Termm-Fall 2024

#include "A3.hpp"
#include "scene_lua.hpp"
using namespace std;

#include "cs488-framework/GlErrorCheck.hpp"
#include "cs488-framework/MathUtils.hpp"
#include "GeometryNode.hpp"
#include "JointNode.hpp"

#include <imgui/imgui.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

static bool show_gui = true;

const size_t CIRCLE_PTS = 48;

static const int POSITION_ORIENTATION = 0;
static const int JOINTS = 1;

State::State(JointNode *node) : node(node), transform(node->get_transform()), x_count(node->x_count), y_count(node->y_count), z_count(node->z_count)
{
}

State::~State() {};
//----------------------------------------------------------------------------------------
// Constructor
A3::A3(const std::string &luaSceneFile)
	: m_luaSceneFile(luaSceneFile),
	  m_positionAttribLocation(0),
	  m_normalAttribLocation(0),
	  m_vao_meshData(0),
	  m_vbo_vertexPositions(0),
	  m_vbo_vertexNormals(0),
	  m_vao_arcCircle(0),
	  m_vbo_arcCircle(0),
	  frontface_culling(false),
	  backface_culling(false),
	  circle(false),
	  z_buffer(true),
	  scene_translation(1.f),
	  scene_rotation(1.f),
	  scene_scale(1.f),
	  starting_trackball({-2.f, -2.f, -2.f}),
	  rotating_joints(false),
	  undo_set(false)

{
}

//----------------------------------------------------------------------------------------
// Destructor
A3::~A3()
{
}

//----------------------------------------------------------------------------------------
/*
 * Called once, at program start.
 */
void A3::init()
{
	// Set the background colour.
	glClearColor(0.2, 0.5, 0.3, 1.0);

	createShaderProgram();

	glGenVertexArrays(1, &m_vao_arcCircle);
	glGenVertexArrays(1, &m_vao_meshData);
	enableVertexShaderInputSlots();

	processLuaSceneFile(m_luaSceneFile);

	// Load and decode all .obj files at once here.  You may add additional .obj files to
	// this list in order to support rendering additional mesh types.  All vertex
	// positions, and normals will be extracted and stored within the MeshConsolidator
	// class.
	unique_ptr<MeshConsolidator> meshConsolidator(new MeshConsolidator{
		getAssetFilePath("cube.obj"),
		getAssetFilePath("sphere.obj"),
		getAssetFilePath("suzanne.obj")});

	// Acquire the BatchInfoMap from the MeshConsolidator.
	meshConsolidator->getBatchInfoMap(m_batchInfoMap);

	// Take all vertex data within the MeshConsolidator and upload it to VBOs on the GPU.
	uploadVertexDataToVbos(*meshConsolidator);

	mapVboDataToVertexShaderInputLocations();

	initPerspectiveMatrix();

	initViewMatrix();

	initLightSources();

	// Exiting the current scope calls delete automatically on meshConsolidator freeing
	// all vertex data resources.  This is fine since we already copied this data to
	// VBOs on the GPU.  We have no use for storing vertex data on the CPU side beyond
	// this point.
	radius = 0.25f * std::min(m_windowHeight, m_windowWidth);
}

void set_all_initial(SceneNode &root)
{
	if (root.m_nodeType == NodeType::JointNode)
	{
		JointNode *cur_node = static_cast<JointNode *>(&root);
		cur_node->set_initial();
	}

	for (auto child : root.children)
	{
		set_all_initial(*child);
	}
}

//----------------------------------------------------------------------------------------
void A3::processLuaSceneFile(const std::string &filename)
{
	// This version of the code treats the Lua file as an Asset,
	// so that you'd launch the program with just the filename
	// of a puppet in the Assets/ directory.
	// std::string assetFilePath = getAssetFilePath(filename.c_str());
	// m_rootNode = std::shared_ptr<SceneNode>(import_lua(assetFilePath));

	// This version of the code treats the main program argument
	// as a straightforward pathname.
	m_rootNode = std::shared_ptr<SceneNode>(import_lua(filename));
	if (!m_rootNode)
	{
		std::cerr << "Could Not Open " << filename << std::endl;
	}
	set_all_initial(*m_rootNode);
}

//----------------------------------------------------------------------------------------
void A3::createShaderProgram()
{
	m_shader.generateProgramObject();
	m_shader.attachVertexShader(getAssetFilePath("VertexShader.vs").c_str());
	m_shader.attachFragmentShader(getAssetFilePath("FragmentShader.fs").c_str());
	m_shader.link();

	m_shader_arcCircle.generateProgramObject();
	m_shader_arcCircle.attachVertexShader(getAssetFilePath("arc_VertexShader.vs").c_str());
	m_shader_arcCircle.attachFragmentShader(getAssetFilePath("arc_FragmentShader.fs").c_str());
	m_shader_arcCircle.link();
}

//----------------------------------------------------------------------------------------
void A3::enableVertexShaderInputSlots()
{
	//-- Enable input slots for m_vao_meshData:
	{
		glBindVertexArray(m_vao_meshData);

		// Enable the vertex shader attribute location for "position" when rendering.
		m_positionAttribLocation = m_shader.getAttribLocation("position");
		glEnableVertexAttribArray(m_positionAttribLocation);

		// Enable the vertex shader attribute location for "normal" when rendering.
		m_normalAttribLocation = m_shader.getAttribLocation("normal");
		glEnableVertexAttribArray(m_normalAttribLocation);

		CHECK_GL_ERRORS;
	}

	//-- Enable input slots for m_vao_arcCircle:
	{
		glBindVertexArray(m_vao_arcCircle);

		// Enable the vertex shader attribute location for "position" when rendering.
		m_arc_positionAttribLocation = m_shader_arcCircle.getAttribLocation("position");
		glEnableVertexAttribArray(m_arc_positionAttribLocation);

		CHECK_GL_ERRORS;
	}

	// Restore defaults
	glBindVertexArray(0);
}

//----------------------------------------------------------------------------------------
void A3::uploadVertexDataToVbos(
	const MeshConsolidator &meshConsolidator)
{
	// Generate VBO to store all vertex position data
	{
		glGenBuffers(1, &m_vbo_vertexPositions);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexPositions);

		glBufferData(GL_ARRAY_BUFFER, meshConsolidator.getNumVertexPositionBytes(),
					 meshConsolidator.getVertexPositionDataPtr(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}

	// Generate VBO to store all vertex normal data
	{
		glGenBuffers(1, &m_vbo_vertexNormals);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexNormals);

		glBufferData(GL_ARRAY_BUFFER, meshConsolidator.getNumVertexNormalBytes(),
					 meshConsolidator.getVertexNormalDataPtr(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}

	// Generate VBO to store the trackball circle.
	{
		glGenBuffers(1, &m_vbo_arcCircle);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_arcCircle);

		float *pts = new float[2 * CIRCLE_PTS];
		for (size_t idx = 0; idx < CIRCLE_PTS; ++idx)
		{
			float ang = 2.0 * M_PI * float(idx) / CIRCLE_PTS;
			pts[2 * idx] = cos(ang);
			pts[2 * idx + 1] = sin(ang);
		}

		glBufferData(GL_ARRAY_BUFFER, 2 * CIRCLE_PTS * sizeof(float), pts, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}
}

//----------------------------------------------------------------------------------------
void A3::mapVboDataToVertexShaderInputLocations()
{
	// Bind VAO in order to record the data mapping.
	glBindVertexArray(m_vao_meshData);

	// Tell GL how to map data from the vertex buffer "m_vbo_vertexPositions" into the
	// "position" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexPositions);
	glVertexAttribPointer(m_positionAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	// Tell GL how to map data from the vertex buffer "m_vbo_vertexNormals" into the
	// "normal" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexNormals);
	glVertexAttribPointer(m_normalAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	//-- Unbind target, and restore default values:
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	CHECK_GL_ERRORS;

	// Bind VAO in order to record the data mapping.
	glBindVertexArray(m_vao_arcCircle);

	// Tell GL how to map data from the vertex buffer "m_vbo_arcCircle" into the
	// "position" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_arcCircle);
	glVertexAttribPointer(m_arc_positionAttribLocation, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

	//-- Unbind target, and restore default values:
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
void A3::initPerspectiveMatrix()
{
	float aspect = ((float)m_windowWidth) / m_windowHeight;
	m_perpsective = glm::perspective(degreesToRadians(60.0f), aspect, 0.1f, 100.0f);
}

//----------------------------------------------------------------------------------------
void A3::initViewMatrix()
{
	m_view = glm::lookAt(vec3(0.0f, 0.0f, 10.0f), vec3(0.0f, 0.0f, -1.0f),
						 vec3(0.0f, 1.0f, 0.0f));
}

//----------------------------------------------------------------------------------------
void A3::initLightSources()
{
	// World-space position
	m_light.position = vec3(10.0f, 10.0f, 10.0f);
	m_light.rgbIntensity = vec3(1.0f); // light
}

//----------------------------------------------------------------------------------------
void A3::uploadCommonSceneUniforms()
{
	m_shader.enable();
	{
		//-- Set Perpsective matrix uniform for the scene:
		GLint location = m_shader.getUniformLocation("Perspective");
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(m_perpsective));
		CHECK_GL_ERRORS;

		if (!do_picking)
		{
			//-- Set LightSource uniform for the scene:
			{
				location = m_shader.getUniformLocation("light.position");
				glUniform3fv(location, 1, value_ptr(m_light.position));
				location = m_shader.getUniformLocation("light.rgbIntensity");
				glUniform3fv(location, 1, value_ptr(m_light.rgbIntensity));
				CHECK_GL_ERRORS;
			}

			//-- Set background light ambient intensity
			{
				location = m_shader.getUniformLocation("ambientIntensity");
				vec3 ambientIntensity(0.25f);
				glUniform3fv(location, 1, value_ptr(ambientIntensity));
				CHECK_GL_ERRORS;
			}
		}
	}
	m_shader.disable();
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, before guiLogic().
 */
void A3::appLogic()
{
	// Place per frame, application logic here ...

	uploadCommonSceneUniforms();
}

void reset_orientation(SceneNode &root)
{
	if (root.m_nodeType == NodeType::JointNode)
	{
		JointNode *cur_node = static_cast<JointNode *>(&root);
		cur_node->initialize();
	}

	for (auto child : root.children)
	{
		reset_orientation(*child);
	}
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after appLogic(), but before the draw() method.
 */
void A3::guiLogic()
{
	if (!show_gui)
	{
		return;
	}

	static bool firstRun(true);
	if (firstRun)
	{
		ImGui::SetNextWindowPos(ImVec2(50, 50));
		firstRun = false;
	}

	static bool showDebugWindow(true);
	ImGuiWindowFlags windowFlags(ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_MenuBar);
	float opacity(0.5f);

	ImGui::Begin("Properties", &showDebugWindow, ImVec2(100, 100), opacity,
				 windowFlags);
	if (ImGui::RadioButton("Position/Orientation (P)", &interaction_mode, POSITION_ORIENTATION))
	{
	}
	if (ImGui::RadioButton("Joints (J)", &interaction_mode, JOINTS))
	{
	}
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("Application"))
		{
			if (ImGui::Button("Reset Position (I)"))
			{
				scene_translation = mat4();
			}
			if (ImGui::Button("Reset Orientation (O)"))
			{
				scene_rotation = mat4();
			}
			if (ImGui::Button("Reset Joints (S)"))
			{
				reset_orientation(*m_rootNode);
			}
			if (ImGui::Button("Reset All (A)"))
			{
			}
			if (ImGui::Button("Quit Application (Q)"))
			{
				glfwSetWindowShouldClose(m_window, GL_TRUE);
			}

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Edit"))
		{
			if (ImGui::Button("Undo (U)"))
			{
				undo();
			}
			if (ImGui::Button("Redo (R)"))
			{
				redo();
			}

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Options"))
		{
			if (ImGui::Checkbox("Circle (C)", &circle))
			{
			}
			if (ImGui::Checkbox("Frontface Culling (F)", &frontface_culling))
			{
			}
			if (ImGui::Checkbox("Backface Culling (B)", &backface_culling))
			{
			}
			if (ImGui::Checkbox("Z-Buffer (Z)", &z_buffer))
			{
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	// Add more gui elements here here ...

	// Create Button, and check if it was clicked:

	ImGui::Text("Undos left: %d", undo_stack.size());
	ImGui::Text("Redos left: %d", redo_stack.size());
	ImGui::Text("Framerate: %.1f FPS", ImGui::GetIO().Framerate);

	ImGui::End();
}

//----------------------------------------------------------------------------------------
// Update mesh specific shader uniforms:
static void updateShaderUniforms(
	const ShaderProgram &shader,
	const GeometryNode &node,
	const glm::mat4 &viewMatrix,
	bool do_picking,
	int interaction_mode)
{

	shader.enable();
	GLint location = shader.getUniformLocation("ModelView");
	mat4 modelView = viewMatrix * node.trans;
	glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(modelView));
	CHECK_GL_ERRORS;
	if (do_picking)
	{
		location = shader.getUniformLocation("picking");
		glUniform1i(location, 0);
		float r = float(node.m_nodeId & 0xff) / 255.0f;
		float g = float((node.m_nodeId >> 8) & 0xff) / 255.0f;
		float b = float((node.m_nodeId >> 16) & 0xff) / 255.0f;

		location = shader.getUniformLocation("material.kd");
		glUniform3f(location, r, g, b);
		CHECK_GL_ERRORS;
	}
	else
	{
		location = shader.getUniformLocation("picking");
		CHECK_GL_ERRORS;
		glUniform1i(location, 1);
		CHECK_GL_ERRORS;
		//-- Set ModelView matrix:

		//-- Set NormMatrix:
		location = shader.getUniformLocation("NormalMatrix");
		mat3 normalMatrix = glm::transpose(glm::inverse(mat3(modelView)));
		glUniformMatrix3fv(location, 1, GL_FALSE, value_ptr(normalMatrix));
		CHECK_GL_ERRORS;

		//-- Set Material values:
		location = shader.getUniformLocation("material.kd");
		vec3 kd = node.material.kd;
		if (node.parent && node.parent->isSelected && interaction_mode == JOINTS)
		{
			kd = glm::vec3(1.0, 1.0, 0.0);
		}
		glUniform3fv(location, 1, value_ptr(kd));
		CHECK_GL_ERRORS;
	}
	shader.disable();
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after guiLogic().
 */
void A3::draw()
{
	if (z_buffer)
	{
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
	}
	else
	{
		glDisable(GL_DEPTH_TEST);
	}
	if (backface_culling && frontface_culling)
	{
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT_AND_BACK);
	}
	else if (backface_culling)
	{
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
	}
	else if (frontface_culling)
	{
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
	}

	renderSceneGraph(*m_rootNode);
	if (circle)
	{
		renderArcCircle();
	}

	if (backface_culling || frontface_culling)
	{
		glDisable(GL_CULL_FACE);
	}
	if (z_buffer)
	{
		glDisable(GL_DEPTH_TEST);
	}
}

//----------------------------------------------------------------------------------------
void A3::renderSceneGraph(const SceneNode &root)
{
	// Bind the VAO once here, and reuse for all GeometryNode rendering below.
	glBindVertexArray(m_vao_meshData);

	// This is emphatically *not* how you should be drawing the scene graph in
	// your final implementation.  This is a non-hierarchical demonstration
	// in which we assume that there is a list of GeometryNodes living directly
	// underneath the root node, and that we can draw them in a loop.  It's
	// just enough to demonstrate how to get geometry and materials out of
	// a GeometryNode and onto the screen.

	// You'll want to turn this into recursive code that walks over the tree.
	// You can do that by putting a method in SceneNode, overridden in its
	// subclasses, that renders the subtree rooted at every node.  Or you
	// could put a set of mutually recursive functions in this class, which
	// walk down the tree from nodes of different types.

	renderSceneNode(root, root.trans);

	glBindVertexArray(0);
	CHECK_GL_ERRORS;
}

void A3::renderSceneNode(const SceneNode &root, mat4 transform)
{
	for (const SceneNode *node : root.children)
	{

		renderSceneNode(*node, transform * node->get_transform());
		if (node->m_nodeType != NodeType::GeometryNode)
		{
			continue;
		}

		const GeometryNode *geometryNode = static_cast<const GeometryNode *>(node);

		updateShaderUniforms(m_shader, *geometryNode, m_view * scene_translation * scene_rotation * scene_scale * transform, do_picking, interaction_mode);

		// Get the BatchInfo corresponding to the GeometryNode's unique MeshId.
		BatchInfo batchInfo = m_batchInfoMap[geometryNode->meshId];

		//-- Now render the mesh:
		m_shader.enable();
		glDrawArrays(GL_TRIANGLES, batchInfo.startIndex, batchInfo.numIndices);
		m_shader.disable();
	}
}

//----------------------------------------------------------------------------------------
// Draw the trackball circle.
void A3::renderArcCircle()
{
	glBindVertexArray(m_vao_arcCircle);

	m_shader_arcCircle.enable();
	GLint m_location = m_shader_arcCircle.getUniformLocation("M");
	float aspect = float(m_framebufferWidth) / float(m_framebufferHeight);
	glm::mat4 M;
	if (aspect > 1.0)
	{
		M = glm::scale(glm::mat4(), glm::vec3(0.5 / aspect, 0.5, 1.0));
	}
	else
	{
		M = glm::scale(glm::mat4(), glm::vec3(0.5, 0.5 * aspect, 1.0));
	}
	glUniformMatrix4fv(m_location, 1, GL_FALSE, value_ptr(M));
	glDrawArrays(GL_LINE_LOOP, 0, CIRCLE_PTS);
	m_shader_arcCircle.disable();

	glBindVertexArray(0);
	CHECK_GL_ERRORS;
}

void A3::translateModel(float depthChange, glm::vec2 lateralChange)
{
	glm::vec3 left_vector = glm::vec3(m_view[0][0], m_view[0][1], m_view[0][2]);
	glm::vec3 up_vector = glm::vec3(m_view[1][0], m_view[1][1], m_view[1][2]);
	glm::vec3 forward_vector = glm::vec3(-m_view[2][0], -m_view[2][1], -m_view[2][2]);

	scene_translation = glm::translate(scene_translation, left_vector * lateralChange.x);
	scene_translation = glm::translate(scene_translation, up_vector * lateralChange.y);
	scene_translation = glm::translate(scene_translation, forward_vector * depthChange);
}

void A3::reset()
{
	frontface_culling = false;
	backface_culling = false;
	circle = false;
	z_buffer = true;
	scene_scale = mat4();
	scene_rotation = mat4();
	starting_trackball = {-2.f, -1.f, -1.f};
}

//----------------------------------------------------------------------------------------
/*
 * Called once, after program is signaled to terminate.
 */
void A3::cleanup()
{
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles cursor entering the window area events.
 */
bool A3::cursorEnterWindowEvent(
	int entered)
{
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

void handleRotation(SceneNode &node, double angle, bool all_joints, bool rotate_head)
{
	bool child_selected = false;
	for (auto child : node.children)
	{
		handleRotation(*child, angle, all_joints, rotate_head);
		child_selected = child_selected || child->isSelected;
	}

	if (node.m_nodeType == NodeType::JointNode && child_selected)
	{
		if ((all_joints && node.m_name != "neck_joint_upper") || (rotate_head && node.m_name == "neck_joint_upper"))
		{
			JointNode *jointNode = static_cast<JointNode *>(&node);

			if (angle + jointNode->x_count < jointNode->m_joint_x.max && angle + jointNode->x_count > jointNode->m_joint_x.min)
			{
				node.rotate('x', angle);
			}
			else if (angle + jointNode->y_count < jointNode->m_joint_y.max && angle + jointNode->y_count > jointNode->m_joint_y.min)
			{
				node.rotate('y', angle);
			}
		}
	}
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse cursor movement events.
 */
bool A3::mouseMoveEvent(
	double xPos,
	double yPos)
{
	bool eventHandled(false);

	float x_diff = float(xPos - mouse_position.x) / 20;
	float y_diff = -float(yPos - mouse_position.y) / 20;

	if (interaction_mode == POSITION_ORIENTATION)
	{
		translateModel(y_diff * depth_translation, glm::vec2(x_diff, y_diff) * lateral_translation);
		// Fill in with event handling code...

		mouse_position = glm::vec2(xPos, yPos);

		glm::vec2 center = {m_windowWidth / 2, m_windowHeight / 2};

		float distance = glm::distance(center, mouse_position);

		if (tracking)
		{
			if (distance < radius)
			{
				glm::vec3 point_projected = glm::vec3(mouse_position.x - center.x, center.y - mouse_position.y, 0.f) / radius;
				float z_val = glm::sqrt(1 - glm::l2Norm(point_projected));
				point_projected.z = z_val;
				if (starting_trackball.x <= -2)
				{
					starting_trackball = point_projected;
				}
				else
				{

					glm::vec3 axis = glm::cross(glm::normalize(starting_trackball), glm::normalize(point_projected - starting_trackball));

					float alpha = 80.f;
					float theta = alpha * glm::l2Norm(point_projected - starting_trackball);
					axis = glm::vec3(glm::inverse(scene_rotation) * glm::vec4(axis, 1.f));
					scene_rotation = glm::rotate(scene_rotation, glm::radians(theta), axis);
					starting_trackball = point_projected;
				}
			}
			else
			{
				starting_trackball.x = -2;
				glm::vec3 axis = glm::vec3(0.f, 0.f, 1.f);
				axis = glm::vec3(glm::inverse(scene_rotation) * glm::vec4(axis, 1.f));
				float theta_rad = x_diff / 10;
				scene_rotation = glm::rotate(scene_rotation, theta_rad, axis);
			}
		}
		else
		{
			starting_trackball.x = -2;
		}
	}
	else if (interaction_mode == JOINTS)
	{
		mouse_position = glm::vec2(xPos, yPos);
		y_diff *= 5;
		if ((rotating_joints || rotating_head) && !undo_set)
		{
			auto states = getStates(*m_rootNode, rotating_joints, rotating_head);
			if (states.size())
			{
				undo_stack.push_back(states);
				redo_stack.clear();
			}
			undo_set = true;
		}
		handleRotation(*m_rootNode, double(y_diff * lateral_translation), rotating_joints, rotating_head);
	}

	return eventHandled;
}
//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse button events.
 */
bool A3::mouseButtonInputEvent(
	int button,
	int actions,
	int mods)
{
	bool eventHandled(false);

	float new_val = 0.f;
	if (actions == GLFW_PRESS)
	{
		new_val = 1.f;
		if (interaction_mode == JOINTS)
		{
			if (button == GLFW_MOUSE_BUTTON_LEFT)
			{
				double xpos, ypos;
				glfwGetCursorPos(m_window, &xpos, &ypos);
				do_picking = true;
				uploadCommonSceneUniforms();
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				draw();
				CHECK_GL_ERRORS;

				xpos *= double(m_framebufferWidth) / double(m_windowWidth);
				// WTF, don't know why I have to measure y relative to the bottom of
				// the window in this case.
				ypos = m_windowHeight - ypos;
				ypos *= double(m_framebufferHeight) / double(m_windowHeight);

				GLubyte buffer[4] = {0, 0, 0, 0};
				// A bit ugly -- don't want to swap the just-drawn false colours
				// to the screen, so read from the back buffer.
				glReadBuffer(GL_BACK);
				// Actually read the pixel at the mouse location.
				glReadPixels(int(xpos), int(ypos), 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
				CHECK_GL_ERRORS;

				// Reassemble the object ID.
				unsigned int what = buffer[0] + (buffer[1] << 8) + (buffer[2] << 16);

				selectNode(what);

				do_picking = false;

				CHECK_GL_ERRORS;
			}
			else if (button == GLFW_MOUSE_BUTTON_MIDDLE)
			{
				rotating_joints = true;
				undo_set = false;
			}
			else if (button == GLFW_MOUSE_BUTTON_RIGHT)
			{
				rotating_head = true;
				undo_set = false;
			}
		}
	}
	switch (button)
	{
	case GLFW_MOUSE_BUTTON_LEFT:
		if (interaction_mode == POSITION_ORIENTATION)
		{
			lateral_translation = new_val;
		}
		break;
	case GLFW_MOUSE_BUTTON_RIGHT:
		if (interaction_mode == POSITION_ORIENTATION)
		{
			tracking = !tracking;
			starting_trackball = glm::vec3(-2.f, -1.f, -1.f);
		}
		else if (interaction_mode == JOINTS)
		{
			lateral_translation = new_val;
			if (actions == GLFW_RELEASE)
			{
				rotating_head = false;
				undo_set = true;
			}
		}
		break;
	case GLFW_MOUSE_BUTTON_MIDDLE:
		if (interaction_mode == POSITION_ORIENTATION)
		{
			depth_translation = new_val;
		}
		else if (interaction_mode == JOINTS)
		{
			lateral_translation = new_val;
			if (actions == GLFW_RELEASE)
			{
				rotating_joints = false;
				undo_set = true;
			}
		}
	default:
		break;
	}
	// Fill in with event handling code...

	return eventHandled;
}

void selectNodeRecur(SceneNode &root, unsigned int id)
{
	if (root.m_nodeId == id)
	{
		if (root.parent && root.parent->parent)
		{
			if (root.parent->parent->m_nodeType == NodeType::JointNode)
			{
				const JointNode *joint_node = static_cast<const JointNode *>(root.parent->parent);
				if (joint_node->m_joint_x.min || joint_node->m_joint_x.max || joint_node->m_joint_y.min || joint_node->m_joint_y.max)
				{
					root.parent->isSelected = !root.parent->isSelected;
				}
			}
		}
	}
	else
	{
		for (auto child : root.children)
		{
			selectNodeRecur(*child, id);
		}
	}
}

void A3::selectNode(unsigned int id)
{
	selectNodeRecur(*m_rootNode, id);
}

std::vector<State> A3::getStates(SceneNode &root, bool all_joints, bool head_joint)
{
	std::vector<State> states;
	bool child_selected = false;
	for (auto child : root.children)
	{
		std::vector<State> child_states = getStates(*child, all_joints, head_joint);
		states.insert(states.end(), child_states.begin(), child_states.end());
		if (child->isSelected)
		{
			child_selected = true;
		}
	}
	if (root.m_nodeType == NodeType::JointNode && child_selected)
	{
		if ((all_joints && root.m_name != "neck_joint_upper") || (head_joint && root.m_name == "neck_joint_upper"))
		{
			JointNode *jointNode = static_cast<JointNode *>(&root);
			states.push_back(State(jointNode));
		}
	}
	return states;
}

void A3::setStates(std::vector<State> states)
{
	for (auto state : states)
	{
		state.node->set_transform(state.transform);
		state.node->x_count = state.x_count;
		state.node->y_count = state.y_count;
		state.node->z_count = state.z_count;
	}
}

void A3::undo()
{

	if (undo_stack.size() > 0)
	{
		redo_stack.push_back(getStates(*m_rootNode, true, true));
		setStates(undo_stack.back());
		undo_stack.pop_back();
	}
}

void A3::redo()
{
	if (redo_stack.size() > 0)
	{
		undo_stack.push_back(getStates(*m_rootNode, true, true));
		setStates(redo_stack.back());
		redo_stack.pop_back();
	}
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse scroll wheel events.
 */

bool A3::mouseScrollEvent(
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
bool A3::windowResizeEvent(
	int width,
	int height)
{
	bool eventHandled(false);
	initPerspectiveMatrix();
	radius = 0.25f * std::min(height, width);
	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles key input events.
 */
bool A3::keyInputEvent(
	int key,
	int action,
	int mods)
{
	bool eventHandled(false);

	if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_M)
		{
			show_gui = !show_gui;
			eventHandled = true;
		}
		if (key == GLFW_KEY_Z)
		{
			z_buffer = !z_buffer;
			eventHandled = true;
		}
		if (key == GLFW_KEY_B)
		{
			backface_culling = !backface_culling;
			eventHandled = true;
		}
		if (key == GLFW_KEY_F)
		{
			frontface_culling = !frontface_culling;
			eventHandled = true;
		}
		if (key == GLFW_KEY_LEFT_SHIFT)
		{
			mouseButtonInputEvent(GLFW_MOUSE_BUTTON_MIDDLE, GLFW_PRESS, 0);
			// depth_translation = 1.f;
		}
		if (key == GLFW_KEY_I)
		{
			scene_translation = mat4();
		}
		if (key == GLFW_KEY_O)
		{
			scene_rotation = mat4();
		}
		if (key == GLFW_KEY_J)
		{
			interaction_mode = JOINTS;
		}
		if (key == GLFW_KEY_P)
		{
			interaction_mode = POSITION_ORIENTATION;
			eventHandled = true;
		}
		if (key == GLFW_KEY_C)
		{
			circle = !circle;
		}
		if (key == GLFW_KEY_S)
		{
			reset_orientation(*m_rootNode);
			undo_stack.clear();
			redo_stack.clear();
		}
		if (key == GLFW_KEY_U)
		{
			undo();
		}
		if (key == GLFW_KEY_R)
		{
			redo();
		}
		if (key == GLFW_KEY_A)
		{
			scene_translation = mat4();
			scene_rotation = mat4();
			reset_orientation(*m_rootNode);
			undo_stack.clear();
			redo_stack.clear();
		}
	}

	if (action == GLFW_RELEASE)
	{
		if (key == GLFW_KEY_LEFT_SHIFT)
		{
			mouseButtonInputEvent(GLFW_MOUSE_BUTTON_MIDDLE, GLFW_RELEASE, 0);
		}
	}
	// Fill in with event handling code...

	return eventHandled;
}
