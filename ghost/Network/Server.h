#ifndef SERVER_H_
#define SERVER_H_

#include "../Multiplatform/Ghost.h"

class ServiceLocator;
class ServerTask;

class Server {
public:
	Server(ServiceLocator* services);
	~Server();
	string receive();
	void send(string send);
private:
	ServiceLocator* services;
	vector<string> packets;
	ServerTask* serverTask;
};

#endif