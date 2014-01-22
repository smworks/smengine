/*
 * HttpResonse.cpp
 *
 *  Created on: 2013.12.26
 *      Author: Martynas Šustavičius
 */

#include "HttpResponse.h"

HttpResponse::HttpResponse(INT8* data, SIZE size) : header_(""), content_(0), size_(0), id_(0) {
	for (SIZE i = 0; i < size; i++) {
		if (data[i] == '\r') {
			if (i < size - 3 && data[i + 1] == '\n' && data[i + 2] == '\r') {
				header_ = string(reinterpret_cast<const char*>(data), i);
				size_ = size - i - 4;
				content_ = NEW INT8[size_];
				memcpy(content_, data + i + 4, size_);
				break;
			}
		}
	}
	//LOGD("Header: %s", header_.c_str());
	//LOGD("Content: %s", string(reinterpret_cast<const char*>(content_), size_).c_str());
	delete [] data;
}

HttpResponse::~HttpResponse() {
	delete [] content_;
}

string HttpResponse::getHeader() {
	return header_;
}

INT8* HttpResponse::getContent() {
	return content_;
}

SIZE HttpResponse::getContentSize() {
	return size_;
}

void HttpResponse::setId(int id) {
	id_ = id;
}

int HttpResponse::getId() {
	return id_;
}
