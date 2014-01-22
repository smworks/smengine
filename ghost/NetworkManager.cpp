/*
 * NetworkManager.cpp
 *
 *  Created on: 2013.12.24
 *      Author: Martynas Šustavičius
 */

#include "NetworkManager.h"
#include "Multiplatform/ServiceLocator.h"
#include "Multiplatform/Thread.h"
#include "Multiplatform/Socket.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "ThreadManager.h"

class RequestTask : public Task {
public:
	void setNetworkManager(NetworkManager* networkManager) { nm_ = networkManager; }
	void setSocket(Socket* socket) { socket_ = socket; }
	void setRequest(HttpRequest* request) { request_ = request; }
	void setTask(NetworkManager::HttpTask* task) { task_ = task; }
	void run() {
		response_ = socket_->send(request_);
		if (response_ != 0) {
			response_->setId(request_->getId());
		}
		delete socket_;
		delete request_;
	}
	void finish() {
		if (task_ != 0) {
			task_->run(response_);
			delete task_;
			task_ = 0;
		}
		if (response_ != 0) {
			delete response_;
		}
		nm_->update();
	}
	Socket* socket_;
	NetworkManager* nm_;
	NetworkManager::HttpTask* task_;
	HttpRequest* request_;
	HttpResponse* response_;
};

NetworkManager::NetworkManager(ServiceLocator* services) : services_(services) {
	LOGD("Created network manager.");
}

NetworkManager::~NetworkManager() {
	while (queue_.size() > 0) {
		delete queue_.front();
		queue_.pop();
	}
	LOGD("Deleted network manager.");
}

void NetworkManager::execute(HttpRequest* request, HttpTask* task) {
	RequestTask* requestTask = NEW RequestTask();
	requestTask->setNetworkManager(this);
	requestTask->setSocket(services_->createSocket());
	requestTask->setRequest(request);
	requestTask->setTask(task);
	queue_.push(requestTask);
	if (queue_.size() == 1) {
		services_->getTM()->execute(requestTask);
	}
}

void NetworkManager::update() {
	queue_.pop();
	if (queue_.size() > 0) {
		Task* task = queue_.front();
		services_->getTM()->execute(task);
	}
}