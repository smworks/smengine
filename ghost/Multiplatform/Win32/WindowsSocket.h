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
	WindowsSocket(Type socketType);
	~WindowsSocket();
	void send(HttpRequest* request);
	HttpResponse* receive();
private:
	bool startWinsock();
	bool isWinsockVersionValid();
	SOCKET createSocket();
	void setSocketTimeout(SOCKET socket, long timeoutInMs);
	int getAddressFamily();
	int getSocketType();
	int getProtocol();
private:
	Type type;
	SOCKET socket;
	WSADATA wsaData;
};

#endif