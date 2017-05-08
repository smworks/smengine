#ifndef MACOSXSOCKET_H_
#define MACOSXSOCKET_H_

#include "../Ghost.h"
#include "../Socket.h"

class MacOSXSocket : public Socket {
public:
	MacOSXSocket();
	~MacOSXSocket();
    void send(HttpRequest* request);
    HttpResponse* receive();
    void shutdown();
    bool isAvailable();
private:
    int sock;
};

#endif
