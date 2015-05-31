/*
 * LinuxSocket.cpp
 *
 *  Created on: 2014.01.09
 *      Author: Martynas Šustavičius
 */

#include "LinuxSocket.h"
#include "../../Network/HttpRequest.h"
#include "../../Network/HttpResponse.h"

#define TEMP_BUFFER_SIZE 512 // Size of buffer used to download result.

LinuxSocket::LinuxSocket(SocketParams params)
//	params(params),
//	length(sizeof(sockaddr_in)),
//	socket(INVALID_SOCKET),
//	available(true)
{
	LOGE("Socket not implemented.");

//    if (startWinsock() && isWinsockVersionValid()) {
//		socket = createSocket();
//		if (socket == INVALID_SOCKET) {
//			return;
//		}
//		setSocketTimeout(socket, 1000);
//		estabilishConnection(socket);
//	}
}

LinuxSocket::~LinuxSocket() {
//    available = false;
//	if (socket != INVALID_SOCKET) {
//		int ret = closesocket(socket);
//		socket = INVALID_SOCKET;
//	}
	PROFILE("Deleted socket");
}

void LinuxSocket::send(HttpRequest* request) {
//	vector<INT8>& requestBuffer = request->getRequestBuffer();
//	int ret;
//	if (params.isForClient()) {
//		ret = ::send(socket, &requestBuffer[0], requestBuffer.size(), 0);
//	} else {
//		ret = ::sendto(
//			socket, &requestBuffer[0], requestBuffer.size(), 0,
//			(struct sockaddr*) &client, length);
//	}
}

HttpResponse* LinuxSocket::receive() {
//	char tempBuffer[TEMP_BUFFER_SIZE];
//	vector<INT8> bytes;
//	int ret;
//	while (true) {
//		if (params.isForClient()) {
//			ret = recv(socket, tempBuffer, TEMP_BUFFER_SIZE, 0);
//		} else {
//			ret = recvfrom(socket, tempBuffer, TEMP_BUFFER_SIZE, 0,
//				(sockaddr*) &client, &length);
//		}
//		if (ret == 0) {
//			break; // Connection has been closed
//		} else if (ret == SOCKET_ERROR) {
//			return 0;
//		} else { // ret is number of bytes read.
//			for (int i = 0; i < ret; i++) {
//				bytes.push_back(tempBuffer[i]);
//			}
//			if (params.isUdp()) {
//				break;
//			}
//		}
//	}
//	bytes.push_back(0);
//	SIZE size = bytes.size();
//	INT8* data = NEW INT8[size];
//	::memcpy(data, &bytes[0], size);
//	HttpResponse* response = NEW HttpResponse(data, size);
//	return response;
    return 0;
}

void LinuxSocket::shutdown() {
//	available = false;
//	int ret = ::shutdown(socket, SD_BOTH);
////	if (ret == SOCKET_ERROR) {
////		logError(WSAGetLastError());
////	}
//	ret = ::closesocket(socket);
//	socket = INVALID_SOCKET;
////	if (ret == SOCKET_ERROR)
////		logError(WSAGetLastError());
}

bool LinuxSocket::isAvailable() {
    //	return available;
    return false;
}

//HttpResponse* LinuxSocket::send(HttpRequest* request) {
//	char tempBuffer[TEMP_BUFFER_SIZE];
//	sockaddr_in	sockAddr = {0};
//	sockAddr.sin_family = AF_INET;
//	sockAddr.sin_port = htons(request->getPort());
//	hostent* hst;
//	if (!(hst = gethostbyname(request->getHost().c_str()))) {
//		LOGE("Could not find IP for hostname: %s.", request->getHost().c_str());
//		LOGE("Error %d: %s.", errno, strerror(errno));
//		return 0;
//	}
//	if (hst->h_addr_list && hst->h_addr_list[0]) {
//	    sockAddr.sin_addr.s_addr = *reinterpret_cast<in_addr_t*>(hst->h_addr_list[0]);
//	}
//	else {
//		sockAddr.sin_addr.s_addr = 0;
//	}
//	if ((socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
//		LOGE("Could not create socket.");
//		LOGE("Error %d: %s.", errno, strerror(errno));
//		return 0;
//	}
//	LOGD("Connecting to %s:%d", inet_ntoa(sockAddr.sin_addr), request->getPort());
//	if (connect(socket_, reinterpret_cast<sockaddr*>(&sockAddr), sizeof(sockAddr)) != 0) {
//		LOGE("Could not connect using socket.");
//		LOGE("Error %d: %s.", errno, strerror(errno));
//		return 0;
//	}
//	vector<INT8>& requestBuffer = request->getRequestBuffer();
//	if (::send(socket_, &requestBuffer[0], requestBuffer.size(), 0) < 0) {
//		LOGE("Unable to send data.");
//		LOGE("Error %d: %s.", errno, strerror(errno));
//		return 0;
//	}
//	vector<INT8>* bytes = NEW vector<INT8>();
//	while (true) {
//		int retval;
//		retval = recv(socket_, tempBuffer, sizeof(tempBuffer) - 1, 0);
//		if (retval == 0) {
//			break; // Connection has been closed
//		}
//		else if (retval < 0) {
//			LOGE("Socket error while receiving data.");
//		}
//		else {
//			// retval is number of bytes read.
//			tempBuffer[retval] = 0;
//			for (int i = 0; i < retval; i++) {
//				bytes->push_back(tempBuffer[i]);
//			}
//		}
//	}
//	bytes->push_back(0);
//	SIZE size = bytes->size();
//	INT8* data = NEW INT8[size];
//	memcpy(data, &(*bytes)[0], size);
//	delete bytes;
//	return NEW HttpResponse(data, size);
//}
