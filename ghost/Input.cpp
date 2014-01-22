/*
 * Input.cpp
 *
 *  Created on: 2012.07.03
 *      Author: MS
 */

#include "Input.h"

Input::Input() :
	pointerX_(0),
	pointerY_(0),
	pointerDeltaX_(0),
	pointerDeltaY_(0),
	lastKeyPressed_(NONE),
	lastKeyReleased_(NONE),
	firstTouch_(true)
{
	reset();
	LOGD("Created input.");
}

Input::~Input() {
	LOGD("Deleted input.");
}

void Input::update() {
	for (UINT32 i = 0; i < KEY_COUNT; i++) {
		if (inputStates_[i] == RELEASED) {
			inputStates_[i] = IDLE;
		}
	}
    pointerDeltaX_ = pointerDeltaY_ = 0;
    vector<InputEvent>::iterator it = inputEvents_.begin();
    while (it != inputEvents_.end()) {
        InputEvent& event = (*it);
        switch (event.type) {
        case TOUCH_SENSOR:
            handleTouch(
				event.x, event.y, event.state == PRESSED ? true : false);
            break;
		case MOUSE:
			handleMousePosition(event.x, event.y);
			break;
		case MOUSE_DELTA:
			handleMouseDelta(event.x, event.y);
			break;
        case BUTTON:
            handleKey(event.key, event.state);
            break;
        case UNSPECIFIED:
            break;
        default:
            LOGW("Not recognized input type detected: %d.", event.type);
        }
        it++;
    }
    inputEvents_.clear();
}

void Input::reset() {
	fill(inputStates_, inputStates_ + KEY_COUNT, IDLE);
}

void Input::provideButton(UINT32 keyIndex, InputState state) {
	InputEvent ie;
	ie.type = BUTTON;
	ie.key = (InputKey) keyIndex;
	ie.state = state;
	provide(ie);
}

void Input::provideMousePosition(int x, int y) {
	InputEvent ie;
	ie.type = MOUSE;
	ie.x = x;
	ie.y = y;
	provide(ie);
}

void Input::provideMouseDelta(int x, int y) {
	InputEvent ie;
	ie.type = MOUSE_DELTA;
	ie.x = x;
	ie.y = y;
	provide(ie);
}

void Input::provideTouch(int x, int y, InputState state) {
	InputEvent ie;
	ie.type = TOUCH_SENSOR;
	ie.x = x;
	ie.y = y;
	ie.state = state;
	provide(ie);
}

bool Input::keyPressed(UINT32 key) {
	return key > KEY_COUNT ?
		false : inputStates_[key] == PRESSED;
}

bool Input::keyReleased(UINT32 key) {
	return key > KEY_COUNT ?
        false : inputStates_[key] == RELEASED;
}

int Input::getPointerX() {
	return pointerX_;
}

int Input::getPointerY() {
	return pointerY_;
}

int Input::getPointerDeltaX() {
	return pointerDeltaX_;
}

int Input::getPointerDeltaY() {
	return pointerDeltaY_;
}

UINT32 Input::getLastKeyPressed() {
	return lastKeyPressed_;
}

UINT32 Input::getLastKeyReleased() {
	return lastKeyReleased_;
}

char Input::codeToChar(UINT32 key) {
	return (char) key;
}

void Input::provide(InputEvent& inputEvent) {
	inputEvents_.push_back(inputEvent);
}

void Input::handleKey(UINT32 key, InputState keyState) {
	if (key > KEY_COUNT) {
		return;
	}
	if (keyState == PRESSED) {
		lastKeyPressed_ = (InputKey) key;
	} else if (keyState == RELEASED) {
		lastKeyReleased_ = (InputKey) key;
	}
	inputStates_[key] = keyState;
}

void Input::handleMousePosition(int x, int y) {
	pointerX_ = x;
	pointerY_ = y;
}

void Input::handleMouseDelta(int x, int y) {
	pointerDeltaX_ += x;
    pointerDeltaY_ += y;
}

void Input::handleTouch(int x, int y, bool pressed) {
	if (pressed) {
		handleKey(TOUCH, PRESSED);
		if (firstTouch_) {
			pointerX_ = x;
			pointerY_ = y;
			firstTouch_ = false;
		}
	}
	else {
		handleKey(TOUCH, RELEASED);
		firstTouch_ = true;
	}
	pointerDeltaX_ += x - pointerX_;
    pointerDeltaY_ += pointerY_ - y;
	pointerX_ = x;
	pointerY_ = y;
}