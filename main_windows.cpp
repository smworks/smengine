/*
 * main.cpp
 *
 *  Created on: 2012.07.05
 *      Author: MS
 */

#ifdef _WIN32

#include "ghost/Multiplatform/ServiceLocator.h"
#include "ghost/Engine.h"
#include "ghost/Multiplatform/Win32/WindowsServiceLocator.h"
#include "ghost/Input.h"

#include <windowsx.h>
#include <io.h>
#include <fcntl.h>
#include <stdio.h>

#define WIDTH 800
#define HEIGHT 600

HWND    g_hwnd;
HDC     hDC;
HGLRC   hRC;
Engine* GHOST = 0;
bool g_input = true;
bool g_fullscreen = false;
bool stopped = true;

void enableInput() {
	stopped = false;
}

void disableInput() {
	stopped = true;
}

void enableFullScreen() {
	int maxWidth = GetSystemMetrics(SM_CXSCREEN);
	int maxHeight = GetSystemMetrics(SM_CYSCREEN);
	DEVMODE dmScreenSettings;
	memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
	dmScreenSettings.dmSize = sizeof(dmScreenSettings);
	dmScreenSettings.dmPelsWidth = maxWidth;
	dmScreenSettings.dmPelsHeight = maxHeight;
	dmScreenSettings.dmBitsPerPel = 32;
	dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
	ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);
	LONG dwExStyle = GetWindowLong(g_hwnd, GWL_EXSTYLE);
	LONG dwStyle = GetWindowLong(g_hwnd, GWL_STYLE);
	dwExStyle &= ~(WS_EX_DLGMODALFRAME | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE);
	dwStyle &= ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZE | WS_MAXIMIZE | WS_SYSMENU);
	SetWindowLong(g_hwnd, GWL_EXSTYLE, dwExStyle);
	SetWindowLong(g_hwnd, GWL_STYLE, dwStyle);
	SetWindowPos(g_hwnd, 0, 0, 0, maxWidth, maxHeight, SWP_NOZORDER | SWP_FRAMECHANGED);
}

void disableFullScreen() {
	ChangeDisplaySettings(0, 0);
	ShowCursor(TRUE);
	LONG dwExStyle = GetWindowLong(g_hwnd, GWL_EXSTYLE);
	LONG dwStyle = GetWindowLong(g_hwnd, GWL_STYLE);
	dwExStyle |= WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
	dwStyle |= WS_OVERLAPPEDWINDOW;
	SetWindowLong(g_hwnd, GWL_EXSTYLE, dwExStyle);
	SetWindowLong(g_hwnd, GWL_STYLE, dwStyle);
	SetWindowPos(g_hwnd, HWND_TOP, 100, 100, WIDTH, HEIGHT,	SWP_FRAMECHANGED);
}

// Set up pixel format for graphics initialization
void SetupPixelFormat() {
    PIXELFORMATDESCRIPTOR pfd, *ppfd;
    int pixelformat;

    ppfd = &pfd;

    ppfd->nSize = sizeof(PIXELFORMATDESCRIPTOR);
    ppfd->nVersion = 1;
    ppfd->dwFlags =
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    ppfd->dwLayerMask = PFD_MAIN_PLANE;
	ppfd->iPixelType = PFD_TYPE_RGBA;// COLORINDEXs;
    ppfd->cColorBits = 16;
    ppfd->cDepthBits = 24;
    ppfd->cAccumBits = 0;
    ppfd->cStencilBits = 0;
    pixelformat = ChoosePixelFormat(hDC, ppfd);
    SetPixelFormat(hDC, pixelformat, ppfd);
}

void setupOpenGL(UINT32 width, UINT32 height) {
    hDC = GetDC(g_hwnd);
    SetupPixelFormat();
    hRC = wglCreateContext(hDC);
    wglMakeCurrent(hDC, hRC);
	GLenum err = glewInit();
	if (err != GLEW_OK) {
		THROWEX("GLEW not initialized");
	}
	if (!GLEW_VERSION_2_1) {
		THROWEX("This computer or terminal doesn't support OpenGL 2.1");
	}
}

// Resize graphics to fit window
void ResizeGraphics()
{
    // Get new window size
    RECT rect;
    GetClientRect(g_hwnd, &rect);
    int width = rect.right;
    int height = rect.bottom;
	if (GHOST) {
		GHOST->resizeScreen(width, height);
	}
}

void onInput(LPARAM lParam) {
	if (!GHOST || !g_input) {
		return;
	}
	UINT iBufferSize;
	GetRawInputData(
		(HRAWINPUT)lParam,
		RID_INPUT,
		NULL,
		&iBufferSize,
		sizeof(RAWINPUTHEADER));
	BYTE* byBuffer = new BYTE[iBufferSize];
	GetRawInputData(
		(HRAWINPUT)lParam, RID_INPUT,
		(LPVOID)byBuffer,
		&iBufferSize,
		sizeof(RAWINPUTHEADER));
	RAWINPUT* input = (RAWINPUT*) byBuffer;
	if (input->header.dwType == RIM_TYPEKEYBOARD) {
		USHORT key = input->data.keyboard.VKey;
		bool keyUp = input->data.keyboard.Flags & RI_KEY_BREAK;
		switch (key)
		{
		case VK_UP: key = Input::UP; break;
		case VK_DOWN: key = Input::DOWN; break;
		case VK_LEFT: key = Input::LEFT; break;
		case VK_RIGHT: key = Input::RIGHT; break;
		case VK_RETURN: key = Input::RETURN;
			if (keyUp && GHOST->getServiceLocator()->getInput()->
				keyPressed(Input::ALT))
			{
				if (g_fullscreen) {
					disableFullScreen();
					g_fullscreen = false;
				}
				else {
					enableFullScreen();
					g_fullscreen = true;
				}
			}
			break;
		case VK_BACK: key = Input::BACK; break;
		case VK_SPACE: key = Input::SPACE; break;
		case VK_CONTROL: key = Input::CTRL; break;
		case VK_MENU: key = Input::ALT; break;
		case VK_ESCAPE: key = Input::ESC; break;
		case VK_SHIFT: key = Input::SHIFT; break;
		case VK_F1: key = Input::F1; break;
		case VK_F2: key = Input::F2; break;
		case VK_F3: key = Input::F3; break;
		case VK_F4: key = Input::F4; break;
		case VK_F5: key = Input::F5; break;
		case VK_F6: key = Input::F6; break;
		case VK_F7: key = Input::F7; break;
		case VK_F8: key = Input::F8; break;
		case VK_F9: key = Input::F9; break;
		case VK_F10: key = Input::F10; break;
		case VK_F11: key = Input::F11; break;
		case VK_F12: key = Input::F12; break;
		case VK_OEM_1: key = Input::SEMICOLON; break;
		case VK_OEM_PLUS: key = Input::PLUS; break;
		case VK_OEM_COMMA: key = Input::COMMA; break;
		case VK_OEM_MINUS: key = Input::MINUS; break;
		case VK_OEM_PERIOD: key = Input::PERIOD; break;
		case VK_OEM_2: key = Input::SLASH; break;
		case VK_OEM_3: key = Input::TILDE; break;

		default:
			//LOGI("KEY: %d", key);
			TCHAR ch = (TCHAR) key;
			key = ch - 54;
		}
		GHOST->getServiceLocator()->getInput()->provideButton(
			key, keyUp == true ? Input::RELEASED : Input::PRESSED);
	}
	else if (input->header.dwType == RIM_TYPEMOUSE) {
		ULONG& buttons = input->data.mouse.ulButtons;
		if (buttons & RI_MOUSE_LEFT_BUTTON_DOWN) {
			GHOST->getServiceLocator()->getInput()->provideButton(
				Input::MOUSE_L, Input::PRESSED);
		} else if (buttons & RI_MOUSE_LEFT_BUTTON_UP) {
			GHOST->getServiceLocator()->getInput()->provideButton(
				Input::MOUSE_L, Input::RELEASED);
		}
		if (buttons & RI_MOUSE_RIGHT_BUTTON_DOWN) {
			GHOST->getServiceLocator()->getInput()->provideButton(
				Input::MOUSE_R, Input::PRESSED);
		} else if (buttons & RI_MOUSE_RIGHT_BUTTON_UP) {
			GHOST->getServiceLocator()->getInput()->provideButton(
				Input::MOUSE_R, Input::RELEASED);
		}
		if (buttons & RI_MOUSE_MIDDLE_BUTTON_DOWN) {
			GHOST->getServiceLocator()->getInput()->provideButton(
				Input::MOUSE_M, Input::PRESSED);
		} else if (buttons & RI_MOUSE_MIDDLE_BUTTON_UP) {
			GHOST->getServiceLocator()->getInput()->provideButton(
				Input::MOUSE_M, Input::RELEASED);
		}
		if (buttons & RI_MOUSE_BUTTON_4_DOWN) {
			GHOST->getServiceLocator()->getInput()->provideButton(
				Input::MOUSE_4, Input::PRESSED);
		} else if (buttons & RI_MOUSE_BUTTON_4_UP) {
			GHOST->getServiceLocator()->getInput()->provideButton(
				Input::MOUSE_4, Input::RELEASED);
		}
		if (buttons & RI_MOUSE_BUTTON_5_DOWN) {
			GHOST->getServiceLocator()->getInput()->provideButton(
				Input::MOUSE_5, Input::PRESSED);
		} else if (buttons & RI_MOUSE_BUTTON_5_UP) {
			GHOST->getServiceLocator()->getInput()->provideButton(
				Input::MOUSE_5, Input::RELEASED);
		}
		long mX = input->data.mouse.lLastX;
		long mY = input->data.mouse.lLastY;
		if (mX != 0 || mY != 0) {
			if ((input->data.mouse.usFlags & MOUSE_MOVE_ABSOLUTE) > 0) {
				//LOGI("LALALA");
				// This part of code seems to be executed
				// in virtual machine with windows XP,
				// laptops with touchpad and using remote desktop
				// connection.
				static long lastX = 0;
				static long lastY = 0;
				long tX = mX;
				long tY = mY;
				mX = mX - lastX;
				mY = mY - lastY;
				static int maxSize = 2;
				if (mX > maxSize) {
					mX = maxSize;
				} else if (mX < -maxSize) {
					mX = -maxSize;
				}
				if (mY > maxSize) {
					mY = maxSize;
				} else if (mY < -maxSize) {
					mY = -maxSize;
				}
				lastX = tX;
				lastY = tY;
			}
			ServiceLocator* services = GHOST->getServiceLocator();
			int height = services->getScreenHeight();
			services->getInput()->provideMouseDelta(mX, mY);
		}
	}
	delete[] byBuffer;
}

LONG WINAPI MainWndProc(HWND g_hwnd, UINT uMsg, WPARAM  wParam, LPARAM  lParam) {
    switch (uMsg) {
	case WM_INPUT:
		onInput(lParam);
		break;
	case WM_MOUSEMOVE:
		{
			if (!g_input) {
				return 0;
			}
			int xPos = GET_X_LPARAM(lParam);
            int yPos = GET_Y_LPARAM(lParam);
			ServiceLocator* services = GHOST->getServiceLocator();
			int height = services->getScreenHeight();
			services->getInput()->provideMousePosition(xPos, height - yPos);
		}
		break;
	case WM_ACTIVATE:
		if (stopped) {
			break;
		}
		if (wParam > 0) {
			LOGD("Input active.");
			g_input = true;
		}
		else {
			LOGD("Input inactive.");
			g_input = false;
			ServiceLocator* services = GHOST->getServiceLocator();
			services->getInput()->reset();
		}
		g_input = wParam > 0 ? true : false;
		break;
    case WM_SIZE:
        ResizeGraphics();
        break;
	case WM_QUIT:
		break;
    case WM_CLOSE:
		if (*GHOST) GHOST->getServiceLocator()->exit();
        //DestroyWindow(g_hwnd);
        break;
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC dc;
			string message = "OpenGL 2.1 not supported!";
			dc = BeginPaint(g_hwnd, &ps);
			if (GHOST && !GHOST->getServiceLocator()->isGuiAvailable()) {
				TextOutA(dc, 10, 10, (LPCSTR) message.c_str(), message.length());
			}
			EndPaint(g_hwnd, &ps);
		}
		break;
    case WM_DESTROY:
		if (*GHOST) GHOST->getServiceLocator()->exit();
        //PostQuitMessage(0);
        break;
    // Default event handler
    default: 
        return DefWindowProc (g_hwnd, uMsg, wParam, lParam); 
        break; 
    } 
 
    return 0; 
}

bool initializeRawInput(HWND& g_hwnd) {
	RAWINPUTDEVICE dev[2];

	// Mouse.
	dev[0].dwFlags = 0; //RIDEV_NOLEGACY;
	dev[0].hwndTarget = NULL;
	dev[0].usUsage = 2;
	dev[0].usUsagePage = 1;

	// Keyboard.
	dev[1].dwFlags = RIDEV_NOLEGACY;
	dev[1].hwndTarget = NULL;
	dev[1].usUsage = 6;
	dev[1].usUsagePage = 1;

	if (RegisterRawInputDevices(dev, 2, sizeof(RAWINPUTDEVICE)) == FALSE) {
		LOGE("Unable to register mouse as raw input device.");
		return false;
	}
	
	return true;
}

void createWindow(HINSTANCE hInstance) {
	LPCWSTR appname = TEXT("SMEngine");
	// Define the window class.
	WNDCLASSEX w;
	ZeroMemory(&w, sizeof(WNDCLASSEX));
	w.cbSize = sizeof(WNDCLASSEX);
	w.style = CS_HREDRAW | CS_VREDRAW;
	w.lpfnWndProc = (WNDPROC) MainWndProc;
	w.hInstance = hInstance;
	w.hIcon = NULL;
	w.hCursor = LoadCursor(NULL, IDC_ARROW);
	w.hbrBackground = NULL;
	w.lpszMenuName = appname;
	w.lpszClassName = appname;
	w.hIconSm = NULL;
    // Register the window class.
    if (!RegisterClassEx(&w)) {
		THROWEX("Failed to register window struct");
	}
	// Window style.
	DWORD exStyle = 0;
	DWORD style = WS_OVERLAPPEDWINDOW; //WS_POPUP | WS_VISIBLE;
    // Create fullscreen window.
	//int width = WIDTH; //GetSystemMetrics(SM_CXSCREEN);
	//int height = HEIGHT; //GetSystemMetrics(SM_CYSCREEN);
    g_hwnd = CreateWindowEx(exStyle, appname, appname, style,
		CW_USEDEFAULT, CW_USEDEFAULT, WIDTH, HEIGHT, 0, 0, hInstance, 0);
    if (!g_hwnd) {
		THROWEX("Failed to initialize window");
	}
}

void createEngineInstance() {
	WindowsServiceLocator* wsl = new WindowsServiceLocator();
	wsl->setScreenWidth(WIDTH);
	wsl->setScreenHeight(HEIGHT);
	GHOST = new Engine(wsl);
	if (!*GHOST) {
		THROWEX("Engine instance not created");
	}
	GHOST->resume();
}

void showWindow(int cmdShow) {
    ShowWindow(g_hwnd, cmdShow);
    UpdateWindow(g_hwnd);
}

void setupMouse() {
	// Init mouse.
	initializeRawInput(g_hwnd);
	//ShowCursor(false);
}

void runMainLoop() {
	MSG msg;
    while (GHOST && !GHOST->getServiceLocator()->isFinished())  {
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
			if (msg.message == WM_QUIT) {
				GHOST->getServiceLocator()->exit();
			}
        }
        GHOST->computeFrame();
		SwapBuffers(hDC);
    }
}

void releaseResources() {
	GHOST->pause();
	delete GHOST;
	LOGD("Engine deleted.");
	// Make sure OpenGL finishes all of its work.
	glFinish();
	glFlush();
	// Release contexts and handles.
	if (!wglMakeCurrent(NULL, NULL)) {
		LOGE("Unable to release DC and RC.");
	}
    if (!wglDeleteContext(hRC)) {
		LOGE("Unable to delete rendering context.");
	}
    ReleaseDC(g_hwnd, hDC);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int cmdShow) {
	#ifdef _DEBUG
		_CrtSetDbgFlag (_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
		_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);
	#endif
	LOGI("Main thread id: %u", hash<thread::id>()(this_thread::get_id()));
    
	try {
		createWindow(hInstance);
		setupOpenGL(WIDTH, HEIGHT);
		createEngineInstance();
		setupMouse();
		enableInput();
		showWindow(cmdShow);
		runMainLoop();
		disableInput();
		releaseResources();
	} catch (exception e) {
		LOGE(e.what());
		return -1;
	}
	
	return 777;
}

#endif
