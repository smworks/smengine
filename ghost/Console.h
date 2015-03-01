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
class CMDTask;

class Console {
public:
	Console(ServiceLocator* services);
	~Console();
	bool isShowing();
	void show();
private:
	ServiceLocator* services;
	CMDTask* cmdTask;
};

#endif /* CONSOLE_H_ */
