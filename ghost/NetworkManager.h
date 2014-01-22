/*
 * NetworkManager.h
 *
 *  Created on: 2013.12.24
 *      Author: Martynas Šustavičius
 */

#ifndef NETWORKMANAGER_H_
#define NETWORKMANAGER_H_

#include "Multiplatform/Ghost.h"

class ServiceLocator;
class Socket;
class HttpRequest;
class HttpResponse;
class Task;

class NetworkManager {
public:
	class HttpTask {
	public:
        virtual ~HttpTask() {}
		virtual void run(HttpResponse* response) = 0;		
	};
public:
	/**
	 * @param services - pointer to service locator object.
	 */
	NetworkManager(ServiceLocator* services);
	~NetworkManager();

	/**
	 * Method executes specified request on separate thread,
	 * when result is received it will be added to response queue.
	 * @param request - HTTP protocol request that
	 * contains headers and data as well as destination
	 * address.
	 * @param task - task class that implements run method
	 * which will handle response. Can be null if there is no need to
	 * handle result.
	 */
	void execute(HttpRequest* request, HttpTask* task = 0);

	/**
	 * This is called after each task execution to check
	 * if there are any other tasks that need to be launched.
	 * @NOTE: Do not call this from anywhere else!
	 */
	void update();
private:
	ServiceLocator* services_;
	queue<Task*> queue_;
};

#endif