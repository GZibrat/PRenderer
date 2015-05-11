#pragma once
#include "SceneNode.h"
#include "Program.h"
#include "Camera.h"
#include "PlanetRenderer.h"
#include "QuadTree.h"
#include "QuadTreeManager.h"
#include <mutex>
#include <condition_variable>

class QuadTree;

struct QuadRequest
{
	QuadTree* tree;
	volatile bool done;
	std::mutex mutex;
	std::condition_variable cv;
	float* data;
};

void initPlanet();

class Planet : public SceneNode
{
	public:
		Planet();
		Planet(glm::dvec3 center, double radius, SceneNode* Parent);
		Planet(Planet& p) : SceneNode(this->parent) {};
		
		double getRadius();

		void update(glm::dvec3& cam);

		void render(Camera* cam, glm::dmat4& view);

		double getSurfaceDistance(glm::dvec3 point);

		QuadTree* tree[6];


	//private:
		void initQuadTree();
		glm::dvec3 center;
		double radius;

		float tempData[4][VBO_NUM_FLOATS];
		struct QuadRequest reqs[4];

		friend class QuadTree;
};