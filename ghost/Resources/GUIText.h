/*
 * GUIText.h
 *
 *  Created on: 2013.07.13
 *      Author: Martynas Šustavičius
 */

#ifndef GUITEXT_H_
#define GUITEXT_H_

#include "GUISurface.h"
#include "../Matrix.h"
#include "Vertex.h"

#define DEFAULT_TEXT_SIZE 12

class Symbol;

class GUIText : public GUISurface {
public:
	explicit GUIText(ServiceLocator* services);
	~GUIText();

	bool create() override;

	void update() override;

	void hasFocus() override;

	void setText(const string& text);

	string& getText();

    void setFontSize(SIZE size);

	SIZE getFontSize() const;

    SIZE getTextWidth() const;

    SIZE getTextHeight() const;

	SIZE getTextVBO() const;

	SIZE getTextVertexCount() const;
private:
	string text;
	SIZE size;
	UINT32 vbo;
	UINT32 vertexCount;
	float textWidth;
	float textHeight;
	vector<VertexPT> vertices;
};

#endif
