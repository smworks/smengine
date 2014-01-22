/*
 * LinuxSocket.h
 *
 *  Created on: 2014.01.09
 *      Author: Martynas Šustavičius
 */

#ifndef LINUXSOCKET_H_
#define LINUXSOCKET_H_

#include "../Ghost.h"
#include "../Socket.h"

class LinuxSocket : public Socket {
public:
	LinuxSocket();
	~LinuxSocket();
	HttpResponse* send(HttpRequest* request);
private:
	int socket_;
};

#endif
