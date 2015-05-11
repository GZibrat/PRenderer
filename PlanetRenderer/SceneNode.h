#pragma once

#include"PlanetRenderer.h"

#include <iostream>
#include <vector>

class SceneNode
{
	public:
		SceneNode(SceneNode* parent);
		void render(glm::dmat4& view, glm::dvec3& cam);
		virtual void update();
		void addChild(SceneNode* child);

		void translate(glm::dvec3& translate);
		void rotate(glm::dvec3& rotate);
		void mulAfter(glm::dmat4 mat);
		void mulBefore(glm::dmat4 mat);

		void setToParent(glm::dmat4& mat);

		void getPosition(glm::dvec3& pos);

	protected:
		glm::dmat4 toParent;
		glm::dmat4 toWorld;

		std::vector<SceneNode*> children;
		SceneNode* parent;
};