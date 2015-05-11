#include "Camera.h"

Camera::Camera(glm::dvec3 &eye, glm::dvec3 &direction,
	glm::dvec3 &up, SceneNode* par) : SceneNode(par)
{
	glm::dvec3 negDir = -direction;
	glm::dvec3 right = glm::cross(up, negDir);
	toParent[0] = glm::vec4(glm::normalize(right), 0);
	toParent[1] = glm::vec4(glm::normalize(up), 0);
	toParent[2] = glm::vec4(glm::normalize(negDir), 0);
	toParent[3] = glm::vec4(eye, 1);
}

void Camera::update()
{
	SceneNode::update();
	inverse = glm::inverse(toWorld);
}

glm::dmat4& Camera::getCameraMatrix()
{
	return inverse;
}