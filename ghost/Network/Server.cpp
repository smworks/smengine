#include "Server.h"
#include "../Multiplatform/ServiceLocator.h"
#include "../Thread.h"
#include "../Multiplatform/Socket.h"
//#include "Request.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "../ThreadManager.h"
#include "../ScriptManager.h"
#include "../Engine.h"

class ServerTask : public Task {
public:
	ServerTask(ServiceLocator* services, HttpResponse* response, Socket* socket) :
		services(services), response(response), socket(socket)
	{
		HttpRequest* request = new HttpRequest("");
		string entity = services->getScriptManager()->provideServerResponse(response);
		INT8* bytes = NEW INT8[entity.size()];
		memcpy(bytes, entity.c_str(), entity.size());
		request->setEntity(bytes, entity.size());
		socket->send(request);
		delete request;
		delete response;
	}

	void run() {
		
	}
private:
	ServiceLocator* services;
	HttpResponse* response;
	Socket* socket;
};

Server::Server(ServiceLocator* services) :
	services(services)
{
	SocketParams sp(SocketParams::IP_ANY, 8888);
	sp.setConnectionType(SocketParams::SERVER);
	sp.setSocketType(SocketParams::UDP);
	socket = services->createSocket(sp);
	LOGD("Created server.");
}

Server::~Server() {
	socket->shutdown();
	mut.lock();
	mut.unlock();
	delete socket;
	LOGD("Deleted server.");
}

void Server::run() {
	mut.lock();
	while (socket->isAvailable()) {
		HttpResponse* r = socket->receive();
		string packet = r == 0 ? "" : r->getContent();
		if (packet.length() > 0) {
			Engine::executeOnMainThread(new ServerTask(
				services, r, socket));
		}
	}
	mut.unlock();
}