// Termm-Fall 2024

#pragma once

#include "cs488-framework/CS488Window.hpp"
#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/ShaderProgram.hpp"
#include "cs488-framework/MeshConsolidator.hpp"

#include "SceneNode.hpp"

#include "JointNode.hpp"

#include <glm/glm.hpp>
#include <memory>

class State
{
public:
	State(JointNode *node);
	~State();
	glm::mat4 transform;
	float x_count;
	float y_count;
	float z_count;
	JointNode *node;
};

struct LightSource
{
	glm::vec3 position;
	glm::vec3 rgbIntensity;
};

class A3 : public CS488Window
{
public:
	A3(const std::string &luaSceneFile);
	virtual ~A3();

protected:
	virtual void init() override;
	virtual void appLogic() override;
	virtual void guiLogic() override;
	virtual void draw() override;
	virtual void cleanup() override;

	//-- Virtual callback methods
	virtual bool cursorEnterWindowEvent(int entered) override;
	virtual bool mouseMoveEvent(double xPos, double yPos) override;
	virtual bool mouseButtonInputEvent(int button, int actions, int mods) override;
	virtual bool mouseScrollEvent(double xOffSet, double yOffSet) override;
	virtual bool windowResizeEvent(int width, int height) override;
	virtual bool keyInputEvent(int key, int action, int mods) override;

	//-- One time initialization methods:
	void processLuaSceneFile(const std::string &filename);
	void createShaderProgram();
	void enableVertexShaderInputSlots();
	void uploadVertexDataToVbos(const MeshConsolidator &meshConsolidator);
	void mapVboDataToVertexShaderInputLocations();
	void initViewMatrix();
	void initLightSources();

	void initPerspectiveMatrix();
	void uploadCommonSceneUniforms();
	void renderSceneGraph(const SceneNode &node);
	void renderSceneNode(const SceneNode &node, glm::mat4 transform);
	void renderArcCircle();
	void translateModel(float depthChange, glm::vec2 lateralChange);
	void reset();
	void selectNode(unsigned int id);
	std::vector<State> getStates(SceneNode &root, bool all_joints, bool head_joint);
	void setStates(std::vector<State> states);
	void undo();
	void redo();

	glm::mat4 m_perpsective;
	glm::mat4 m_view;

	LightSource m_light;

	//-- GL resources for mesh geometry data:
	GLuint m_vao_meshData;
	GLuint m_vbo_vertexPositions;
	GLuint m_vbo_vertexNormals;
	GLint m_positionAttribLocation;
	GLint m_normalAttribLocation;
	ShaderProgram m_shader;

	//-- GL resources for trackball circle geometry:
	GLuint m_vbo_arcCircle;
	GLuint m_vao_arcCircle;
	GLint m_arc_positionAttribLocation;
	ShaderProgram m_shader_arcCircle;

	// BatchInfoMap is an associative container that maps a unique MeshId to a BatchInfo
	// object. Each BatchInfo object contains an index offset and the number of indices
	// required to render the mesh with identifier MeshId.
	BatchInfoMap m_batchInfoMap;

	std::string m_luaSceneFile;

	std::shared_ptr<SceneNode> m_rootNode;
	bool frontface_culling;
	bool backface_culling;
	bool z_buffer;
	bool circle;
	int interaction_mode;
	glm::mat4 scene_translation;
	glm::mat4 scene_rotation;
	glm::mat4 scene_scale;
	float lateral_translation;
	float depth_translation;
	glm::vec2 mouse_position;
	float radius;
	bool tracking;
	glm::vec3 starting_trackball;
	bool do_picking;
	bool rotating_joints;
	bool rotating_head;
	bool undo_set;
	std::vector<std::vector<State>> undo_stack;
	std::vector<std::vector<State>> redo_stack;
};