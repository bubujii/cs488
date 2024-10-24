// Termm--Fall 2024

#pragma once

#include "SceneNode.hpp"

class JointNode : public SceneNode
{
public:
	JointNode(const std::string &name);
	virtual ~JointNode();

	void set_joint_x(double min, double init, double max);
	void set_joint_y(double min, double init, double max);

	void rotate(char axis, float angle) override;
	float x_count;
	float y_count;
	float z_count;
	void set_initial();
	void initialize();

	struct JointRange
	{
		double min, init, max;
	};

	JointRange m_joint_x, m_joint_y;
	glm::mat4 initial;
	float initial_x_count;
	float initial_y_count;
	float initial_z_count;
};
