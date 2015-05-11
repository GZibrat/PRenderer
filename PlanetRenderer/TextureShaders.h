#pragma once

#include "PlanetRenderer.h"
#include "Program.h"

void initTexture();

void perlinToTexture(Program& program, int w, int h,
	glm::dvec3& v00, glm::dvec3& v10, glm::dvec3& v01,
	glm::dvec3& v11, GLuint texture);

void renderTextureToScreen(GLuint texture);