// Termm--Fall 2024

#pragma once

#include "cs488-framework/CS488Window.hpp"
#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/ShaderProgram.hpp"

#include <glm/glm.hpp>

#include <vector>

// Set a global maximum number of vertices in order to pre-allocate VBO data
// in one shot, rather than reallocating each frame.
const GLsizei kMaxVertices = 1000;

// Convenience class for storing vertex data in CPU memory.
// Data should be copied over to GPU memory via VBO storage before rendering.
class VertexData
{
public:
	VertexData();

	std::vector<glm::vec2> positions;
	std::vector<glm::vec3> colours;
	GLuint index;
	GLsizei numVertices;
};

class Model
{
public:
	Model();
	virtual ~Model();
	std::vector<glm::vec4> getVertices();
	void transform(glm::mat4 transform_by_matrix);
	std::vector<glm::vec4> applyMatrix(glm::mat4 matrix);
	void translate(glm::vec3 translate_vector);
	void rotate(glm::vec3 angles);
	std::vector<glm::vec4> applyMatrixSubModel(glm::mat4 matrix);

protected:
	glm::mat4 transformation_matrix;
	std::vector<glm::vec4> vertices;
	Model *subModel;
};

class Gnomon : public Model
{
public:
	Gnomon();
	virtual ~Gnomon();
};

class Cube : public Model
{
public:
	Cube();
	virtual ~Cube();
	void scale(glm::vec3 scale_vector);

private:
	glm::vec3 scale_amount;
};

class A2 : public CS488Window
{
public:
	A2();
	virtual ~A2();

protected:
	virtual void init() override;
	virtual void appLogic() override;
	virtual void guiLogic() override;
	virtual void draw() override;
	virtual void cleanup() override;

	virtual bool cursorEnterWindowEvent(int entered) override;
	virtual bool mouseMoveEvent(double xPos, double yPos) override;
	virtual bool mouseButtonInputEvent(int button, int actions, int mods) override;
	virtual bool mouseScrollEvent(double xOffSet, double yOffSet) override;
	virtual bool windowResizeEvent(int width, int height) override;
	virtual bool keyInputEvent(int key, int action, int mods) override;

	void createShaderProgram();
	void enableVertexAttribIndices();
	void generateVertexBuffers();
	void mapVboDataToVertexAttributeLocation();
	void uploadVertexDataToVbos();

	void initLineData();

	void setLineColour(const glm::vec3 &colour);

	void drawLine(
		const glm::vec2 &v0,
		const glm::vec2 &v1);

	ShaderProgram m_shader;

	GLuint m_vao;			// Vertex Array Object
	GLuint m_vbo_positions; // Vertex Buffer Object
	GLuint m_vbo_colours;	// Vertex Buffer Object

	VertexData m_vertexData;

	glm::vec3 m_currentLineColour;

	Cube cube;

	glm::mat4 view_matrix;
	glm::mat4 projection_matrix;

	int current_change;
	glm::vec3 active_buttons;

	glm::vec2 mouse_position;
};
