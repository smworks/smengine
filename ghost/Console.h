/*
 * Console.h
 *
 *  Created on: 2012.10.15
 *      Author: MS
 */

#ifndef CONSOLE_H_
#define CONSOLE_H_

#include "Multiplatform/Ghost.h"

class ServiceLocator;

class Console {
public:
	Console(ServiceLocator* services);
	~Console();
	void show();
	void hide();
};

#endif /* CONSOLE_H_ */
