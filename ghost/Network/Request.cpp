#include "Request.h"
#include "../Utils.h"

Request::Request(string url) :
	ip(""),
	port(0),
	id(0)
{
	vector<string> parts = stringSplit(url, ':');
	if (parts.size() == 2) {
		ip = parts[0];
		port = toInt(parts[1].c_str());
	} else {
		ip = url;
	}
}

Request::~Request() {
}

string Request::getIp() {
	return ip;
}

int Request::getPort() {
	return port;
}

void Request::setData(INT8* entity, SIZE size) {
	data.resize(size);
	memcpy(&data[0], entity, size);
	delete[] entity;
}

vector<INT8>& Request::getData() {
	return data;
}

void Request::setId(int id) {
	this->id = id;
}

int Request::getId() {
	return id;
}
