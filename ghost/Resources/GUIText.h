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

#define DEFAULT_TEXT_SIZE 12

class Symbol;

class GUIText : public GUISurface {
public:
	struct SymbolData {
		bool operator<(const SymbolData& right) {
			return index < right.index;
		}
		float posX;
		Mat4 matProjPosScale;
		Symbol* symbol;
		SIZE index;
	};
public:
	GUIText(ServiceLocator* services);
	~GUIText();

	/**
     * @see GUISurface
     */
    virtual bool create();

	/**
	 * @see Resource
	 */
	virtual Resource::Type getType();

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
	 * @return Size of the text.
	 */
	SIZE getTextSize();

	/**
	 * @return Text offset on X axis used to align text.
	 */
	float getTextOffsetX();

	/**
	 * @return Text offset on Y axis used to align text.
	 */
	float getTextOffsetY();

	/**
	 * @return Array of SymbolData objects.
	 */
	SymbolData* getSymbolArray();

	/**
	 * @return Symbol count in text.
	 */
	SIZE getSymbolCount();
private:
	string text_;
	SIZE size_, textOffsetX_, textOffsetY_;
	vector<SymbolData> symbols_;
};

#endif
