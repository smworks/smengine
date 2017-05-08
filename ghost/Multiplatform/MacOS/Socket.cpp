/*
 * MacOSXSocket.cpp
 *
 *  Created on: 2014.01.09
 *      Author: Martynas Šustavičius
 */

#include "Socket.h"
#include "../../Network/HttpRequest.h"
#include "../../Network/HttpResponse.h"

#define TEMP_BUFFER_SIZE 512 // Size of buffer used to download result.

MacOSXSocket::MacOSXSocket() :
    sock(0)
{
	PROFILE("Created socket.");
}

MacOSXSocket::~MacOSXSocket() {
	PROFILE("Deleted socket.");
}

void MacOSXSocket::send(HttpRequest* request) {
	sockaddr_in	sockAddr = {0};
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = htons(request->getPort());
	hostent* hst;
	if (!(hst = gethostbyname(request->getHost().c_str()))) {
		LOGE("Could not find IP for hostname: %s.", request->getHost().c_str());
		return;
	}
	if (hst->h_addr_list && hst->h_addr_list[0]) {
	    sockAddr.sin_addr.s_addr = *reinterpret_cast<in_addr_t*>(hst->h_addr_list[0]);
	}
	else {
		sockAddr.sin_addr.s_addr = 0;
	}
    
	if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		LOGE("Could not create socket.");
		return;
	}
	LOGD("Connecting to %s:%d", inet_ntoa(sockAddr.sin_addr), request->getPort());
	if (connect(sock, reinterpret_cast<sockaddr*>(&sockAddr), sizeof(sockAddr)) != 0) {
		LOGE("Could not connect using socket.");
        close(sock);
        return;
	}
	vector<INT8>& requestBuffer = request->getRequestBuffer();
	if (::send(sock, &requestBuffer[0], requestBuffer.size(), SO_NOSIGPIPE) < 0) {
		LOGE("Unable to send data.");
        close(sock);
	}
}

HttpResponse* MacOSXSocket::receive() {
    char tempBuffer[TEMP_BUFFER_SIZE];
    vector<INT8>* bytes = NEW vector<INT8>();
    while (true) {
        long retval;
        retval = recv(sock, tempBuffer, sizeof(tempBuffer) - 1, 0);
        if (retval == 0) {
            break; // Connection has been closed
        }
        else if (retval < 0) {
            LOGE("Socket error while receiving data.");
        }
        else {
            // retval is number of bytes read.
            tempBuffer[retval] = 0;
            for (int i = 0; i < retval; i++) {
                bytes->push_back(tempBuffer[i]);
            }
        }
    }
    bytes->push_back(0);
    SIZE size = bytes->size();
    INT8* data = NEW INT8[size];
    memcpy(data, &(*bytes)[0], size);
    delete bytes;
    close(sock);
    return NEW HttpResponse(data, size);
}

void MacOSXSocket::shutdown() {
}

bool MacOSXSocket::isAvailable() {
    return true;
}
