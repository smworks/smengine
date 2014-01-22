/*
 * WindowsSocket.cpp
 *
 *  Created on: 2013.12.24
 *      Author: Martynas Šustavičius
 */

#include "WindowsSocket.h"
#include "../../HttpRequest.h"
#include "../../HttpResponse.h"

#define REQ_WINSOCK_VER 2 // Minimum winsock version required.
#define TEMP_BUFFER_SIZE 512 // Size of buffer used to download result.
typedef unsigned long IPNumber; // IP number typedef for IPv4.

WindowsSocket::WindowsSocket() : socket_(INVALID_SOCKET) {
	PROFILE("Creating socket.");
}

WindowsSocket::~WindowsSocket() {
	PROFILE("Deleted socket.");
}

HttpResponse* WindowsSocket::send(HttpRequest* request) {
    if (WSAStartup(MAKEWORD(REQ_WINSOCK_VER,0), &wsaData_) != 0) {
		LOGE("Failed to start WSA.");
		return 0;
	}
    // Check if major version is at least REQ_WINSOCK_VER
    if (LOBYTE(wsaData_.wVersion) < REQ_WINSOCK_VER) {
		LOGE("Winsocks version is lower than required. Required: %d, current: %d.",
			REQ_WINSOCK_VER, LOBYTE(wsaData_.wVersion));
		return 0;
	}
	char tempBuffer[TEMP_BUFFER_SIZE];
	sockaddr_in	sockAddr = {0};
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = htons(request->getPort());
	HOSTENT* hostent;
	if (!(hostent = gethostbyname(request->getHost().c_str()))) {
		LOGW("Could not find IP for hostname: %s.", request->getHost().c_str());
		return 0;
	}
	if (hostent->h_addr_list && hostent->h_addr_list[0]) {
		sockAddr.sin_addr.S_un.S_addr = *reinterpret_cast<IPNumber*>(hostent->h_addr_list[0]);
	}
	else {
		sockAddr.sin_addr.S_un.S_addr = 0;
	}
	if ((socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET) {
		LOGW("Could not create socket.");
		return 0;
	}
	LOGD("Connecting to %s:%d", inet_ntoa(sockAddr.sin_addr), request->getPort());
	if (connect(socket_, reinterpret_cast<sockaddr*>(&sockAddr), sizeof(sockAddr)) != 0) {
		LOGW("Could not connect using socket.");
		if (WSACleanup() != 0) {
			LOGW("Failed to cleanup WSA.");
		}
		if (socket_ != INVALID_SOCKET) {
			closesocket(socket_);
		}
		return 0;
	}
	vector<INT8>& requestBuffer = request->getRequestBuffer();
	if (::send(socket_, &requestBuffer[0], requestBuffer.size(), 0) == SOCKET_ERROR) {
		LOGE("Unable to send data.");
		if (WSACleanup() != 0) {
			LOGW("Failed to cleanup WSA.");
		}
		if (socket_ != INVALID_SOCKET) {
			closesocket(socket_);
		}
		return 0;
	}
	vector<INT8> bytes;
	while (true) {
		int retval;
		retval = recv(socket_, tempBuffer, sizeof(tempBuffer) - 1, 0);
		if (retval == 0) { 
			break; // Connection has been closed
		}
		else if (retval == SOCKET_ERROR) {
			LOGW("Socket error while receiving data.");
			if (WSACleanup() != 0) {
				LOGW("Failed to cleanup WSA.");
			}
			if (socket_ != INVALID_SOCKET) {
				closesocket(socket_);
			}
			return 0;
		}
		else {
			// retval is number of bytes read.
			tempBuffer[retval] = 0;
			for (int i = 0; i < retval; i++) {
				bytes.push_back(tempBuffer[i]);
			}
		}
	}
	if (WSACleanup() != 0) {
		LOGW("Failed to cleanup WSA.");
	}
	if (socket_ != INVALID_SOCKET) {
		closesocket(socket_);
	}
	bytes.push_back(0);
	SIZE size = bytes.size();
	INT8* data = NEW INT8[size];
	memcpy(data, &bytes[0], size);
	return NEW HttpResponse(data, size);
}