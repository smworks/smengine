/*
 * Socket.h
 *
 *  Created on: 2013.12.24
 *      Author: Martynas Šustavičius
 */

#ifndef SOCKET_H_
#define SOCKET_H_

#include "Ghost.h"

class HttpRequest;
class HttpResponse;

class Socket {
public:
	virtual ~Socket() {};
	virtual void send(HttpRequest* request) = 0;
	virtual HttpResponse* receive() = 0;
	virtual void shutdown() = 0;
	virtual bool isAvailable() = 0;
};

#endif