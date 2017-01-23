/*
 * HttpRequest.cpp
 *
 *  Created on: 2013.12.26
 *      Author: Martynas Šustavičius
 */

#include "HttpRequest.h"
#include "../Utils.h"

HttpRequest::HttpRequest(string uri, Method method, MimeType mimeType) :
	method_(method),
	mimeType_(mimeType),
	port_(80),
	id_(0)
{
	string http = "http://";
	SIZE index = uri.find(http);
	if (index != string::npos) {
		uri = uri.replace(index, http.length(), "");
	}
	vector<string> parts = stringSplit(uri, '/');
	if (parts.size() > 0 && uri.find('/') != string::npos) {
		host_ = parts[0];
		path_ = uri.substr(uri.find('/'));
	} else {
		host_ = uri;
		path_ = "/";
	}
}

HttpRequest::~HttpRequest() {
}

string HttpRequest::getHost() {
	return host_;
}

int HttpRequest::getPort() {
	return port_;
}

void HttpRequest::setPort(int port) {
	port_ = port;
}

string HttpRequest::getMethod() {
	switch (method_) {
	case HEAD:
		return "HEAD";
	case POST:
		return "HEAD";
	default:
		return "GET";
	}
}

HttpRequest::MimeType HttpRequest::getMimeType() {
	return mimeType_;
}

void HttpRequest::addHeader(string name, string value) {
	headers_.push_back(Header(name, value));
}

vector<HttpRequest::Header>& HttpRequest::getHeaders() {
	return headers_;
}

void HttpRequest::setEntity(INT8* entity, SIZE size) {
	entity_.resize(size);
	memcpy(&entity_[0], entity, size);
	delete[] entity;
}

vector<INT8>& HttpRequest::getEntity() {
	return entity_;
}

vector<INT8>& HttpRequest::getRequestBuffer() {
	stringstream ss;
	if (method_ == HEAD) {
		ss << "HEAD ";
	} else if (method_ == POST) {
		ss << "POST ";
	} else {
		ss << "GET ";
	}
	ss << path_ << " HTTP/1.1\r\n";
	ss << "Host: " << host_ << "\r\n";
	ss << "User-agent: SMart Engine v0.1 beta\r\n";
	ss << "Connection: close\r\n";
	for (SIZE i = 0; i < headers_.size(); i++) {
		ss << headers_[i].name << ": " << headers_[i].value << "\r\n";
	}
	if (mimeType_ == FORM) {
		ss << "Content-Type: application/x-www-form-urlencoded\r\n";
	}
	if (entity_.size() > 0) {
		ss << "Content-Length: " << entity_.size() << "\r\n";
	}
	ss << "\r\n";
	string header = ss.str();
	buffer_.resize(header.length() + entity_.size());
	memcpy(&buffer_[0], header.c_str(), header.size());
	if (entity_.size() > 0) {
		memcpy(&buffer_[0] + header.size(), &entity_[0], entity_.size());
	}
	//LOGD("Request: %s", string(reinterpret_cast<const char*>(&buffer_[0]), buffer_.size()).c_str());
	return buffer_;
}

void HttpRequest::setId(int id) {
	id_ = id;
}

int HttpRequest::getId() {
	return id_;
}