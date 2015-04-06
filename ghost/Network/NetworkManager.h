#ifndef NETWORKMANAGER_H_
#define NETWORKMANAGER_H_

#include "../Multiplatform/Ghost.h"

class ServiceLocator;
class Socket;
class HttpRequest;
class HttpResponse;
class Task;
class Server;

class NetworkManager {
public:
	class HttpTask {
	public:
        virtual ~HttpTask() {}
		virtual void run(HttpResponse* response) = 0;		
	};
public:
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
private:
	ServiceLocator* services;
};

#endif