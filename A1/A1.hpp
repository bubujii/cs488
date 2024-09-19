// Termm--Fall 2024

#pragma once

#include <glm/glm.hpp>
#include <ctime>
#include <chrono>

#include "cs488-framework/CS488Window.hpp"
#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/ShaderProgram.hpp"

#include "maze.hpp"

class A1 : public CS488Window
{
public:
    A1();
    virtual ~A1();

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

private:
    void initCubeIbo();
    void initGrid();
    void initCubes();
    void initAvatar();
    void positionAvatar();
    void resetAvatar();
    void resetBoard();
    void digBoard();
    void moveAvatar(int x, int y, bool with_force);

    Maze maze;

    // Fields related to the shader and uniforms.
    ShaderProgram m_shader;
    GLint P_uni;   // Uniform location for Projection matrix.
    GLint V_uni;   // Uniform location for View matrix.
    GLint M_uni;   // Uniform location for Model matrix.
    GLint col_uni; // Uniform location for cube colour.

    // Fields related to grid geometry.
    GLuint m_grid_vao; // Vertex Array Object
    GLuint m_grid_vbo; // Vertex Buffer Object

    GLuint m_cube_vao;
    GLuint m_cube_vbo;
    GLuint m_cube_ibo;
    glm::vec3 m_cube_color;

    GLuint m_avatar_vao;
    GLuint m_avatar_vbo;
    GLuint m_avatar_ibo;
    int number_of_indices;
    glm::vec3 m_avatar_color;
    glm::vec2 avatar_position;

    GLuint m_floor_vao;
    GLuint m_floor_vbo;
    glm::vec3 m_floor_color;

    // Matrices controlling the camera and projection.
    glm::mat4 proj;
    glm::mat4 view;

    GLfloat m_shape_rotation;

    int height;
    GLfloat scale;
    float colour[3];
    int current_col;

    bool with_force;
    glm::vec2 mouse_position;
    bool mousehold;
    GLfloat velocity_rotation;
    GLfloat velocity_on_release;

    std::chrono::system_clock::time_point time_of_release;
};
