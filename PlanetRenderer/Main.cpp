#include "PlanetRenderer.h"
#include "Program.h"
#include "Camera.h"
#include <iostream>
#include "Planet.h"
#include "KeyBoard.h"
#include "CameraController.h"
#include <chrono>
#include "TextureShaders.h"


double np_d = 1;
float np_f = (float)np_d;

double fp_d = 250000000.0;
float fp_f = (float)fp_d;

Program* basic;
Camera* cam;
SceneNode* root;
glm::dmat4 view;
Planet* planet;
//Planet* planet2;
CameraController* controller;
bool msaa = false;
int num = 0;

GLint render = GL_FILL;

using namespace std::chrono;
high_resolution_clock::time_point t1;
high_resolution_clock::time_point t2;

GLuint tex;

void idleFunc()
{
	glm::dvec3 camPos;
	root->update();
	cam->getPosition(camPos);
	double dist = planet->getSurfaceDistance(camPos);
	controller->update(dist);
	cam->getPosition(camPos);
	planet->update(camPos);
	//planet2->update(camPos);
	glutPostRedisplay();
}

void renderScene(void)
{
	t1 = high_resolution_clock::now();
	if (duration_cast<milliseconds>(t1 - t2).count() > 30)
		std::cout << duration_cast<milliseconds>(t1 - t2).count() << " : ";
	t2 = high_resolution_clock::now();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.1f, 0.6f, 0.6f, 1.0f);
	if (IS_PRESSED_REG('z')) render = GL_LINE;
	if (IS_PRESSED_REG('x')) render = GL_FILL;
	if (!IS_PRESSED_REG('1'))
	{
		glPolygonMode(GL_FRONT_AND_BACK, render);
		planet->render(cam, view);
		//planet2->render(cam, view);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	else
	{
		renderTextureToScreen(planet->tree[POS_Z_FACE]->children[0]->texture);
	}

	glutSwapBuffers();
}

void onResize(int width, int height)
{
	float viewAsp = width*1.0f / height;
	float w = 1.0f*width, h = 1.0f*height;
	view = glm::perspectiveFov(45.0f, w, h, np_f, fp_f);
	glViewport(0, 0, width, height);
	gluPerspective(45.0, w / h, np_d, fp_d);
}

void init()
{
	root = new SceneNode(NULL);
	cam = new Camera(glm::dvec3(0.0, 0.0, 6371000*2),
		glm::dvec3(0.0, 0.0, -1.0),
		glm::dvec3(0.0, 1.0, 0.0), NULL);
	root->addChild(cam);
	view = glm::perspective(45.0f, 1.3333f, np_f, fp_f);
	gluPerspective(45.0, 4.0/3.0, np_d, fp_d);
	//basic = new Program("basic.vert", "basic.frag");
	planet = new Planet(glm::dvec3(0, 0, 0), 6371000, NULL);
	//planet2 = new Planet(glm::dvec3(6371000, 0, 0), 6371000.0 / 2, NULL);
	controller = new CameraController(cam);
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Big Ball Renderer");
	glewInit();

	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	initQuadManager();
	initTexture();
	initPlanet();
	initKeyboard();
	init();

	glutReshapeFunc(onResize);
	glutDisplayFunc(renderScene);

	glutIdleFunc(idleFunc);
	glutMainLoop();

	//glDeleteProgram(program);
	return 0;
}