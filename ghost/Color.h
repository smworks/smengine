/*
 * Color.h
 *
 *  Created on: 2012.10.25
 *      Author: MS
 */

#ifndef COLOR_H_
#define COLOR_H_

#include "Multiplatform/Ghost.h"

#define CLAMP(val) if (val > 1.0f) val = 1.0f; if (val < 0.0f) val = 0.0f;

/**
 * Creates color object that contains RGBA color components.
 * Default color is white (1.0f, 1.0f, 1.0f, 1.0f).
 * All values ar clamped to range from 0.0f to 1.0f.
 */
class Color
{
public:
	/** Default constructor. Initializes color to white. */
	Color();

	/**
	 * @param color - string containing color in hex format.
	 */
	Color(const string& color);

	/**
	 * @param color - color value for each RGBA component.
	 */
	Color(float color);

	/**
	 * Initializes RGB color components. A component is set to 1.0f.
	 * @param r, g, b - color components.
	 */
	Color(float r, float g, float b);

	/**
	 * Initializes all color components.
	 * @param r, g, b, a - color components.
	 */
	Color(float r, float g, float b, float a);

	~Color();

	/**
	 * Sets red color component.
	 * @param val - red color component.
	 */
	void setR(float val);

	/**
	 * Sets green color component.
	 * @param val - green color component.
	 */
	void setG(float val);

	/**
	 * Sets blue color component.
	 * @param val - blue color component.
	 */
	void setB(float val);

	/**
	 * Sets alpha color component.
	 * @param val - alpha color component.
	 */
	void setA(float val);

	/**
	 * Sets RGB color components. A component is set to 1.0f.
	 * @param r, g, b - color components.
	 */
	void setRGB(float r, float g, float b);

	/**
	 * Sets all color components from string.
	 * @param color - string containing color hex code.
	 */
	void setRGBA(const string& color);

	/**
	 * Sets all color components.
	 * @param r, g, b, a - color components.
	 */
	void setRGBA(float r, float g, float b, float a);

	/**
	 * Sets all color components.
	 * @param color - color object.
	 */
	void setRGBA(const Color& col);

	/**
	 * @return red color component.
	 */
	float getR();

	/**
	 * @return green color component.
	 */
	float getG();

	/**
	 * @return blue color component.
	 */
	float getB();

	/**
	 * @return alpha color component.
	 */
	float getA();

	/**
	 * Returns pointer to float array with color components.
	 * @return Array of color components.
	 */
	float* toArray();

	/**
	 * Prints color values.
	 * @param space - space
	 * before text.
	 */
	void print(std::string space = "");
private:
	float color_[4];
};

#endif /* COLOR_H_ */
