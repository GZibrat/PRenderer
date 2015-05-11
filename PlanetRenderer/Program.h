#pragma once

#include "PlanetRenderer.h"

class Program
{
	public:
		Program(char* vertexShaderFilename,
			char* geometryShaderFilename,
			char* fragmentShaderFilename);

		Program(char* vertexShaderFilename,
			char* fragmentShaderFilename);

		Program(char* computeShaderFileName);

		Program::Program();

		void useProgram();

		GLuint getAttributeLocation(char* name);
		GLuint getUniformLocation(char* name);
		GLuint getProgram();

	private:
		GLuint program;
};