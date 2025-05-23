﻿/*
 * NetworkManager.cpp
 *
 *  Created on: 2013.12.24
 *      Author: Martynas Šustavičius
 */

#include "NetworkManager.h"
#include "../Multiplatform/ServiceLocator.h"
#include "../Thread.h"
#include "../Multiplatform/Socket.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "../ThreadManager.h"

class RequestTask : public Task {
public:
	RequestTask() : socket(0), task(0), request(0), response(0) {}
	~RequestTask() {
		if (task != 0)
			task->run(response);
		delete request;
		delete task;
		delete socket;
		delete response;
	}
	void setNetworkManager(NetworkManager* networkManager) { nm = networkManager; }
	void setSocket(Socket* socket) { this->socket = socket; }
	void setRequest(HttpRequest* request) { this->request = request; }
	void setTask(NetworkManager::HttpTask* task) { this->task = task; }
	void run() {
		if (socket != 0 && request != 0) {
			socket->send(request);
			response = socket->receive();
		}
		if (response != 0)
			response->setId(request->getId());
	}
private:
	Socket* socket;
	NetworkManager* nm;
	NetworkManager::HttpTask* task;
	HttpRequest* request;
	HttpResponse* response;
};

NetworkManager::NetworkManager(ServiceLocator* services) :
	services(services)
{
	LOGD("Created network manager.");
}

NetworkManager::~NetworkManager() {
	LOGD("Deleted network manager.");
}

void NetworkManager::execute(HttpRequest* request, HttpTask* task) {
	RequestTask* requestTask = NEW RequestTask();
	requestTask->setNetworkManager(this);
	SocketParams sp(request->getHost(), request->getPort());
	sp.setSocketType(SocketParams::TCP);
	requestTask->setSocket(services->createSocket(sp));
	requestTask->setRequest(request);
	requestTask->setTask(task);
	services->getThreadManager()->execute(requestTask);
}