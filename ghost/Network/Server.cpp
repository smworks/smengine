#include "Server.h"
#include "../Multiplatform/ServiceLocator.h"
#include "../Thread.h"
#include "../Multiplatform/Socket.h"
#include "Request.h"
#include "HttpResponse.h"
#include "../ThreadManager.h"

class ServerTask : public Task {
public:
	ServerTask(ServiceLocator* services) :
		services(services),
		running(true)
	{
		socket = services->createSocket(Socket::UDP);
	}
	~ServerTask() {
		delete socket;
	}
	void run() {
		Request* request = new Request("127.0.0.1:8888");
		while (running) {
			HttpResponse* response = socket->receive();
			string packet = response->getContent();
			packets.push_back(packet);
			delete response;
		}
	}
	vector<string> getPackets() {
		mut.lock();
		vector<string> p(packets);
		packets.clear();
		mut.unlock();
		return p;
	}
private:
	ServiceLocator* services;
	Socket* socket;
	mutex mut;
	vector<string> packets;
	bool running;
};

Server::Server(ServiceLocator* services) :
	services(services)
{
	serverTask = new ServerTask(services);
	LOGD("Created network manager.");
}

Server::~Server() {
	delete serverTask;
	LOGD("Deleted network manager.");
}

string Server::receive() {
	vector<string> newPackets = serverTask->getPackets();
	for (string p : newPackets) {
		packets.push_back(p);
	}
	string res = packets[packets.size() - 1];
	packets.pop_back();
	return res;
}

void Server::send(string send) {
	LOGW("SEND NOT IMPLEMENTD");
}