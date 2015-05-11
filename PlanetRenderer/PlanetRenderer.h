#pragma once

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdio.h>

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/vec2.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "MathConstants.h"

typedef char byte;
typedef unsigned char ubyte;

typedef unsigned int uint;

#define MAX_PLANETS 16

//Quad Tree Manager Constants
#define NUM_VBOS 4096*2*2

//Quad Tree Manager Constants
#define NUM_TEXTURES 1024*4

#define NUM_FLOATS_PER_VERTEX 9
#define NUM_BYTES_PER_VERTEX sizeof(GLfloat)*NUM_FLOATS_PER_VERTEX
#define NUM_THREADS 4

//Quad Tree Faces

#define NODE_SIZE 65
#define NODE_SIZE_1 (NODE_SIZE - 1)
#define TEXTURE_SIZE 64
#define VBO_NUM_FLOATS NUM_FLOATS_PER_VERTEX * NODE_SIZE * NODE_SIZE
#define VBO_NUM_BYTES VBO_NUM_FLOATS*sizeof(GLfloat)

#define MAXLOD 21
#define MAXBREAKS 4

enum FACE {
	NEG_X_FACE, POS_X_FACE,
	NEG_Z_FACE, POS_Z_FACE,
	NEG_Y_FACE, POS_Y_FACE
};

#define NX NEG_X_FACE
#define PX POS_X_FACE
#define NZ NEG_Z_FACE
#define PZ POS_Z_FACE
#define NY NEG_Y_FACE
#define PY POS_Y_FACE

enum Neighbor { N, S, E, W, X };
enum Children { NW, NE, SW, SE };
enum Change { DEC, INC };

#define XX -1

static const byte FSI[] =
{
	-1, 1
};

static const byte PI[4][4][4] =
{
	// {N, S, E, W}
	{ { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } },
	{ { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } },

	//SW     NW              NE              SW              SE                    
	{ { N, X, X, W }, { N, X, E, X }, { X, S, X, W }, { X, S, E, X } },
	{ { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }
};

static const byte PN[4][4] =
{
	//N, S, E, W
	
	//NW
	{ N, X, X, W },

	//NE
	{ N, X, E, X },
               
	//SW
	{ X, S, X, W },

	//SE
	{ X, S, E, X }
};

static const byte CI[4][4] =
{
	//N, S, E, W

	//NW
	{ SW, SW, NE, NE },

	//NE
	{ SE, SE, NW, NW },

	//SW
	{ NW, NW, SE, SE },

	//SE
	{ NE, NE, SW, SW }
};

static const byte FI[][3] =
{
	{ 1, 2, 0 },
	{ 1, 2, 0 },
	{ 0, 1, 2 },
	{ 0, 1, 2 },
	{ 0, 2, 1 },
	{ 0, 2, 1 }
};

static const byte CN[6][6][4] =
{
	//NW, NE, SW, SE

	//       Neg X      ,        Pos X      ,       Neg Z      ,        Pos Z       ,        Neg Y      ,          Pos Y

	//Neg X Face
	{ { NW, NE, SW, SE }, { XX, XX, XX, XX }, { SW, NW, XX, XX }, { XX, XX, SW, NW }, { XX, NW, XX, SW }, { 0, 0, 0, 0 } },

	//Pos X Face
	{ { XX, XX, XX, XX }, { NW, NE, SW, SE }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } },

	//Neg Z Face
	{ { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { NW, NE, SW, SE }, { XX, XX, XX, XX }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } },

	//Pos Z Face
	{ { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { XX, XX, XX, XX }, { NW, NE, SW, SE }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } },

	//Neg Y Face
	{ { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { NW, NE, SW, SE }, { XX, XX, XX, XX } },

	//Pos Y Face
	{ { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { XX, XX, XX, XX }, { NW, NE, SW, SE } }
};


#define printOpenGLError() printOglError(__FILE__, __LINE__)

static inline int printOglError(char *file, int line)
{

	GLenum glErr;
	int    retCode = 0;

	glErr = glGetError();
	if (glErr != GL_NO_ERROR)
	{
		printf("glError in file %s @ line %d: %s\n",
			file, line, gluErrorString(glErr));
		retCode = 1;
	}
	return retCode;
}