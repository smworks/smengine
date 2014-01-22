/*
 * Socket.h
 *
 *  Created on: 2014.01.09
 *      Author: Martynas Šustavičius
 */

#ifndef MACOSXSOCKET_H_
#define MACOSXSOCKET_H_

#include "../Ghost.h"
#include "../Socket.h"

class MacOSXSocket : public Socket {
public:
	MacOSXSocket();
	~MacOSXSocket();
	HttpResponse* send(HttpRequest* request);
};

#endif
