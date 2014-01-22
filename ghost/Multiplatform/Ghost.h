/*
 * Ghost.h
 *
 *  Created on: 2013.03.27
 *      Author: Martynas Šustavičius
 */

#ifndef GHOST_H_
#define GHOST_H_

#define SMART_PI 3.14159265358979323846

#ifdef _WIN32
#include "Win32/WindowsGhost.h"
#else
	#ifdef __ANDROID__
	#include "Android/AndroidGhost.h"
	#else
		#ifdef __linux
		#include "Linux/LinuxGhost.h"
        #else
            #ifdef __MACH__
            #include "MacOSX/SMart.h"
            #endif
		#endif
	#endif
#endif

#endif /* GHOST_H_ */
