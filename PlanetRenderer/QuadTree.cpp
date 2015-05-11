#include "QuadTree.h"
#include <cmath>
#include "TextureShaders.h"
#include "Program.h"
#include "QuadTreeManager.h"
#include <chrono>


using namespace std::chrono;
using namespace glm;

Program perlin;
Program simplex;
Program quadtree;

GLfloat* buffer;
GLuint QTibo;
GLuint QTinstanceVBO;
GLuint currentTexture;
GLuint currentVBO;

int breaks = 0;

#define SIZE NODE_SIZE
#define SI NODE_SIZE_1
#define FPV NUM_FLOATS_PER_VERTEX

void useQuadTreeProgram()
{
	quadtree.useProgram();
}

void generateIndexes(GLuint* arr, int size)
{

	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			int idx = i*size * 6 + j * 6;
			int index = i*(size + 1) + j;
			arr[idx] = index;
			arr[idx + 1] = index + size + 2;
			arr[idx + 2] = index + 1;
			arr[idx + 3] = index;
			arr[idx + 4] = index + size + 1;
			arr[idx + 5] = index + size + 2;
		}
	}
}

void initQuad()
{
	perlin = Program("perlin.comp");
	simplex = Program("simplex.comp");
	quadtree = Program("bumpmap.vert", "planet.frag");

	int size = SIZE;
	int s = SI;

	GLuint* ind = new GLuint[s * s * 6];
	generateIndexes(ind, s);

	glGenBuffers(1, &QTibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, QTibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, s * s * 6 * sizeof(GLuint), ind, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//delete[] ind;
}

void QuadTree::genTexture()
{
	texture = getFreeTexture();
	this->hasTexture = true;
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, TEXTURE_SIZE, TEXTURE_SIZE, 0, GL_RGBA, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	int i = FI[face][0], j = FI[face][1], k = FI[face][2];
	double z = (double)FSI[face % 2];

	double w = uw / 2.0;
	dvec3 v00;
	v00[i] = (uv.x - w);
	v00[j] = (uv.y - w);
	v00[k] = z;
	dvec3 v10;
	v10[i] = (uv.x + w);
	v10[j] = (uv.y - w);
	v10[k] = z;
	dvec3 v11;
	v11[i] = (uv.x + w);
	v11[j] = (uv.y + w);
	v11[k] = z;
	dvec3 v01;
	v01[i] = (uv.x - w);
	v01[j] = (uv.y + w);
	v01[k] = z;

	perlinToTexture(perlin, TEXTURE_SIZE, TEXTURE_SIZE, v00, v10, v01, v11, texture);
}


QuadTree::QuadTree(dvec3& cent, double w, byte f, double s, Planet* plan)
{
	center = dvec3(cent);
	width = w;
	children[0] = NULL;
	parent = NULL;
	uv = dvec2(0, 0);
	uw = 2.0;
	face = f;
	lod = 0;
	localUV = dvec2(0,0);
	localUW = 1.0;
	side = -1;
	scale = s;
	planet = plan;
	generateSelf();
	hasTexture = false;
}

QuadTree::QuadTree(QuadTree* par, double w,
	glm::dvec3& cent, double uvw, glm::dvec2& uv_tex,
	byte s, glm::dvec2& luv_tex, double luvw)
{
	parent = par;
	planet = par->planet;
	face = par->face;
	uv = dvec2(uv_tex);
	center = dvec3(cent);
	width = w;
	uw = uvw;
	lod = par->lod + 1;
	localUV = dvec2(luv_tex);
	localUW = luvw;
	localUV = dvec2(0, 0);
	localUW = 1.0;
	children[0] = NULL;
	
	side = s;
	scale = par->scale;
	hasTexture = false;
}

void QuadTree::render(Camera* cam, dmat4& view)
{
	if (children[0] != NULL)
	{
		children[0]->render(cam, view);
		children[1]->render(cam, view);
		children[2]->render(cam, view);
		children[3]->render(cam, view);
		return;
	}

	dmat4 tcam = cam->getCameraMatrix() * toWorld;
	dvec4 local = toWorld * dvec4(0, 0, 0, 1);
	local = cam->getCameraMatrix() * local;
	local = local / local.w;
	tcam[3][0] = local.x;
	tcam[3][1] = local.y;
	tcam[3][2] = local.z;

	glEnable(GL_TEXTURE_2D);
	
	GLint myLoc = quadtree.getUniformLocation("cam");
	glUniformMatrix4fv(myLoc, 1, GL_FALSE, glm::value_ptr(glm::mat4(tcam)));
	myLoc = quadtree.getUniformLocation("view");
	glUniformMatrix4fv(myLoc, 1, GL_FALSE, glm::value_ptr(glm::mat4(view)));

	GLuint loc = quadtree.getAttributeLocation("pos");
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glEnableVertexAttribArray(loc);
	glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * NUM_FLOATS_PER_VERTEX, (GLvoid*)0);

	loc = quadtree.getAttributeLocation("uv");
	glEnableVertexAttribArray(loc);
	glVertexAttribPointer(loc, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * NUM_FLOATS_PER_VERTEX, (GLvoid*)(3*sizeof(GLfloat)));

	loc = quadtree.getAttributeLocation("normal");
	glEnableVertexAttribArray(loc);
	glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * NUM_FLOATS_PER_VERTEX, (GLvoid*)(5 * sizeof(GLfloat)));

	loc = quadtree.getAttributeLocation("delta");
	glEnableVertexAttribArray(loc);
	glVertexAttribPointer(loc, 1, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * NUM_FLOATS_PER_VERTEX, (GLvoid*)(8 * sizeof(GLfloat)));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, QTibo);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glDrawElements(GL_TRIANGLES, SI * SI * 6, GL_UNSIGNED_INT, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}


void QuadTree::connectNeighbor(QuadTree* child, byte s)
{
	QuadTree* neighbor;
	for (int i = 0; i < 4; i++)
	{
		int pn = PN[s][i];
		if (pn == X)
		{
			neighbor = this;
		}
		else
		{
			neighbor = neighbors[pn];
		}
		int ci = CI[s][i];
		ci = CN[side][neighbor->side][ci];
		child->neighbors[i] = neighbor->neighbors[ci];
	}
}

void QuadTree::generateSelf()
{
	registerRequest(&planet->reqs[0], planet->tempData[0], this);
	vbo = getFreeVBO();
	genTexture();
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
	waitOnQuadRequest(&planet->reqs[0]);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, VBO_NUM_BYTES, planet->tempData[0]);
}

void QuadTree::generateChildren()
{
	registerRequest(&planet->reqs[SW], planet->tempData[SW], children[SW]);
	registerRequest(&planet->reqs[SE], planet->tempData[SE], children[SE]);
	registerRequest(&planet->reqs[NE], planet->tempData[NE], children[NE]);
	registerRequest(&planet->reqs[NW], planet->tempData[NW], children[NW]);

	GLuint vboSW = getFreeVBO();
	GLuint vboSE = getFreeVBO();
	GLuint vboNW = getFreeVBO();
	GLuint vboNE = getFreeVBO();
	putFreeTexture(this, texture);
	children[SW]->genTexture();
	children[SE]->genTexture();
	children[NE]->genTexture();
	children[NW]->genTexture();
	
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
	waitOnQuadRequest(&planet->reqs[SW]);
	waitOnQuadRequest(&planet->reqs[SE]);
	waitOnQuadRequest(&planet->reqs[NE]);
	waitOnQuadRequest(&planet->reqs[NW]);

	children[SW]->vbo = vboSW;
	glBindBuffer(GL_ARRAY_BUFFER, vboSW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, VBO_NUM_BYTES, planet->tempData[SW]);

	children[SE]->vbo = vboSE;
	glBindBuffer(GL_ARRAY_BUFFER, vboSE);
	glBufferSubData(GL_ARRAY_BUFFER, 0, VBO_NUM_BYTES, planet->tempData[SE]);

	children[NW]->vbo = vboNW;
	glBindBuffer(GL_ARRAY_BUFFER, vboNW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, VBO_NUM_BYTES, planet->tempData[NW]);

	children[NE]->vbo = vboNE;
	glBindBuffer(GL_ARRAY_BUFFER, vboNE);
	glBufferSubData(GL_ARRAY_BUFFER, 0, VBO_NUM_BYTES, planet->tempData[NE]);
}

void QuadTree::split()
{
	double w = width / 4.0; double w2 = width / 2.0;
	double uvw = uw / 4.0; double uvw2 = uw / 2.0;
	double luw = localUW / 2.0;

	int i = FI[face][0], j = FI[face][1];
	dvec3 ncenter;
	dvec2 nuv;
	dvec2 lnuv;

	ncenter = dvec3(center.x, center.y, center.z);
	ncenter[i] -= w;
	ncenter[j] -= w;
	nuv = dvec2(uv.x - uvw, uv.y - uvw);
	lnuv = dvec2(localUV);
	children[SW] = new QuadTree(this, w2, ncenter, uvw2, nuv, SW, lnuv, luw);

	ncenter = dvec3(center.x, center.y, center.z);
	ncenter[i] += w;
	ncenter[j] -= w;
	nuv = dvec2(uv.x + uvw, uv.y - uvw);
	lnuv = dvec2(localUV.x + luw, localUV.y);
	children[SE] = new QuadTree(this, w2, ncenter, uvw2, nuv, SE, lnuv, luw);

	ncenter = dvec3(center.x, center.y, center.z);
	ncenter[i] += w;
	ncenter[j] += w;
	nuv = dvec2(uv.x + uvw, uv.y + uvw);
	lnuv = dvec2(localUV.x + luw, localUV.y + luw);
	children[NE] = new QuadTree(this, w2, ncenter, uvw2, nuv, NE, lnuv, luw);
	ncenter = dvec3(center.x, center.z, center.y);

	ncenter = dvec3(center.x, center.y, center.z);
	ncenter[i] -= w;
	ncenter[j] += w;
	nuv = dvec2(uv.x - uvw, uv.y + uvw);
	lnuv = dvec2(localUV.x, localUV.y + luw);
	children[NW] = new QuadTree(this, w2, ncenter, uvw2, nuv, NW, lnuv, luw);

	generateChildren();
}

double QuadTree::lineDistance(glm::dvec3& point)
{
	glm::dvec3 pos = planet->center + planet->radius*normalize(point - planet->center);
	return distance(pos, sphereCenter);
}

void QuadTree::calcStatus()
{

}

void QuadTree::updateState()
{
	if (children[0] != NULL)
	{
		children[0]->updateState();
		children[1]->updateState();
		children[2]->updateState();
		children[3]->updateState();
		return;
	}
	calcStatus();
}

void QuadTree::splitNeighbors()
{
	QuadTree* neighbor;
	for (int i = 0; i < 4; i++)
	{
		neighbor = neighbors[i];
		if (neighbor->children[0] == NULL)
		{
			neighbor->split();
		}
	}
}

#define WW 1.2
#define WO WW
void QuadTree::update(glm::dvec3& cam, double d)
{
	double dist = lineDistance(cam);
	if (children[0] == NULL)
	{
		if (lod < MAXLOD && breaks < MAXBREAKS && 
			((dist < WW*width && d < 2000) || (distance(cam, sphereCenter) < WW*width*WO)) )
		{
			//breaks++;
			//splitNeighbors();
			split();
		}
	}
	else
	{
		if (!((dist < WW*width && d < 2000) || (distance(cam, sphereCenter) < WW*width*WO)))
		{
			delete children[0];
			delete children[1];
			delete children[2];
			delete children[3];
			children[0] = NULL;
			children[1] = NULL;
			children[2] = NULL;
			children[3] = NULL;
			genTexture();
		}
		else
		{
			children[0]->update(cam, d);
			children[1]->update(cam, d);
			children[2]->update(cam, d);
			children[3]->update(cam, d);
		}
	}
}

QuadTree::~QuadTree()
{
	if (children[0] != NULL)
	{
		delete children[0];
		delete children[1];
		delete children[2];
		delete children[3];
		children[0] = NULL;
		children[1] = NULL;
		children[2] = NULL;
		children[3] = NULL;
	}
	putFreeTexture(this, texture);
	putFreeVBO(vbo);
}