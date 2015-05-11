#include "SceneNode.h"

SceneNode::SceneNode(SceneNode* nodeParent)
{
	parent = nodeParent;
	if (parent != NULL)
	{
		parent->addChild(this);
	}
	toWorld = glm::dmat4(1.0);
	toParent = glm::dmat4(1.0);
}

void SceneNode::setToParent(glm::dmat4& mat)
{
	toParent = glm::dmat4(mat);
}

void SceneNode::addChild(SceneNode* child)
{
	children.push_back(child);
}

void SceneNode::update()
{
	for (unsigned int i = 0; i < children.size(); i++)
	{
		SceneNode *c = children[i];
		c->toWorld = toWorld * c->toParent;
		c->update();
	}
}

void SceneNode::translate(glm::dvec3& translate)
{
	toParent = glm::translate(toParent, translate);
}

void SceneNode::rotate(glm::dvec3& rot)
{
	if (rot.x != 0.0)
	{
		toParent = glm::rotate(toParent, rot.x, glm::dvec3(1.0, 0.0, 0.0));
	}
	if (rot.y != 0.0)
	{
		toParent = glm::rotate(toParent, rot.y, glm::dvec3(0.0, 1.0, 0.0));
	}
	if (rot.z != 0.0)
	{
		toParent = glm::rotate(toParent, rot.z, glm::dvec3(0.0, 0.0, 1.0));
	}
}

void SceneNode::getPosition(glm::dvec3& pos)
{
	pos.x = toWorld[3][0];
	pos.y = toWorld[3][1];
	pos.z = toWorld[3][2];
}