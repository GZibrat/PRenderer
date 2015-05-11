#include "TextureShaders.h"

GLuint fbo;
GLuint vbo;
GLuint ibo;

Program text;

using namespace glm;

static const GLubyte quadIdx[] = {
	0, 1, 2, 0, 2, 3
};

static const GLfloat quadData[] = {
	-1.0f, -1.0f,
	1.0f, -1.0f,
	1.0f, 1.0f,
	-1.0f, 1.0f,
	0.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f
};

int perm[256] = { 151, 160, 137, 91, 90, 15,
131, 13, 201, 95, 96, 53, 194, 233, 7, 225, 140, 36, 103, 30, 69, 142, 8, 99, 37, 240, 21, 10, 23,
190, 6, 148, 247, 120, 234, 75, 0, 26, 197, 62, 94, 252, 219, 203, 117, 35, 11, 32, 57, 177, 33,
88, 237, 149, 56, 87, 174, 20, 125, 136, 171, 168, 68, 175, 74, 165, 71, 134, 139, 48, 27, 166,
77, 146, 158, 231, 83, 111, 229, 122, 60, 211, 133, 230, 220, 105, 92, 41, 55, 46, 245, 40, 244,
102, 143, 54, 65, 25, 63, 161, 1, 216, 80, 73, 209, 76, 132, 187, 208, 89, 18, 169, 200, 196,
135, 130, 116, 188, 159, 86, 164, 100, 109, 198, 173, 186, 3, 64, 52, 217, 226, 250, 124, 123,
5, 202, 38, 147, 118, 126, 255, 82, 85, 212, 207, 206, 59, 227, 47, 16, 58, 17, 182, 189, 28, 42,
223, 183, 170, 213, 119, 248, 152, 2, 44, 154, 163, 70, 221, 153, 101, 155, 167, 43, 172, 9,
129, 22, 39, 253, 19, 98, 108, 110, 79, 113, 224, 232, 178, 185, 112, 104, 218, 246, 97, 228,
251, 34, 242, 193, 238, 210, 144, 12, 191, 179, 162, 241, 81, 51, 145, 235, 249, 14, 239, 107,
49, 192, 214, 31, 181, 199, 106, 157, 184, 84, 204, 176, 115, 121, 50, 45, 127, 4, 150, 254,
138, 236, 205, 93, 222, 114, 67, 29, 24, 72, 243, 141, 128, 195, 78, 66, 215, 61, 156, 180 };

int grad3[16][3] = { { 0, 1, 1 }, { 0, 1, -1 }, { 0, -1, 1 }, { 0, -1, -1 },
{ 1, 0, 1 }, { 1, 0, -1 }, { -1, 0, 1 }, { -1, 0, -1 },
{ 1, 1, 0 }, { 1, -1, 0 }, { -1, 1, 0 }, { -1, -1, 0 }, // 12 cube edges
{ 1, 0, -1 }, { -1, 0, -1 }, { 0, -1, 1 }, { 0, 1, 1 } }; // 4 more to make 16

GLuint permTextureID;

void initPermTexture(GLuint *texID)
{
	char *pixels;
	int i, j;

	glGenTextures(1, texID); // Generate a unique texture ID
	glBindTexture(GL_TEXTURE_2D, *texID); // Bind the texture to texture unit 0

	pixels = new char[256 * 256 * 4];
	for (i = 0; i<256; i++)
		for (j = 0; j<256; j++) {
			int offset = (i * 256 + j) * 4;
			char value = perm[(j + perm[i]) & 0xFF];
			pixels[offset] = grad3[value & 0x0F][0] * 64 + 64;   // Gradient x
			pixels[offset + 1] = grad3[value & 0x0F][1] * 64 + 64; // Gradient y
			pixels[offset + 2] = grad3[value & 0x0F][2] * 64 + 64; // Gradient z
			pixels[offset + 3] = value;                     // Permuted index
		}


	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	delete[] pixels;
}

GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };

void initTexture()
{
	initPermTexture(&permTextureID);
	text = Program("quad.vert", "screenTex.frag");
	glGenFramebuffers(1, &fbo);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 20, quadData, GL_DYNAMIC_DRAW);

	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(GLubyte), quadIdx, GL_STATIC_DRAW);
}

void perlinToTexture(Program& program, int w, int h,
	dvec3& v00, dvec3& v10, dvec3& v01, dvec3& v11, GLuint texture)
{
	glEnable(GL_TEXTURE_2D);
	program.useProgram();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, permTextureID);

	glActiveTexture(GL_TEXTURE1);
	glBindImageTexture(1, texture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);

	GLuint loc = program.getUniformLocation("top_right");
	glUniform3d(loc, v11.x, v11.y, v11.z);

	loc = program.getUniformLocation("lower_left");
	glUniform3d(loc, v00.x, v00.y, v00.z);

	glDispatchCompute(8, 8, 1);
	//glMemoryBarrier(GL_ALL_BARRIER_BITS);
}

void renderTextureToScreen(GLuint texture)
{

	int w = glutGet(GLUT_WINDOW_WIDTH);
	int h = glutGet(GLUT_WINDOW_HEIGHT);
	glEnable(GL_TEXTURE_2D);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 8, sizeof(GLfloat) * 12, quadData + 8);

	text.useProgram();
	GLuint loc = text.getAttributeLocation("position");
	glEnableVertexAttribArray(loc);
	glVertexAttribPointer(loc, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 2, (GLvoid*)0);

	loc = text.getAttributeLocation("uv");
	glEnableVertexAttribArray(loc);
	glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, (GLvoid*)(sizeof(GLfloat) * 8));

	glBindTexture(GL_TEXTURE_2D, texture);
	glActiveTexture(GL_TEXTURE0);

	glViewport(0, 0, w, h);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, 0);

}