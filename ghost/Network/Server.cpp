#include "Server.h"
#include "../Multiplatform/ServiceLocator.h"
#include "../Thread.h"
#include "../Multiplatform/Socket.h"
//#include "Request.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "../ThreadManager.h"
#include "../ScriptManager.h"
#include <future>

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
			HttpRequest* request = new HttpRequest("");
			future<string> fut = async(launch::async,
				&ScriptManager::provideServerResponse, services->getScriptManager(), r);
			string entity = fut.get();
			INT8* bytes = NEW INT8[entity.size()];
			memcpy(bytes, entity.c_str(), entity.size());
			request->setEntity(bytes, entity.size());
			socket->send(request);
			delete request;
		}
		delete r;
	}
	mut.unlock();
}