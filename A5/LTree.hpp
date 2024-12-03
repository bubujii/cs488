#include <map>
#include <stack>
#include <iostream>
#include "GeometryNode.hpp"

class LTree
{
public:
    LTree(const std::string &axiom, const std::map<char, std::string> &rules, int depth, double angle, double length = 1.0, double radius = 1.0)
    {
        m_axiom = axiom;
        m_rules = rules;
        m_depth = depth;
        m_angle = angle;
        m_length = length;
        m_radius = radius;
        generate_tree();
    }
    ~LTree() {}
    void generate_tree()
    {
        std::string current = m_axiom;
        for (int i = 0; i < m_depth; ++i)
        {
            std::string next = "";
            for (char c : current)
            {
                if (m_rules.find(c) != m_rules.end())
                {
                    next += m_rules[c];
                }
                else
                {
                    next += c;
                }
            }
            current = next;
        }
        final_structure = current;
    }

    GeometryNode *render_tree()
    {
        if (final_structure[0] == 'F')
        {
            final_structure = final_structure.substr(1);
        }
        GeometryNode *root = new GeometryNode("root", new Cylinder(m_radius, m_length, true));

        std::stack<GeometryNode *> state;
        for (char c : final_structure)
        {
            if (c == 'F' || c == 'A' || c == 'B' || c == 'C')
            {
                auto new_root = new GeometryNode("branch", new Cylinder(m_radius, m_length, true));
                root->add_child(new_root);
                root = new_root;
            }
            else if (c == '+')
            {
                root->rotate('x', m_angle);
                root->translate(glm::dvec3(0, 0.95 * m_length, 0));
            }
            else if (c == '-')
            {
                root->rotate('x', -m_angle);
                root->translate(glm::dvec3(0, 0.95 * m_length, 0));
            }
            else if (c == '&')
            {
                root->rotate('y', m_angle);
                root->translate(glm::dvec3(0, 0.95 * m_length, 0));
            }
            else if (c == '^')
            {
                root->rotate('y', -m_angle);
                root->translate(glm::dvec3(0, 0.95 * m_length, 0));
            }
            else if (c == '\\')
            {
                root->rotate('z', m_angle);
                root->translate(glm::dvec3(0, 0.95 * m_length, 0));
            }
            else if (c == '/')
            {
                root->rotate('z', -m_angle);
                root->translate(glm::dvec3(0, 0.95 * m_length, 0));
            }
            else if (c == '[')
            {
                state.push(root);
            }
            else if (c == ']')
            {
                root = state.top();
                state.pop();
            }
            else if (c == '|')
            {
                root->translate(glm::dvec3(0, 0.95 * m_length, 0));
            }
        }
        return root;
    }

private:
    std::string m_axiom;
    std::map<char, std::string> m_rules;
    int m_depth;
    double m_angle;
    double m_length;
    double m_radius;
    std::string final_structure;
};