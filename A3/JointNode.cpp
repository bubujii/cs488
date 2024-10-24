// Termm--Fall 2024

#include "JointNode.hpp"

#include "cs488-framework/MathUtils.hpp"

#include <iostream>
#include <sstream>
using namespace std;

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/transform.hpp>

using namespace glm;
//---------------------------------------------------------------------------------------
JointNode::JointNode(const std::string &name)
	: SceneNode(name)
{
	m_nodeType = NodeType::JointNode;
	x_count = 0;
	y_count = 0;
	z_count = 0;
}

//---------------------------------------------------------------------------------------
JointNode::~JointNode()
{
}
//---------------------------------------------------------------------------------------
void JointNode::set_joint_x(double min, double init, double max)
{
	m_joint_x.min = min;
	m_joint_x.init = init;
	m_joint_x.max = max;
}

//---------------------------------------------------------------------------------------
void JointNode::set_joint_y(double min, double init, double max)
{
	m_joint_y.min = min;
	m_joint_y.init = init;
	m_joint_y.max = max;
}

void JointNode::rotate(char axis, float angle)
{
	vec3 rot_axis;

	switch (axis)
	{
	case 'x':
		rot_axis = vec3(1, 0, 0);
		x_count += angle;
		break;
	case 'y':
		rot_axis = vec3(0, 1, 0);
		y_count += angle;
		break;
	case 'z':
		rot_axis = vec3(0, 0, 1);
		z_count += angle;
		break;
	default:
		break;
	}
	mat4 rot_matrix = glm::rotate(degreesToRadians(angle), rot_axis);
	set_transform(rot_matrix * trans);
}

void JointNode::set_initial()
{
	initial = get_transform();
	initial_x_count = x_count;
	initial_y_count = y_count;
	initial_z_count = z_count;
}

void JointNode::initialize()
{
	set_transform(initial);
	x_count = initial_x_count;
	y_count = initial_y_count;
	z_count = initial_z_count;
}
