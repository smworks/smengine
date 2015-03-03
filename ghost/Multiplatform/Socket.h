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
	enum Type {TCP, UDP, BTH};
public:
	virtual ~Socket() {};
	virtual void send(HttpRequest* request) = 0;
	virtual HttpResponse* receive() = 0;
};

#endif