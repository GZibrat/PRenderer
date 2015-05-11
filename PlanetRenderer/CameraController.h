#pragma once

#include "Camera.h"
#include "PlanetRenderer.h"
#include <mutex>
#include <vector>

#define MOVE_FORWARD 'w'
#define MOVE_BACKWARD 's'
#define MOVE_LEFT 'a'
#define MOVE_RIGHT 'd'
#define MOVE_UP ' '
#define MOVE_DOWN 'v'

#define ROT_UP GLUT_KEY_UP
#define ROT_DOWN GLUT_KEY_DOWN
#define ROT_LEFT GLUT_KEY_LEFT
#define ROT_RIGHT GLUT_KEY_RIGHT
#define ROT_CW 'e'
#define ROT_CCW 'q'


class CameraController
{
	public:
		CameraController(Camera* camera);

		void setActiveCamera(Camera* camera);
		Camera* getActiveCamera();

		//distance is the distance to the the surface of the planet
		//so we can slow speed when closer to the planet
		void update(double distance);

	private:
		Camera* cam;
};
