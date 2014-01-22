/*
 * Animatable.h
 *
 *  Created on: 2012.12.30
 *      Author: MS
 *
 * This interface represents scene objects that
 * contain animation.
 */

#ifndef ANIMATABLE_H_
#define ANIMATABLE_H_

#include "../Multiplatform/Ghost.h"

class Animatable {
public:
	/**
	 * Sets up next frame to be rendered.
	 * @param time - amount of milliseconds elapsed
	 * since last frame.
	 */
	virtual void nextFrame(double time) = 0;

	/**
	 * @param name - name of the animatin.
	 */
	virtual void setAnimation(string name) = 0;
private:

};

#endif /* ANIMATABLE_H_ */