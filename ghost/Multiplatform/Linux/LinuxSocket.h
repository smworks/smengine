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
#include "../../Network/SocketParams.h"

class LinuxSocket : public Socket {
public:
	LinuxSocket(SocketParams params);
	~LinuxSocket();
	void send(HttpRequest* request);
	HttpResponse* receive();
	void shutdown();
	bool isAvailable();
private:
//    sockaddr_in client;
//	int length;
//	SOCKET socket;
//	WSADATA wsaData;
//	SocketParams params;
//	bool available;
};

#endif
