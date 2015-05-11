#include "KeyBoard.h"
#include "CameraController.h"

#define ROT_AMT M_PI_128

CameraController::CameraController(Camera* camera)
{
	setActiveCamera(camera);
}

Camera* CameraController::getActiveCamera()
{
	return cam;
}

void CameraController::setActiveCamera(Camera* camera)
{
	cam = camera;
}

void CameraController::update(double distance)
{
	double amt = sqrt(distance)/3.0;

	if (distance < 150)
		amt /= 10;
	else if (distance < 256)
		amt /= 8;
	else if (distance > 1000)
		amt *= 80;

	glm::dvec3 trans = glm::dvec3(0.0, 0.0, 0.0);
	glm::dvec3 rot = glm::dvec3(0.0, 0.0, 0.0);

	if (IS_PRESSED_REG(MOVE_FORWARD))
		trans.z -= amt;
	if (IS_PRESSED_REG(MOVE_BACKWARD))
		trans.z += amt;
	if (IS_PRESSED_REG(MOVE_LEFT))
		trans.x -= amt;
	if (IS_PRESSED_REG(MOVE_RIGHT))
		trans.x += amt;
	if (IS_PRESSED_REG(MOVE_UP))
		trans.y += amt;
	if (IS_PRESSED_REG(MOVE_DOWN))
		trans.y -= amt;
	
	if (IS_PRESSED_SPC(ROT_LEFT))
		rot.y += ROT_AMT;
	if (IS_PRESSED_SPC(ROT_RIGHT))
		rot.y -= ROT_AMT;
	if (IS_PRESSED_SPC(ROT_UP))
		rot.x += ROT_AMT;
	if (IS_PRESSED_SPC(ROT_DOWN))
		rot.x -= ROT_AMT;
	if (IS_PRESSED_REG(ROT_CW))
		rot.z -= ROT_AMT;
	if (IS_PRESSED_REG(ROT_CCW))
		rot.z += ROT_AMT;

	cam->translate(trans);
	cam->rotate(rot);

}