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
#include "../../Network/SocketParams.h"

class WindowsSocket : public Socket {
public:
	WindowsSocket(SocketParams params);
	~WindowsSocket();
	void send(HttpRequest* request);
	HttpResponse* receive();
	void shutdown();
	bool isAvailable();
private:
	bool startWinsock();
	bool isWinsockVersionValid();
	SOCKET createSocket();
	void estabilishConnection(SOCKET socket);
	void connectToServer(sockaddr_in address);
	void bindAsServer(sockaddr_in address);
	void setSocketTimeout(SOCKET socket, long timeoutInMs);
	int getAddressFamily();
	int getSocketType();
	int getProtocol();
	void logError(int error);
private:
	sockaddr_in client;
	int length;
	SOCKET socket;
	WSADATA wsaData;
	SocketParams params;
	bool available;
};

#endif