/*
 * Utils.cpp
 *
 *  Created on: 2012.06.25
 *      Author: MS
 */

#include "Utils.h"

float sinFast(float degrees) {
	// Wrap input angle in bounds from -PI to PI.
	while (degrees < -PI) degrees += 360.0f;
	while (degrees > PI) degrees -= 360.0f;
	// Compute sine.
	float sin =
		1.2732395447351626861510701069801f // From (4 / Pi) in RADIANS to DEGREES.
		* degrees
		+ -0.00707355302630645936750594503878f // From (-4 / (Pi * Pi)) in RADIANS to DEGREES.
		* degrees
		* ((degrees > 0.000000000000000000001f) ? degrees : -degrees);
	// Multiply by (1 / (180 / Pi)) to wrap result to bounds from -1.0f to 1.0f.
	sin *= 0.01745329251994329576923690768489f;
	// Return result.
	return sin;
}

float sinPrec(float degrees) {
	// Lower precision sine.
	float sin = sinFast(degrees);
	// Extra precision.
	sin = 0.225f * (sin * abs(sin) - sin) + sin;
	// Return result.
	return sin;
}

float cosFast(float degrees) {
	// Cosine is the same as sine just with additional offset of (Pi/2).
	return sinFast(degrees + 90.0f);
}

float cosPrec(float degrees) {
	// Cosine is the same as sine just with additional offset of (Pi/2).
	return sinPrec(degrees + 90.0f);
}

float tanFast(float degrees) {
	return sinFast(degrees) / cosFast(degrees);
}

float tanPrec(float degrees) {
	return sinPrec(degrees) / cosPrec(degrees);
}

float toFloat(const char* str) {
	float value;
    if(EOF == sscanf(str, "%f", &value)) {
        return 0.0f;
    }
    return value;
}

//float toFloat(const char* str, SIZE length) {
//	float value;
//	char* temp = NEW char[length + 1];
//	memcpy(temp, str, length);
//	temp[length] = '\0';
//	LOGD("FLOAT: %s", temp);
//    if(EOF == sscanf(str, "%f", &value)) {
//		delete [] temp;
//        return 0.0f;
//    }
//	delete [] temp;
//    return value;
//}

int toInt(const char* str) {
	int value;
    if(EOF == sscanf(str, "%d", &value)) {
        return 0;
    }
    return value;
}

UINT32 toUint(const char* str) {
	UINT32 value;
    if(EOF == sscanf(str, "%u", &value)) {
        return 0;
    }
    return value;
}

UINT32 toUint(const char* str, SIZE length) {
	UINT32 value;
	char* temp = NEW char[length + 1];
	memcpy(temp, str, length);
	temp[length] = '\0';
    if(EOF == sscanf(temp, "%u", &value)) {
		delete [] temp;
        return 0;
	}
	delete [] temp;
    return value;
}

bool toBool(const string& str) {
	return str == STRING_TRUE;
}

string toString(int value) {
	static ostringstream oss;
	oss.str(string());
	oss << value;
	return oss.str();
}

bool stringContains(const string& str1, const string& str2) {
    if (string::npos != str1.find(str2)) {
        return true;
    }
    return false;
}

vector<string> stringSplit(const string& str, char delimiter) {
	vector<string> parts;
	size_t lastPos = 0, pos = 0, size = str.size();
	while (lastPos < size) {
		pos = str.find(delimiter, lastPos);
		if (pos == string::npos) {
			pos = size;
		}
		if (pos != lastPos) {
			parts.push_back(str.substr(lastPos, pos - lastPos));
		}
		lastPos = pos + 1;
	}
	return parts;
}

string trim(string str)
{
	//str.erase(str.begin(), find_if(str.begin(), str.end(), not1(ptr_fun<int, int>(isspace))));
	return str;
}

void stringReplace(string& src, string search, string replace) {
	SIZE start = 0;
	while ((start = src.find(search, start)) != string::npos) {
         src.replace(start, search.length(), replace);
         start += replace.length();
	}
}

void floatToZero(float& value, float delta) {
	if (value > 0.0f) {
		value -= delta;
		if (value < 0.0f) {
			value = 0.0f;
		}
	} else {
		value += delta;
		if (value > 0.0f) {
			value = 0.0f;
		}
	}
}

float floatTo360Bounds(float x) {
	while (x > 180.0f) {
		x -= 360.0f;
	}
	while (x < -180.0f) {
		x += 360.0f;
	}
	return x;
}

bool isPowerOfTwo(UINT32 number) {
    return ((number > 0) && ((number & (~number + 1)) == number));
}

UINT32 toPowerOfTwo(UINT32 number) {
    if (number <= 2) {
        return 2;
    }
    --number;
    number |= number >> 1;
    number |= number >> 2;
    number |= number >> 4;
    number |= number >> 8;
    number |= number >> 16;
    return number + 1;
}

void* rgba8888ToRgba4444(void* in, int size) {
    int pixelCount = size / 4;
    UINT32* input = static_cast<UINT32*>(in);
    UINT16* output = NEW UINT16[pixelCount];
    for (int i = 0; i < pixelCount; i++) {
        UINT32 pixel = input[i];
        // Unpack the source data as 8 bit values
        UINT32 r = pixel & 0xff;
        UINT32 g = (pixel >> 8) & 0xff;
        UINT32 b = (pixel >> 16) & 0xff;
        UINT32 a = (pixel >> 24) & 0xff;
        // Convert to 4 bit values
        r >>= 4; g >>= 4; b >>= 4; a >>= 4;
        output[i] = r << 12 | g << 8 | b << 4 | a;
    }
    return static_cast<void*>(output);
}