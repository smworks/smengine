/*
 * Utils.cpp
 *
 *  Created on: 2012.06.25
 *      Author: MS
 */

#include "Utils.h"
#include "Multiplatform/ServiceLocator.h"
#include "Multiplatform/FileManager.h"
#include "../dependencies/includes/libpng/png.h"

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

float toFloat(const char* str, SIZE length) {
	float value;
	char* temp = NEW char[length + 1];
	memcpy(temp, str, length);
	temp[length] = '\0';
    if(EOF == sscanf(str, "%f", &value)) {
		delete [] temp;
        return 0.0f;
    }
	delete [] temp;
    return value;
}

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

void stringSplit(const string& str, vector<string>& parts, char delimiter) {
	parts.clear();
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

struct PngBuffer {
	INT8* buffer;
	SIZE bytesRead;
};

void readPng(png_structp png, png_bytep data, png_size_t size) {
	png_voidp a = png_get_io_ptr(png);
	PngBuffer* pngBuffer = (PngBuffer*) a;
	memcpy(data, pngBuffer->buffer + pngBuffer->bytesRead, size);
	pngBuffer->bytesRead += size;
}

#define HEADER_SIZE 8
INT8* pngToRaw(INT8* in, UINT32& width, UINT32& height, bool upperLeft) {
	if (in == 0) {
		LOGW("No PNG byte buffer specified.");
		return 0;
	}
	// Read header.
	png_byte header[HEADER_SIZE];
	memcpy(header, in, HEADER_SIZE);
	// Test if header is for PNG.
	if (png_sig_cmp(header, 0, HEADER_SIZE) != 0) {
		LOGE("Specified PNG byte buffer does not contain valid PNG header.");
		return 0;
	}
	// Create PNG structure.
	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_ptr) {
		LOGE("Unable to create PNG structure.");
		return 0;
	}
	// Create png info struct.
	png_infop info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr) {
		png_destroy_read_struct(&png_ptr, (png_infopp) NULL, (png_infopp) NULL);
		LOGE("Unable to create PNG info.");
		return 0;
	}
	// Create png info struct.
	png_infop end_info = png_create_info_struct(png_ptr);
	if (!end_info) {
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);
		LOGE("Unable to create PNG end info");
		return 0;
	}
	// PNG error stuff, not sure libpng man suggests this.
	if (setjmp(png_jmpbuf(png_ptr))) {
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
		LOGE("Error during setjmp.");
		return 0;
	}
	// Initialize PNG data reader.
	PngBuffer pngBuffer;
	pngBuffer.buffer = in;
	pngBuffer.bytesRead = HEADER_SIZE;
	png_set_read_fn(png_ptr, (png_voidp) &pngBuffer, readPng);
	// Let libpng know you already read the first 8 bytes.
	png_set_sig_bytes(png_ptr, HEADER_SIZE);
	// Read all the info up to the image data.
	png_read_info(png_ptr, info_ptr);
	// Variables to pass to get info.
	int bit_depth, color_type;
	png_uint_32 pngW, pngH;
	// Get info about PNG.
	png_get_IHDR(png_ptr, info_ptr, &pngW, &pngH, &bit_depth, &color_type,
	NULL, NULL, NULL);
	width = pngW;
	height = pngH;
	// Update the png info struct.
	png_read_update_info(png_ptr, info_ptr);
	// Row size in bytes.
	SIZE rowbytes = png_get_rowbytes(png_ptr, info_ptr);
	// Allocate the image_data as a big block, to be given to opengl.
	SIZE size = rowbytes * height;
	png_byte *image_data = NEW png_byte[size];
	if (!image_data) {
		// Clean up memory and close stuff.
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
		LOGE("Unable to allocate image_data.");
		return 0;
	}
	// Row_pointers is for pointing to image_data for reading the png with libpng.
	png_bytep *row_pointers = NEW png_bytep[height];
	if (!row_pointers) {
		// Clean up memory and close stuff.
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
		delete[] image_data;
		LOGE("Unable to allocate row_pointer");
		return 0;
	}
	// Set the individual row_pointers to point at the correct offsets of image_data.
	if (upperLeft) {
		for (UINT32 i = 0; i < height; ++i) {
			row_pointers[i] = image_data + i * rowbytes;
		}
	}
	else {
		for (UINT32 i = 0; i < height; ++i) {
			row_pointers[height - 1 - i] = image_data + i * rowbytes;
		}
	}
	// Read the png into image_data through row_pointers.
	png_read_image(png_ptr, row_pointers);
	INT8* buffer = NEW INT8[size];
	memcpy(buffer, image_data, size);
	// Clean up memory and close stuff
	png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
	delete[] image_data;
	delete[] row_pointers;
	return buffer;
}

INT8* loadPng(ServiceLocator* services, const char* path, UINT32& width, UINT32& height, bool upperLeft) {
	if (path == 0) {
		LOGW("No image file path specified. Unable to load image.");
		return 0;
	}
	INT8* bytes = 0;
	SIZE size;
	services->getFileManager()->loadRaw(bytes, size, path);
	INT8* buffer = pngToRaw(bytes, width, height, upperLeft);
	delete[] bytes;
	if (buffer == 0) {
		LOGW("Unable to convert PNG to raw format for path: \"%s\".", path);
		return 0;
	}
	LOGD("Loaded PNG: %s.", path);
	return buffer;
}
