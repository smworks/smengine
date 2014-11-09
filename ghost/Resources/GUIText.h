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
	GUIText(ServiceLocator* services);
	~GUIText();

	/**
     * @see GUISurface
     */
    virtual bool create();

	/**
	 * @see GUISurface
	 */
	void update();

	/**
	 * @see GUISurface
	 */
	virtual void hasFocus();

	/**
	 * @param text - text that is displayed
	 * in this GUI element.
	 */
	void setText(const string& text);

	/**
	 * @return String containing text.
	 */
	string& getText();
    
    /**
     * @param size - text size in pixels.
     */
    void setTextSize(SIZE size);

	/**
	 * @return Size of the text.
	 */
	SIZE getTextSize();

	/**
	 * @return Text width.
	 */
	float getTextWidth();

	/**
	 * @return Text height.
	 */
	float getTextHeight();

	/**
	 * @return Text offset on X axis used to align text.
	 */
	float getTextOffsetX();

	/**
	 * @return Text offset on Y axis used to align text.
	 */
	float getTextOffsetY();

	/**
	 * @return Text vbo.
	 */
	SIZE getTextVBO();

	/**
	 * @return Vertex count in text VBO.
	 */
	SIZE getTextVertexCount();
private:
	string text_;
	SIZE size_, textOffsetX_, textOffsetY_;
	SIZE vbo_, vertexCount_;
	float textWidth_, textHeight_;
	vector<VertexPT> vertices_;
};

#endif
