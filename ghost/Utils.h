/*
 * Utils.h
 *
 *  Created on: 2012.06.25
 *      Author: MS
 */

#ifndef UTILS_H_
#define UTILS_H_

#include "Multiplatform/Ghost.h"

#define DEG_TO_RAD(val) val * 0.0174532925f
#define RAD_TO_DEG(val) val * 57.2957795f

static const string STRING_TRUE = "true";
static const string STRING_FALSE = "false";

/** PI value in degrees. */
static const float PI = 180.0f;

/**
 * This is a very fast sine method implementation with
 * reduced floating point precision.
 * @param degrees - float representing degrees.
 * @return Sine of given amount of degrees.
 */
float sinFast(float degrees);

/**
 * This is a very precise sine method implementation with
 * increased calculation time.
 * @param degrees - float representing degrees.
 * @return Sine of given amount of degrees.
 */
float sinPrec(float degrees);

/**
 * This is a very fast cosine method implementation with
 * reduced floating point precision.
 * @param degrees - float representing degrees.
 * @return Cosine of given amount of degrees.
 */
float cosFast(float degrees);

/**
 * This is a very precise cosine method implementation with
 * increased calculation time.
 * @param degrees - float representing degrees.
 * @return Cosine of given amount of degrees.
 */
float cosPrec(float degrees);

/**
 * This is a very fast tangent method implementation with
 * reduced floating point precision.
 * @param degrees - float representing degrees.
 * @return Tangent of given amount of degrees.
 */
float tanFast(float degrees);

/**
 * This is a very precise tangent method implementation with
 * increased calculation time.
 * @param degrees - float representing degrees.
 * @return Tangent of given amount of degrees.
 */
float tanPrec(float degrees);

/**
 * Converts c style string into float.
 * @param str - c styel string containing float.\
 * @return Float representation of string.
 */
float toFloat(const char* str);

///**
// * Converts c style string into float.
// * @param str - c styel string containing float.
// * @param length - length of the string.
// * @return Float representation of string.
// */
//float toFloat(const char* str, SIZE length);

/**
 * Converts c style string into integer.
 * @param str - c style string containing integer value.
 * @return Integer representation of string.
 */
int toInt(const char* str);

/**
 * Converts c style string into unsigned integer.
 * @param str - c style string containing unsigned integer.
 * @return Unsigned integer representation of string.
 */
UINT32 toUint(const char* str);

/**
 * Converts c style string into unsigned integer.
 * @param str - c style string containing unsigned integer.
 * @param length - length of the string.
 * @return Unsigned integer representation of string.
 */
UINT32 toUint(const char* str, SIZE length);

/**
 * Converts string into boolean.
 * @param str - string that contains boolean value.
 * @return Boolean value contained in string.
 */
bool toBool(const string& str);

/**
 * Converts specified value to string.
 * @param value - value to be converted.
 * @return String with specified value.
 */
string toString(int value);

/**
 * Checks if source string contains desired string.
 * @param str1 - source string.
 * @param str2 - search string.
 * @return True if match string was found in source string.
 */
bool stringContains(const string& str1, const string& str2);

/**
 * Splits specified string by specified delimiter into separate strings.
 * @param str - specified string.
 * @param delimiter - specified delimiter.
 * @return vector that will contain splited string.
 */
vector<string> stringSplit(const string& str, char delimiter);

/**
 * Removes leading and trailing empty spaces
 * @param str - original string
 * @return trimmed string
 */
string trim(string str);

/**
 * Iterates through string searching for "search" string and replacing
 * it with "replace" string.
 * @param src - source string.
 * @param search - string that will be searched and be replaced.
 * @param replace - replacement string.
 * @return String with replaced content.
 */
void stringReplace(string& src, string search, string replace);

/**
 * Increments or decrements a value so it would eventually
 * reach value near zero.
 * @param value - current value.
 * @param delta - increment or decrement step value.
 */
void floatToZero(float& value, float delta);

/**
 * Adjust float in bounds between -180 and 180 degrees.
 * @param x - value in degrees.
 * @return Adjusted value.
 */
float floatTo360Bounds(float x);

/**
 * @param number - number to be checked.
 * @return True if number is power of two.
 */
bool isPowerOfTwo(UINT32 number);

/**
 * Changes specified number to nearest power of
 * two number.
 * @param number - number that should be
 * changed to nearest number that is power
 * of two.
 * @return Number that is nearest power of two.
 */
UINT32 toPowerOfTwo(UINT32 number);

/**
 * Converts rgba8888 color buffer into rgba4444.
 * @param in - pointer to original rgba8888 color buffer.
 * @param size - size of the original buffer in bytes.
 * @return Pointer to compressed image buffer.
 */
void* rgba8888ToRgba4444(void* in, int size);

#endif
