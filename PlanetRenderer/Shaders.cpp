#include <vector>
#include <iostream>
#include <fstream>
#include "Program.h"


std::string readShader(char *filename)
{
	std::string shaderCode;
	std::ifstream file(filename, std::ios::in);

	if (!file.good())
	{
		std::cout << "Can't read file " << filename << std::endl;
		std::terminate();
	}

	file.seekg(0, std::ios::end);
	shaderCode.resize((unsigned int)file.tellg());
	file.seekg(0, std::ios::beg);
	file.read(&shaderCode[0], shaderCode.size());
	file.close();
	return shaderCode;
}

GLuint createShader(GLenum shaderType, std::string
	source, char* shaderName)
{

	int compileResult = 0;

	GLuint shader = glCreateShader(shaderType);
	const char *shaderCodePtr = source.c_str();
	const int shaderCodeSize = source.size();

	glShaderSource(shader, 1, &shaderCodePtr, &shaderCodeSize);
	glCompileShader(shader);
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compileResult);

	//check for errors
	if (compileResult == GL_FALSE)
	{

		int infoLogLength = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
		std::vector<char> shaderLog(infoLogLength);
		glGetShaderInfoLog(shader, infoLogLength, NULL, &shaderLog[0]);
		std::cout << "ERROR compiling shader: " << shaderName << std::endl << &shaderLog[0] << std::endl;
		return 0;
	}
	return shader;
}

GLuint createProgram(char* computeShaderFileName)
{

	std::string computeShaderCode = readShader(computeShaderFileName);

	GLuint computeShader = createShader(GL_COMPUTE_SHADER, computeShaderCode, "compute shader");

	int linkResult = 0;
	//create the program handle, attatch the shaders and link it
	GLuint program = glCreateProgram();
	glAttachShader(program, computeShader);

	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &linkResult);
	//check for link errors
	if (linkResult == GL_FALSE)
	{
		int infoLogLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);
		std::vector<char> programLog(infoLogLength);
		glGetProgramInfoLog(program, infoLogLength, NULL, &programLog[0]);
		std::cout << "Shader Loader : LINK ERROR" << std::endl << &programLog[0] << std::endl;
		return 0;
	}
	return program;
}

GLuint createProgram(char* vertexShaderFilename,
	char* fragmentShaderFilename)
{

	//read the shader files and save the code
	std::string vertexShaderCode = readShader(vertexShaderFilename);
	std::string fragmentShaderCode = readShader(fragmentShaderFilename);

	GLuint vertexShader = createShader(GL_VERTEX_SHADER, vertexShaderCode, "vertex shader");
	GLuint fragmentShader = createShader(GL_FRAGMENT_SHADER, fragmentShaderCode, "fragment shader");

	int linkResult = 0;
	//create the program handle, attatch the shaders and link it
	GLuint program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);

	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &linkResult);
	//check for link errors
	if (linkResult == GL_FALSE)
	{
		int infoLogLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);
		std::vector<char> programLog(infoLogLength);
		glGetProgramInfoLog(program, infoLogLength, NULL, &programLog[0]);
		std::cout << "Shader Loader : LINK ERROR" << std::endl << &programLog[0] << std::endl;
		return 0;
	}
	return program;
}

GLuint createProgram(char* vertexShaderFilename,
	char* geometryShaderFileName,
	char* fragmentShaderFilename)
{
	//read the shader files and save the code
	std::string vertexShaderCode = readShader(vertexShaderFilename);
	std::string geometryShaderCode = readShader(geometryShaderFileName);
	std::string fragmentShaderCode = readShader(fragmentShaderFilename);

	GLuint vertexShader = createShader(GL_VERTEX_SHADER, vertexShaderCode, "vertex shader");
	GLuint geometryShader = createShader(GL_GEOMETRY_SHADER, geometryShaderCode, "geometry shader");
	GLuint fragmentShader = createShader(GL_FRAGMENT_SHADER, fragmentShaderCode, "fragment shader");

	int linkResult = 0;
	//create the program handle, attatch the shaders and link it
	GLuint program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, geometryShader);
	glAttachShader(program, fragmentShader);

	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &linkResult);
	//check for link errors
	if (linkResult == GL_FALSE)
	{

		int infoLogLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);
		std::vector<char> programLog(infoLogLength);
		glGetProgramInfoLog(program, infoLogLength, NULL, &programLog[0]);
		std::cout << "Shader Loader : LINK ERROR" << std::endl << &programLog[0] << std::endl;
		return 0;
	}
	return program;
}

Program::Program()
{

}

Program::Program(char* vertexShaderFilename,
	char* geometryShaderFilename,
	char* fragmentShaderFilename)
{
	createProgram(vertexShaderFilename,
		geometryShaderFilename,
		fragmentShaderFilename);
}


Program::Program(char* vertexShaderFilename,
	char* fragmentShaderFilename)
{
	program = createProgram(vertexShaderFilename, fragmentShaderFilename);
}

Program::Program(char* computeShaderFileName)
{
	program = createProgram(computeShaderFileName);
}

void Program::useProgram()
{
	glUseProgram(program);
}

GLuint Program::getProgram()
{
	return program;
}

GLuint Program::getAttributeLocation(char* name)
{
	return glGetAttribLocation(program, name);
}

GLuint Program::getUniformLocation(char* name)
{
	return glGetUniformLocation(program, name);
}