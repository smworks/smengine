#ifndef SERVER_H_
#define SERVER_H_

#include "../Multiplatform/Ghost.h"
#include "../Thread.h"

class ServiceLocator;
class Socket;

class Server : public Task {
public:
	Server(ServiceLocator* services);
	~Server();
	void run();
private:
	ServiceLocator* services;
	Socket* socket;
	mutex mut;
};

#endif