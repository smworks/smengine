#ifndef HTTPREQUEST_H_
#define HTTPREQUEST_H_

#include "../Multiplatform/Ghost.h"

class Request {
public:
	Request(string url);
	~Request();

	string getIp();
	int getPort();
	void setData(INT8* entity, SIZE size);
	vector<INT8>& getData();
	void setId(int id);
	int getId();
private:
	string ip;
	int port;
	vector<INT8> data;
	int id;
};

#endif