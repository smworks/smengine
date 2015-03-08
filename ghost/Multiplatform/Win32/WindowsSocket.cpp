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

WindowsSocket::WindowsSocket(SocketParams params) :
	params(params),
	length(sizeof(sockaddr_in)),
	socket(INVALID_SOCKET),
	available(true)
{
	PROFILE("Creating socket.");
	if (startWinsock() && isWinsockVersionValid()) {
		socket = createSocket();
		if (socket == INVALID_SOCKET) {
			logError(WSAGetLastError());
			return;
		}
		setSocketTimeout(socket, 1000);
		estabilishConnection(socket);
	}
}

WindowsSocket::~WindowsSocket() {
	available = false;
	if (socket != INVALID_SOCKET) {
		int ret = closesocket(socket);
		if (ret == SOCKET_ERROR)
			logError(WSAGetLastError());
		socket = INVALID_SOCKET;
	}
	if (WSACleanup() != 0)
		LOGW("Failed to cleanup winsock.");
	PROFILE("Deleted socket.");
}

void WindowsSocket::send(HttpRequest* request) {
	vector<INT8>& requestBuffer = request->getRequestBuffer();
	int ret;
	if (params.isForClient()) {
		ret = ::send(socket, &requestBuffer[0], requestBuffer.size(), 0);
	} else {
		ret = ::sendto(
			socket, &requestBuffer[0], requestBuffer.size(), 0,
			(struct sockaddr*) &client, length);
	}
	if (ret == SOCKET_ERROR)
		logError(WSAGetLastError());
}

HttpResponse* WindowsSocket::receive() {
	char tempBuffer[TEMP_BUFFER_SIZE];
	vector<INT8> bytes;
	int ret;
	while (true) {
		if (params.isForClient()) {
			ret = recv(socket, tempBuffer, TEMP_BUFFER_SIZE, 0);
		} else {
			ret = recvfrom(socket, tempBuffer, TEMP_BUFFER_SIZE, 0,
				(sockaddr*) &client, &length);
		}
		if (ret == 0) {
			break; // Connection has been closed
		} else if (ret == SOCKET_ERROR) {
			logError(WSAGetLastError());
			return 0;
		} else { // ret is number of bytes read.
			for (int i = 0; i < ret; i++) {
				bytes.push_back(tempBuffer[i]);
			}
			if (params.isUdp()) {
				break;
			}
		}
	}
	bytes.push_back(0);
	SIZE size = bytes.size();
	INT8* data = NEW INT8[size];
	::memcpy(data, &bytes[0], size);
	HttpResponse* response = NEW HttpResponse(data, size);
	return response;
}

void WindowsSocket::shutdown() {
	available = false;
	int ret = ::shutdown(socket, SD_BOTH);
	if (ret == SOCKET_ERROR) {
		logError(WSAGetLastError());
	}
	ret = ::closesocket(socket);
	socket = INVALID_SOCKET;
	if (ret == SOCKET_ERROR)
		logError(WSAGetLastError());
}

bool WindowsSocket::isAvailable() {
	return available;
}

bool WindowsSocket::startWinsock() {
	int res = WSAStartup(MAKEWORD(REQ_WINSOCK_VER, 0), &wsaData);
	if (res != 0)
		logError(res);
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
	return s;
}

void WindowsSocket::estabilishConnection(SOCKET socket)
{
	sockaddr_in a = {0};
	a.sin_family = params.isBth() ? AF_BTH : AF_INET;
	a.sin_port = htons(params.getPort());
	if (params.getUrl() == SocketParams::IP_ANY) {
		a.sin_addr.s_addr = INADDR_ANY;
	} else {
		HOSTENT* h = gethostbyname(params.getUrl().c_str());
		if (!h) {
			LOGW("Could not find IP for hostname: %s.",
				params.getUrl().c_str());
			return;
		}
		if (h->h_addr_list && h->h_addr_list[0]) {
			a.sin_addr.s_addr = *reinterpret_cast<IPNumber*>(
				h->h_addr_list[0]);
		} else {
			a.sin_addr.s_addr = 0;
		}
	}
	if (params.isForClient()) {
		connectToServer(a);
	} else {
		bindAsServer(a);
	}
}

void WindowsSocket::connectToServer(sockaddr_in a) {
	LOGD("Connecting to %s:%d", inet_ntoa(a.sin_addr), params.getPort());
	if (connect(socket, reinterpret_cast<sockaddr*>(&a), sizeof(a)) != 0) {
		LOGW("Could not connect using socket.");
	}
}
void WindowsSocket::bindAsServer(sockaddr_in a) {
	LOGD("Binding to %s:%d", inet_ntoa(a.sin_addr), params.getPort());
	if (::bind(socket, reinterpret_cast<sockaddr*>(&a), sizeof(a)) == SOCKET_ERROR) {
		LOGW("Could not bind using socket.");
	}
}


void WindowsSocket::setSocketTimeout(SOCKET socket, long timeoutInMs) {
	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = timeoutInMs * 1000;
	int res = setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, (const char*) &tv, sizeof(tv));
	if (res == SOCKET_ERROR)
		logError(WSAGetLastError());
}

int WindowsSocket::getAddressFamily() {
	return params.isBth() ? AF_BTH : AF_INET;
}

int WindowsSocket::getSocketType() {
	return params.isUdp() ? SOCK_DGRAM : SOCK_STREAM;
}

int WindowsSocket::getProtocol() {
	if (params.isTcp()) return IPPROTO_TCP;
	if (params.isUdp()) return IPPROTO_UDP;
	if (params.isBth()) return IPPROTO_CBT;
	ASSERT(false, "Unknown socket type");
	return IPPROTO_TCP;
}

void WindowsSocket::logError(int error) {
	switch (error) {
	case WSASYSNOTREADY:
		LOGW("The underlying network subsystem is not ready for network communication.");
		break;
	case WSAVERNOTSUPPORTED:
		LOGW("The version of Windows Sockets support requested is not provided by this particular Windows Sockets implementation.");
		break;
	case WSAEPROCLIM:
		LOGW("A limit on the number of tasks supported by the Windows Sockets implementation has been reached.");
		break;
	case WSANOTINITIALISED:
		LOGW("A successful WSAStartup call must occur before using this function.");
		break;
	case WSAENETDOWN:
		LOGW("The network subsystem or the associated service provider has failed.");
		break;
	case WSAEAFNOSUPPORT:
		LOGW("The specified address family is not supported. For example, an application tried to create a socket for the AF_IRDA address family but an infrared adapter and device driver is not installed on the local computer.");
		break;
	case WSAEINPROGRESS:
		LOGW("A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.");
		break;
	case WSAEMFILE:
		LOGW("No more socket descriptors are available.");
		break;
	case WSAEINVAL:
		LOGW("An invalid argument was supplied. This error is returned if the af parameter is set to AF_UNSPEC and the type and protocol parameter are unspecified.");
		break;
	case WSAEINVALIDPROVIDER:
		LOGW("The service provider returned a version other than 2.2.");
		break;
	case WSAEINVALIDPROCTABLE:
		LOGW("The service provider returned an invalid or incomplete procedure table to the WSPStartup.");
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
	case WSAEFAULT:
		LOGW("The buffer pointed to by the optval parameter is not in a valid part of the process address space or the optlen parameter is too small.");
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
	case WSAEINTR:
		LOGW("The (blocking) call was canceled through WSACancelBlockingCall.");
		break;
	case WSAEOPNOTSUPP:
		LOGW("MSG_OOB was specified, but the socket is not stream-style such as type SOCK_STREAM, OOB data is not supported in the communication domain associated with this socket, or the socket is unidirectional and supports only send operations.");
		break;
	case WSAESHUTDOWN:
		LOGW("The socket has been shut down; it is not possible to receive on a socket after shutdown has been invoked with how set to SD_RECEIVE or SD_BOTH.");
		break;
	case WSAEWOULDBLOCK:
		LOGW("The socket is marked as nonblocking and the receive operation would block.");
		break;
	case WSAEMSGSIZE:
		LOGW("The message was too large to fit into the specified buffer and was truncated.");
		break;
	case WSAECONNABORTED:
		LOGW("The virtual circuit was terminated due to a time-out or other failure. The application should close the socket as it is no longer usable.");
		break;
	case WSAETIMEDOUT:
		LOGW("The connection has been dropped because of a network failure or because the peer system failed to respond.");
		break;
	case WSAECONNRESET:
		LOGW("The virtual circuit was reset by the remote side executing a hard or abortive close. The application should close the socket as it is no longer usable. On a UDP-datagram socket, this error would indicate that a previous send operation resulted in an ICMP \"Port Unreachable\" message.");
		break;
	case WSAEACCES:
		LOGW("The requested address is a broadcast address, but the appropriate flag was not set. Call setsockopt with the SO_BROADCAST socket option to enable use of the broadcast address.");
		break;
	case WSAEHOSTUNREACH:
		LOGW("The remote host cannot be reached from this host at this time.");
		break;
	default:
		LOGW("Unknown windows socket error: %d.", error);
	}
}