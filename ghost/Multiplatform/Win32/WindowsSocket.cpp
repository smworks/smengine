/*
 * WindowsSocket.cpp
 *
 *  Created on: 2013.12.24
 *      Author: Martynas Šustavičius
 */

#include "WindowsSocket.h"
#include "../../Network/HttpRequest.h"
#include "../../Network/HttpResponse.h"

#define REQ_WINSOCK_VER 2 // Minimum winsock version required.
#define TEMP_BUFFER_SIZE 512 // Size of buffer used to download result.
typedef unsigned long IPNumber; // IP number typedef for IPv4.

WindowsSocket::WindowsSocket(Type socketType) :
	type(socketType),
	socket(INVALID_SOCKET)
{
	PROFILE("Creating socket.");
	if (startWinsock() && isWinsockVersionValid()) {
		socket = createSocket();
		setSocketTimeout(socket, 1000);
	}
}



WindowsSocket::~WindowsSocket() {
	if (socket != INVALID_SOCKET) {
		closesocket(socket);
		socket = INVALID_SOCKET;
	}
	if (WSACleanup() != 0)
		LOGW("Failed to cleanup winsock.");
	PROFILE("Deleted socket.");
}

void WindowsSocket::send(HttpRequest* request) {
	sockaddr_in address = {0};
	address.sin_family = type == BTH ? AF_BTH : AF_INET;
	address.sin_port = htons(request->getPort());
	HOSTENT* hostent;
	if (!(hostent = gethostbyname(request->getHost().c_str()))) {
		LOGW("Could not find IP for hostname: %s.",
			request->getHost().c_str());
		return;
	}
	if (hostent->h_addr_list && hostent->h_addr_list[0]) {
		address.sin_addr.S_un.S_addr = *reinterpret_cast<IPNumber*>(hostent->h_addr_list[0]);
	}
	else {
		address.sin_addr.S_un.S_addr = 0;
	}
	LOGD("Connecting to %s:%d", inet_ntoa(address.sin_addr), request->getPort());
	if (connect(socket, reinterpret_cast<sockaddr*>(&address), sizeof(address)) != 0) {
		LOGW("Could not connect using socket.");
		return;
	}
	vector<INT8>& requestBuffer = request->getRequestBuffer();
	if (::send(socket, &requestBuffer[0], requestBuffer.size(), 0) == SOCKET_ERROR) {
		LOGE("Unable to send data.");
		return;
	}
}

HttpResponse* WindowsSocket::receive() {
	char tempBuffer[TEMP_BUFFER_SIZE];
	vector<INT8> bytes;
	while (true) {
		int retval = recv(socket, tempBuffer, sizeof(tempBuffer) - 1, 0);
		if (retval == 0) {
			break; // Connection has been closed
		} else if (retval == SOCKET_ERROR) {
			LOGW("Socket error while receiving data.");
			return 0;
		} else {
			// retval is number of bytes read.
			tempBuffer[retval] = 0;
			for (int i = 0; i < retval; i++) {
				bytes.push_back(tempBuffer[i]);
			}
			if (type == UDP) {
				break;
			}
		}
	}
	bytes.push_back(0);
	SIZE size = bytes.size();
	INT8* data = NEW INT8[size];
	::memcpy(data, &bytes[0], size);
	return NEW HttpResponse(data, size);
}

bool WindowsSocket::startWinsock() {
	int res = WSAStartup(MAKEWORD(REQ_WINSOCK_VER, 0), &wsaData);
	switch (res) {
	case WSASYSNOTREADY:
		LOGE("The underlying network subsystem is not ready for network communication.");
		break;
	case WSAVERNOTSUPPORTED:
		LOGE("The version of Windows Sockets support requested is not provided by this particular Windows Sockets implementation.");
		break;
	case WSAEINPROGRESS:
		LOGE("A blocking Windows Sockets 1.1 operation is in progress.");
		break;
	case WSAEPROCLIM:
		LOGE("A limit on the number of tasks supported by the Windows Sockets implementation has been reached.");
		break;
	case WSAEFAULT:
		LOGE("The lpWSAData parameter is not a valid pointer.");
		break;
	}
	return res == 0;
}

bool WindowsSocket::isWinsockVersionValid() {
	if (LOBYTE(wsaData.wVersion) < REQ_WINSOCK_VER) {
		LOGE("Winsock version is lower than required. "
			"Required: %d, current: %d.",
			REQ_WINSOCK_VER, LOBYTE(wsaData.wVersion));
		return false;
	}
	return true;
}

SOCKET WindowsSocket::createSocket() {
	SOCKET s = ::socket(getAddressFamily(), getSocketType(), getProtocol());
	switch (s) {
	case WSANOTINITIALISED:
		LOGE("A successful WSAStartup call must occur before using this function.");
		break;
	case WSAENETDOWN:
		LOGE("The network subsystem or the associated service provider has failed.");
		break;
	case WSAEAFNOSUPPORT:
		LOGE("The specified address family is not supported. For example, an application tried to create a socket for the AF_IRDA address family but an infrared adapter and device driver is not installed on the local computer.");
		break;
	case WSAEINPROGRESS:
		LOGE("A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.");
		break;
	case WSAEMFILE:
		LOGE("No more socket descriptors are available.");
		break;
	case WSAEINVAL:
		LOGE("An invalid argument was supplied. This error is returned if the af parameter is set to AF_UNSPEC and the type and protocol parameter are unspecified.");
		break;
	case WSAEINVALIDPROVIDER:
		LOGE("The service provider returned a version other than 2.2.");
		break;
	case WSAEINVALIDPROCTABLE:
		LOGE("The service provider returned an invalid or incomplete procedure table to the WSPStartup.");
		break;
	case WSAENOBUFS:
		LOGE("No buffer space is available. The socket cannot be created.");
		break;
	case WSAEPROTONOSUPPORT:
		LOGE("The specified protocol is not supported.");
		break;
	case WSAEPROTOTYPE:
		LOGE("The specified protocol is the wrong type for this socket.");
		break;
	case WSAEPROVIDERFAILEDINIT:
		LOGE("The service provider failed to initialize. This error is returned if a layered service provider (LSP) or namespace provider was improperly installed or the provider fails to operate correctly.");
		break;
	case WSAESOCKTNOSUPPORT:
		LOGE("The specified socket type is not supported in this address family.");
		break;
	}
	return s;
}

void WindowsSocket::setSocketTimeout(SOCKET socket, long timeoutInMs) {
	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = timeoutInMs * 1000;
	int res = setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, (const char*) &tv, sizeof(tv));
	switch (res) {
	case WSANOTINITIALISED:
		LOGW("A successful WSAStartup call must occur before using this function.");
		break;
	case WSAENETDOWN:
		LOGW("The network subsystem has failed.");
		break;
	case WSAEFAULT:
		LOGW("The buffer pointed to by the optval parameter is not in a valid part of the process address space or the optlen parameter is too small.");
		break;
	case WSAEINPROGRESS:
		LOGW("A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.");
		break;
	case WSAEINVAL:
		LOGW("The level parameter is not valid, or the information in the buffer pointed to by the optval parameter is not valid.");
		break;
	case WSAENETRESET:
		LOGW("The connection has timed out when SO_KEEPALIVE is set.");
		break;
	case WSAENOPROTOOPT:
		LOGW("The option is unknown or unsupported for the specified provider or socket (see SO_GROUP_PRIORITY limitations).");
		break;
	case WSAENOTCONN:
		LOGW("The connection has been reset when SO_KEEPALIVE is set.");
		break;
	case WSAENOTSOCK:
		LOGW("The descriptor is not a socket.");
		break;
	}
}

int WindowsSocket::getAddressFamily() {
	return type == BTH ? AF_BTH : AF_INET;
}

int WindowsSocket::getSocketType() {
	return type == UDP ? SOCK_DGRAM : SOCK_STREAM;
}

int WindowsSocket::getProtocol() {
	switch (type) {
	case TCP:
		return IPPROTO_TCP;
	case UDP:
		return IPPROTO_UDP;
	case BTH:
		return IPPROTO_CBT;
	}
	ASSERT(false, "Unknown socket type");
	return IPPROTO_TCP;
}