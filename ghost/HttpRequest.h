/*
 * HttpRequest.h
 *
 *  Created on: 2013.12.26
 *      Author: Martynas Šustavičius
 */

#ifndef HTTPREQUEST_H_
#define HTTPREQUEST_H_

#include "Multiplatform/Ghost.h"

class HttpRequest {
public:
	enum Method {
		HEAD, POST, GET
	};
	enum MimeType {
		RAW, FORM
	};
	struct Header {
		Header(string n, string v) : name(n), value(v) {}
		string name;
		string value;
	};
public:
	/**
	 * Creates HTTP request for specified uri.
	 * @param uri - address of content.
	 * @param method - request method. Default is GET.
	 * @param mimeType - mime type that will be used for entity. Default is RAW.
	 */
	HttpRequest(string uri, Method method = GET, MimeType mimeType = RAW);
	~HttpRequest();

	/**
	 * @return Host name.
	 */
	string getHost();

	/**
	 * @return Used port.
	 */
	int getPort();

	/**
	 * @return Request method.
	 */
	string getMethod();

	/**
	 * @return Requset content MIME type.
	 */
	MimeType getMimeType();

	/**
	 * Adds header to request.
	 * @param name - header name.
	 * @param value - header value.
	 */
	void addHeader(string name, string value);

	/**
	 * When called, generates and returns
	 * all headers in one string.
	 * @return All headers in one string.
	 */
	vector<Header>& getHeaders();

	/**
	 * Adds entity to request.
	 * @param entity - pointer to entity array.
	 * @param size - size of the entity in bytes.
	 */
	void setEntity(INT8* entity, SIZE size);

	/**
	 * @return Entity data or 0 if no entity is provided.
	 */
	vector<INT8>& getEntity();

	/**
	 * @return Byte buffer containing request headers
	 * and entity.
	 */
	vector<INT8>& getRequestBuffer();

	/**
	 * Sets id that is used to match request with response.
	 * @param id - request id.
	 */
	void setId(int id);

	/**
	 * @return Request id.
	 */
	int getId();
private:
	string host_;
	string path_;
	Method method_;
	MimeType mimeType_; 
	int port_;
	vector<Header> headers_;
	vector<INT8> entity_;
	vector<INT8> buffer_;
	int id_;
};

#endif