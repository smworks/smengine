/*
 * AndroidSocket.h
 *
 *  Created on: 2014.01.09
 *      Author: Martynas Šustavičius
 */

#ifndef ANDROIDSOCKET_H_
#define ANDROIDSOCKET_H_

#include "../Ghost.h"
#include "../Socket.h"

class AndroidSocket : public Socket {
public:
	AndroidSocket();
	~AndroidSocket();
	HttpResponse* send(HttpRequest* request);
};

#endif
