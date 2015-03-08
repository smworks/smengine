#include "SocketParams.h"

const string SocketParams::IP_ANY = "any_ip";

SocketParams::SocketParams(string url, int port) :
	url(url),
	port(port),
	socketType(TCP),
	connectionType(CLIENT)
{}

SocketParams::~SocketParams() {}

void SocketParams::setSocketType(SocketType type) {
	socketType = type;
}

void SocketParams::setConnectionType(ConnectionType type) {
	connectionType = type;
}

string SocketParams::getUrl() {
	return url;
}

int SocketParams::getPort() {
	return port;
}

bool SocketParams::isTcp() {
	return socketType == TCP;
}

bool SocketParams::isUdp() {
	return socketType == UDP;
}

bool SocketParams::isBth() {
	return socketType == BTH;
}

bool SocketParams::isForServer() {
	return connectionType == SERVER;
}

bool SocketParams::isForClient() {
	return connectionType == CLIENT;
}