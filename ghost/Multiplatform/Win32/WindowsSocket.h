/*
 * WindowsSocket.h
 *
 *  Created on: 2013.12.24
 *      Author: Martynas Šustavičius
 */

#ifndef WINDOWSSOCKET_H_
#define WINDOWSSOCKET_H_

#include "../Ghost.h"
#include "../Socket.h"

class WindowsSocket : public Socket {
public:
	WindowsSocket();
	~WindowsSocket();
	HttpResponse* send(HttpRequest* request);
private:
	SOCKET socket_;
	WSADATA wsaData_;
};

#endif