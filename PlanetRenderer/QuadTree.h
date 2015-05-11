#pragma once

#include "PlanetRenderer.h"
#include "Planet.h"
#include "Camera.h"

typedef unsigned char ubyte;
typedef char byte;

void initQuad();
void useQuadTreeProgram();

extern int breaks;

class Planet;

class QuadTree
{
	public:
		QuadTree(glm::dvec3& cent, double width, byte face, double scale, Planet* plan);
		void render(Camera* cam, glm::dmat4& view);
		void update(glm::dvec3& cam, double distance);
		double lineDistance(glm::dvec3& point);

		~QuadTree();

	//private:
		QuadTree(QuadTree* parent, double width,
			glm::dvec3& center, double uvw, glm::dvec2& uv,
			byte side, glm::dvec2& luv_tex, double luvw);

		void smallRender(Camera* cam, glm::dmat4& view);

		void connectNeighbor(QuadTree* child, byte s);
		void genTexture();
		void generateSelf();
		void generateChildren();

		void splitNeighbors();
		void calcStatus();
		void updateState();

		glm::dvec3 center;
		glm::dvec3 sphereCenter;
		glm::dvec2 uv;
		glm::dmat4 toWorld;

		double scale;
		double width;
		double uw;
		QuadTree* children[4];
		QuadTree* neighbors[4];
		QuadTree* parent;
		Planet* planet;
		void split();

		glm::dvec2 localUV;
		double localUW;
		GLuint texture;
		GLuint vbo;

		byte ibo;
		byte lod;
		byte face;
		byte side;
		ubyte state;
		
		bool hasTexture;

		friend class Planet;
		friend void handleRequest();

};