#include "Planet.h"

#include <chrono>

using namespace glm;

void initPlanet()
{
	initQuad();
}

void Planet::update(dvec3& cam)
{
	breaks = 0;
	for (int i = 0; i < 6; i++)
	{
		tree[i]->update(cam, getSurfaceDistance(cam));
	}
}

void Planet::initQuadTree()
{
	dvec3 cent = dvec3(center.x - radius, center.y, center.z);
	tree[NEG_X_FACE] = new QuadTree(cent, 2 * radius, NEG_X_FACE, radius, this);

	cent = dvec3(center.x, center.y - radius, center.z);
	tree[NEG_Y_FACE] = new QuadTree(cent, 2 * radius, NEG_Y_FACE, radius, this);

	cent = dvec3(center.x, center.y, center.z - radius);
	tree[NEG_Z_FACE] = new QuadTree(cent, 2 * radius, NEG_Z_FACE, radius, this);

	cent = dvec3(center.x + radius, center.y, center.z);
	tree[POS_X_FACE] = new QuadTree(cent, 2 * radius, POS_X_FACE, radius, this);

	cent = dvec3(center.x, center.y + radius, center.z);
	tree[POS_Y_FACE] = new QuadTree(cent, 2 * radius, POS_Y_FACE, radius, this);

	cent = dvec3(center.x, center.y, center.z + radius);
	tree[POS_Z_FACE] = new QuadTree(cent, 2 * radius, POS_Z_FACE, radius, this);

}

Planet::Planet(glm::dvec3 cent, double r, SceneNode* parent) : SceneNode(parent)
{
	radius = r;
	for (int i = 0; i < 6; i++)
	{
		tree[i] = NULL;
	}
	center = glm::dvec3(cent);
	reqs[0].data = tempData[0];
	reqs[1].data = tempData[1];
	reqs[2].data = tempData[2];
	reqs[3].data = tempData[3];

	initQuadTree();
}

void Planet::render(Camera* cam, dmat4& view)
{
	int w = glutGet(GLUT_WINDOW_WIDTH);
	int h = glutGet(GLUT_WINDOW_HEIGHT);
	glViewport(0, 0, w, h);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	useQuadTreeProgram();
	for (int i = 0; i < 6; i++)
	{
		tree[i]->render(cam, view);
	}
}

double Planet::getSurfaceDistance(glm::dvec3 point)
{
	glm::dvec3 pos = center + radius*normalize(point - center);
	return distance(pos, point);
}

