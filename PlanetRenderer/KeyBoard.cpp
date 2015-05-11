
#include "KeyBoard.h"
#include <stdio.h>

byte keys[512];

void pressNormalKeys(unsigned char key, int x, int y)
{
	IS_PRESSED_REG(key) = 1;
}

void releaseNormalKeys(unsigned char key, int x, int y)
{
	IS_PRESSED_REG(key) = 0;
}

void pressSpecialKeys(int key, int x, int y)
{
	IS_PRESSED_SPC(key) = 1;
}

void releaseSpecialKeys(int key, int x, int y)
{
	IS_PRESSED_SPC(key) = 0;
}

void initKeyboard()
{
	memset(keys, 0, sizeof(keys));

	glutKeyboardFunc(pressNormalKeys);
	glutSpecialFunc(pressSpecialKeys);
	glutKeyboardUpFunc(releaseNormalKeys);
	glutSpecialUpFunc(releaseSpecialKeys);
}
