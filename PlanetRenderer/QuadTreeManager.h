#pragma once

#include "PlanetRenderer.h"
#include "QuadTree.h"
#include <mutex>
#include <condition_variable>

class QuadTree;

struct QuadRequest;

void registerRequest(struct QuadRequest* req, float* data, QuadTree* tree);

void waitOnQuadRequest(struct QuadRequest* req);

void initQuadManager();

GLuint getFreeVBO();

void putFreeVBO(GLuint vbo);

GLuint getFreeTexture();

void putFreeTexture(QuadTree* tree, GLuint texture);


