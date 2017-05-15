#include "ImageLoader.h"
#include "ServiceLocator.h"
#include "FileManager.h"

#ifndef __MACH__
#include "../../dependencies/includes/libpng/png.h"

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

ImageLoader::RawImage ImageLoader::loadPng(ServiceLocator* services, const char* path, bool upperLeft) {
    RawImage png;
    int res;
    SIZE imageSize;
    INT8* in = 0;
    services->getFileManager()->loadRaw(in, imageSize, path);
    ASSERT(in != 0, "No PNG byte buffer specified.");
    png_byte header[HEADER_SIZE];
    memcpy(header, in, HEADER_SIZE);
    res = png_sig_cmp(header, 0, HEADER_SIZE);
    ASSERT(res == 0, "Specified PNG byte buffer does not contain valid PNG header.");
    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    ASSERT(png_ptr, "Unable to create PNG structure.");
    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_read_struct(&png_ptr, (png_infopp) NULL, (png_infopp) NULL);
        LOGE("Unable to create PNG info.");
        return png;
    }
    png_infop end_info = png_create_info_struct(png_ptr);
    if (!end_info) {
        png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);
        LOGE("Unable to create PNG end info");
        return png;
    }
    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        LOGE("Error during setjmp.");
        return png;
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
    int bit_depth, colorType;
    png_uint_32 pngW, pngH;
    // Get info about PNG.
    png_get_IHDR(png_ptr, info_ptr, &pngW, &pngH, &bit_depth, &colorType,
                 NULL, NULL, NULL);
    // Log image info.
    string col = "Unknown";

    if (colorType == PNG_COLOR_TYPE_GRAY) {
        col = "PNG_COLOR_TYPE_GRAY";
    } else if (colorType == PNG_COLOR_TYPE_PALETTE) {
        col = "PNG_COLOR_TYPE_PALETTE";
    } else if (colorType == PNG_COLOR_TYPE_RGB) {
        col = "PNG_COLOR_TYPE_RGB";
    } else if (colorType == PNG_COLOR_TYPE_RGB_ALPHA) {
        col = "PNG_COLOR_TYPE_RGB_ALPHA";
        png.alpha = true;
    } else if (colorType == PNG_COLOR_TYPE_GRAY_ALPHA) {
        col = "PNG_COLOR_TYPE_GRAY_ALPHA";
        png.alpha = true;
    }

    if (bit_depth == 16) {
        LOGD("Image channel size is 16 bit, so striping it to 8.");
        png_set_strip_16(png_ptr);
    }
    if (colorType == PNG_COLOR_TYPE_PALETTE) {
        LOGD("Converting png from 8 bit to 24.");
        png.alpha = false;
        png_set_palette_to_rgb(png_ptr);
    }
    if (colorType == PNG_COLOR_TYPE_GRAY || colorType == PNG_COLOR_TYPE_GRAY_ALPHA) {
        LOGD("Converting png from grey to rgb.");
        png_set_gray_to_rgb(png_ptr);
    }
    if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) {
        LOGD("Adding alpha channel to RGB image.");
        png.alpha = true;
        png_set_tRNS_to_alpha(png_ptr);
    }
    LOGD("Image. Width=%d, height=%d, bit depth=%d, color type=%s", pngW, pngH, bit_depth, col.c_str());
    png.width = pngW;
    png.height = pngH;
    // Update the png info struct.
    png_read_update_info(png_ptr, info_ptr);
    // Row size in bytes.
    SIZE rowbytes = png_get_rowbytes(png_ptr, info_ptr);
    // Allocate the image_data as a big block, to be given to opengl.
    SIZE size = rowbytes * png.height;
    png_byte *image_data = NEW png_byte[size];
    // Row_pointers is for pointing to image_data for reading the png with libpng.
    png_bytep *row_pointers = NEW png_bytep[png.height];
    // Set the individual row_pointers to point at the correct offsets of image_data.
    if (upperLeft) {
        for (UINT32 i = 0; i < png.height; ++i) {
            row_pointers[i] = image_data + i * rowbytes;
        }
    } else {
        for (UINT32 i = 0; i < png.height; ++i) {
            row_pointers[png.height - 1 - i] = image_data + i * rowbytes;
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
    png.buffer = reinterpret_cast<UINT8*>(buffer);
    return png;
}
#endif