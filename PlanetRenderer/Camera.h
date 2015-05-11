#pragma once

#include "PlanetRenderer.h"
#include "SceneNode.h"

class Camera : public SceneNode
{
	public:

		Camera(glm::dvec3 &eye, glm::dvec3 &direction, glm::dvec3 &up, SceneNode* par);

		glm::dmat4& getCameraMatrix();

		void update();

	private:
		glm::dmat4 inverse;

};