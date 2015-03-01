/*
 * Input.h
 *
 *  Created on: 2012.07.03
 *      Author: Martynas Šustavičius
 */

#ifndef INPUT_H_
#define INPUT_H_

#include "Multiplatform/Ghost.h"

/**
 * Input class is responsible for handling input
 * for a variety of different operating systems.
 * It takes most basic input data and tries
 * to map it to inner universal controller model.
 * Input class can take input from many different
 * input sources and distinguish different controllers
 * which is essential for multiplayer.
 * Each frame data from input sources is collected
 * into input buffer and at the end of the current
 * frame this data is mapped to the controllers.
 * @TODO implement controller interface.
 * @TODO add support for multiple controllers.
 * @TODO implement custom queue data structure
 * for fast input event handling.
 * @TODO find a way to synchronize queue.
 */
class Input {
public:
	enum InputType {UNSPECIFIED, MOUSE, MOUSE_DELTA,
		TOUCH_SENSOR, BUTTON};
	enum InputKey {NONE,
		N0, N1, N2, N3, N4, N5, N6, N7, N8, N9,
		A, B, C, D, E, F, G, H, I, J, K, L, M,
		N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
		a, b, c, d, e, f, g, h, i, j, k, l, m,
		n, o, p, q, r, s, t, u, v, w, x, y, z,
		F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
		UP, DOWN, LEFT, RIGHT, RETURN,
		PAGE_UP, PAGE_DOWN, HOME, END, INSERT, DELETE,
		BACK, SPACE, ALT, CTRL, ESC, SHIFT, TAB,
		SEMICOLON, PLUS, COMMA, MINUS, PERIOD, SLASH, TILDE,
		MOUSE_L, MOUSE_M, MOUSE_R, MOUSE_4, MOUSE_5, // Mouse buttons.
		TOUCH, // Touch screen press.
		KEY_COUNT};
	enum InputState {IDLE, PRESSED, RELEASED};

    struct InputEvent {
		InputType type;
		int x, y;
		InputState state;
		InputKey key;

		/**
         * Empty input event.
         */
		InputEvent() :
			type(UNSPECIFIED), x(0), y(0),
			state(IDLE),
			key(NONE) {}

        /**
         * Input event used to handel touch screen input.
         */
        InputEvent(int x, int y, bool pressed) :
            type(TOUCH_SENSOR), x(x), y(y),
            key(NONE)
        {
            if (pressed) {
                state = PRESSED;
            }
            else {
                state = RELEASED;
            }
        }

        /**
         * Button input event.
         */
         InputEvent(UINT32 keyIndex, InputState state) :
            type(BUTTON), x(0), y(0),
            state(state)
        {
            key = (InputKey) keyIndex;
        }
	};
public:
	Input();
	~Input();

	/**
	 * Needs to be called at the beginning
	 * of each frame to
	 * be able to get correct keyReleased() values.
	 */
	void update();

	/**
	 * Resets inputs to default values.
	 */
	void reset();

	/**
	 * @param keyIndex - key identifier.
	 * @param state - button state.
	 */
	void provideButton(UINT32 keyIndex, InputState state);

	/**
	 * @param x - pixel column of the screen.
	 * @param y - pixel row of the screen.
	 */
	void provideMousePosition(int x, int y);

	/**
	 * @param x - mouse movement difference on x axis.
	 * @param y - mouse movement difference on y axis.
	 */
	void provideMouseDelta(int dx, int dy);

	/**
	 * @param x - pixel column of the screen.
	 * @param y - pixel row of the screen.
	 * @param state - touch state.
	 */
	void provideTouch(int x, int y, InputState state);

	/**
	 * @param key - key id.
	 * @return True if key was pressed.
	 */
	bool keyPressed(UINT32 key);

	/**
	 * @param key - key id.
	 * @return True if key was released.
	 */
	bool keyReleased(UINT32 key);

	/**
	 * @return Screen x coordinate where
	 * pointer was last positioned.
	 */
	int getPointerX();

	/**
	 * @return Screen y coordinate where
	 * pointer was last positioned.
	 */
	int getPointerY();

	/**
	 * Return difference in current and last
	 * pointer X coordinate positions.
	 */
	int getPointerDeltaX();

	/**
	 * Return difference in current and last
	 * pointer Y coordinate positions.
	 */
	int getPointerDeltaY();

	/**
	 * @return Key code for last pressed key.
	 */
	UINT32 getLastKeyPressed();

	/**
	 * @return Key code for last released key.
	 */
	UINT32 getLastKeyReleased();

	/**
	 * @param key - code of the key.
	 * @return Char representing specified key.
	 */
	char codeToChar(UINT32 key);
protected:
	/**
	 * Puts new input event into input event buffer.
	 * @param inputEvent - structure that contains
	 * information about single input event.
	 */
    void provide(InputEvent& inputEvent);

	/**
	 * Sets key and it's state.
	 * @param key - key value.
	 * @param state - state in which certain key is.
	 */
	void handleKey(UINT32 key, InputState state);

	/**
	 * This method handles mouse pointer coordinates.
	 * @param x - pixel column of the screen.
	 * @param y - pixel row of the screen.
	 */
	void handleMousePosition(int x, int y);

	/**
	 * This method handles mouse movement.
	 * @param x - mouse movement on x axis.
	 * @param y - mouse movement on y axis.
	 */
	void handleMouseDelta(int dx, int dy);

	/**
	 * This method handles touch input.
	 * @param x - pixel column of the screen.
	 * @param y - pixel row of the screen.
	 * @param pressed - true if touch was active.
	 */
	void handleTouch(int x, int y, bool pressed);
public:
	UINT32 pointerX_, pointerY_;
	int pointerDeltaX_, pointerDeltaY_;
	UINT32 lastKeyPressed_, lastKeyReleased_;
	/** Array used to store all input source states. */
	InputState inputStates_[KEY_COUNT];
	/** Input event array. */
	vector<InputEvent> inputEvents_;
	/** First touch flag used to identify when screen is first touched. */
	bool firstTouch_;
};

#endif /* INPUT_H_ */
