#include <GL/glew.h>
#include <OpenGL/glu.h>
#include "AGL/agl.h"
#include <GLUT/glut.h>
#include "ghost/Input.h"
#include "ghost/Engine.h"
#include "ghost/Multiplatform/MacOSX/MacOSXServiceLocator.h"

Engine* GHOST = 0;
bool fullscreen = false;

void load();
void resize(int width, int height);
void handleMouseMove(int x, int y);
void handleKey(UINT32 key, bool keyUp);
void handleSpecialKey(UINT32 key, bool keyUp);
void handleKeyDown(unsigned char key, int x, int y);
void handleKeyUp(unsigned char key, int x, int y);
void handleSpecialKeyDown(int key, int x, int y);
void handleSpecialKeyUp(int key, int x, int y);
void handleMouseButtons(int button, int state, int x, int y);
void computeFrame();
void close();

void load() {
	MacOSXServiceLocator* sl = new MacOSXServiceLocator();
	GHOST = new Engine(sl);
	if (!*GHOST) {
		delete GHOST;
		GHOST = 0;
		LOGE("Engine terminated due to error.");
	}
    GHOST->resume();
}

void resize(int width, int height) {
	GHOST->resizeScreen(width, height);
}

int g_lastX = 0, g_lastY = 0;
void handleMouseMove(int x, int y) {
    GHOST->getServiceLocator()->getInput()->provideMousePosition(x, GHOST->getServiceLocator()->getScreenHeight() - y);
	GHOST->getServiceLocator()->getInput()->provideMouseDelta(x - g_lastX, y - g_lastY);
	g_lastX = x;
	g_lastY = y;
}

void handleKey(UINT32 key, bool keyUp) {
	switch (key) {
    case 0:
        key = Input::RETURN;
        break;
	case 8:
		key = Input::BACK;
		break;
	case 32:
		key = Input::SPACE;
		break;
	case 27:
		key = Input::ESC;
		break;
	case 97:
	case 98:
	case 99:
	case 100:
	case 101:
	case 102:
	case 103:
	case 104:
	case 105:
	case 106:
	case 107:
	case 108:
	case 109:
	case 110:
	case 111:
	case 112:
	case 113:
	case 114:
	case 115:
	case 116:
	case 117:
	case 118:
	case 119:
	case 120:
	case 121:
	case 122:
		key -= 96; // Handle English letters.
		break;
	default:
		key = Input::NONE;
		break;
	}
	int modifierKeys = glutGetModifiers();
	if (modifierKeys == GLUT_ACTIVE_ALT) {
		GHOST->getServiceLocator()->getInput()->provideButton(Input::ALT, Input::PRESSED);
        if (key == 0 && keyUp) {
            if (!fullscreen) {
                glutFullScreen();
                fullscreen = true;
            }
            else {
                glutReshapeWindow(640, 480);
                glutInitWindowPosition(100, 100);
                fullscreen = false;
            }
        }
	}
	GHOST->getServiceLocator()->getInput()->provideButton(key, keyUp ? Input::RELEASED : Input::PRESSED);
}

void handleSpecialKey(UINT32 key, bool keyUp) {
	switch (key) {
	case GLUT_KEY_F1:
	case GLUT_KEY_F2:
	case GLUT_KEY_F3:
	case GLUT_KEY_F4:
	case GLUT_KEY_F5:
	case GLUT_KEY_F6:
	case GLUT_KEY_F7:
	case GLUT_KEY_F8:
	case GLUT_KEY_F9:
	case GLUT_KEY_F10:
	case GLUT_KEY_F11:
	case GLUT_KEY_F12:
		key = Input::F1 + key - 1; // Handle F* keys.
		break;
	case GLUT_KEY_UP:
		key = Input::UP;
		break;
	case GLUT_KEY_DOWN:
		key = Input::DOWN;
		break;
	case GLUT_KEY_LEFT:
		key = Input::LEFT;
		break;
	case GLUT_KEY_RIGHT:
		key = Input::RIGHT;
		break;
	case GLUT_KEY_PAGE_UP:
		key = Input::PAGE_UP;
		break;
	case GLUT_KEY_PAGE_DOWN:
		key = Input::PAGE_DOWN;
		break;
	case GLUT_KEY_HOME:
		key = Input::HOME;
		break;
	case GLUT_KEY_END:
		key = Input::END;
		break;
	default:
		key = Input::NONE;
		break;
	}
	int modifierKeys = glutGetModifiers();
	if (modifierKeys == GLUT_ACTIVE_ALT) {
		GHOST->getServiceLocator()->getInput()->provideButton(Input::ALT, Input::PRESSED);
	}
	if (modifierKeys == GLUT_ACTIVE_CTRL) {
		GHOST->getServiceLocator()->getInput()->provideButton(Input::CTRL, Input::PRESSED);
	}
	if (modifierKeys == GLUT_ACTIVE_SHIFT) {
		GHOST->getServiceLocator()->getInput()->provideButton(Input::SHIFT, Input::PRESSED);
	}
    GHOST->getServiceLocator()->getInput()->provideButton(key, keyUp == true ? Input::RELEASED : Input::PRESSED);
}

void handleKeyDown(unsigned char key, int x, int y) {
	handleKey(static_cast<UINT32>(key), false);
}

void handleKeyUp(unsigned char key, int x, int y) {
	handleKey(static_cast<UINT32>(key), true);
}

void handleSpecialKeyDown(int key, int x, int y) {
	handleSpecialKey(static_cast<UINT32>(key), false);
}

void handleSpecialKeyUp(int key, int x, int y) {
	handleSpecialKey(static_cast<UINT32>(key), true);
}

void handleMouseButtons(int button, int state, int x, int y) {
	bool clicked = state == GLUT_DOWN;
	switch (button) {
	case GLUT_LEFT_BUTTON:
		GHOST->getServiceLocator()->getInput()->provideButton(Input::MOUSE_L,
			clicked ? Input::PRESSED : Input::RELEASED);
		break;
	case GLUT_RIGHT_BUTTON:
		GHOST->getServiceLocator()->getInput()->provideButton(Input::MOUSE_R,
			clicked ? Input::PRESSED : Input::RELEASED);
		break;
	case GLUT_MIDDLE_BUTTON:
        GHOST->getServiceLocator()->getInput()->provideButton(Input::MOUSE_M,
			clicked ? Input::PRESSED : Input::RELEASED);
		break;
    default:
        LOGE("Unknown mouse button recieved. Button: %d.", button);
	}
}

void computeFrame() {
	GHOST->computeFrame();
	if (GHOST->getServiceLocator()->isFinished()) {
		exit(0);// glutLeaveMainLoop();
	}
	//glFinish();
	//glutPostRedisplay();
    //glFlush();
    glutSwapBuffers();
}

void close() {
    GHOST->pause();
	delete GHOST;
}

int main(int argc, char** argv) {
    PROFILE("Started loading SMart engine.");
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(800, 600);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("SMart engine");
	glutDisplayFunc(computeFrame);
	glutIdleFunc(computeFrame);
	glutReshapeFunc(resize);
	glutMouseFunc(handleMouseButtons);
	glutMotionFunc(handleMouseMove);
	glutPassiveMotionFunc(handleMouseMove);
	glutKeyboardFunc(handleKeyDown);
	glutKeyboardUpFunc(handleKeyUp);
	glutSpecialFunc(handleSpecialKeyDown);
	glutSpecialUpFunc(handleSpecialKeyUp);
	atexit(close);
    PROFILE("Main window created and ready for use.");
	GLenum err = glewInit();
	if (err != GLEW_OK) {
		LOGE("GLEW not initialized. Error: %s.", glewGetErrorString(err));
		return false;
	}
	if (!GLEW_VERSION_2_1) {
		LOGE("GLEW doesn't support OpenGL 2.1");
		return false;
	}
	load();
    PROFILE("Entering main loop.");
	glutMainLoop();
	return 1;
}
