#ifndef SOCKETPARAMS_H_
#define SOCKETPARAMS_H_

#include "../Multiplatform/Ghost.h"

class SocketParams {
public:
	enum SocketType {TCP, UDP, BTH};
	enum ConnectionType {SERVER, CLIENT};
	static const string IP_ANY;
public:
	SocketParams(string url, int port);
	void setSocketType(SocketType type);
	void setConnectionType(ConnectionType type);
	string getUrl();
	int getPort();
	bool isTcp();
	bool isUdp();
	bool isBth();
	bool isForServer();
	bool isForClient();
	~SocketParams();
private:
	string url;
	int port;
	SocketType socketType;
	ConnectionType connectionType;
};

#endif