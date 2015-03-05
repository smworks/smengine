/*
 * HttpResonse.h
 *
 *  Created on: 2013.12.26
 *      Author: Martynas Šustavičius
 */

#ifndef HTTPRESPONSE_H_
#define HTTPRESPONSE_H_

#include "../Multiplatform/Ghost.h"

class HttpResponse {
public:
	HttpResponse(INT8* data, SIZE size);
	~HttpResponse();

	/** 
	 * @return Response HTTP header.
	 */
	string getHeader();

	/**
	 * @return Pointer to response content byte array.
	 */
	INT8* getContent();

	/**
	 * @return Size of content byte array.
	 */
	SIZE getContentSize();

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
	string header_;
	INT8* content_;
	SIZE size_;
	int id_;
};

#endif