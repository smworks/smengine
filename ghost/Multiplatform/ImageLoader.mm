#include "ImageLoader.h"
#include "ServiceLocator.h"

#ifdef __MACH__
#include <CoreData/CoreData.h>
#include <ApplicationServices/ApplicationServices.h>
#include <CoreFoundation/CoreFoundation.h>
    
ImageLoader::RawImage ImageLoader::loadPng(ServiceLocator* services, const char* path, bool upperLeft) {
        ASSERT(path != 0, "No image file path specified. Unable to load image.");
        RawImage png;
//        SIZE size;s
//        INT8* bytes = 0;
//        services->getFileManager()->loadRaw(bytes, size, path);
//        RawImage png = pngToRaw(bytes, upperLeft);
//        delete[] bytes;
//        ASSERT(png.buffer != 0, "Unable to convert PNG to raw format for path: \"%s\".", path);
//        LOGD("Loaded PNG: %s.", path);
//        return png;
        CGDataProviderRef source = 0;
        source = CGDataProviderCreateWithFilename(path);
        if (source == 0) {
            LOGW("Image not found under path: \"%s\"", path);
            return png;
        }
        CGImageRef image = 0;
        image = CGImageCreateWithPNGDataProvider(source, NULL, YES, kCGRenderingIntentDefault);
        //ggimage->components = (int)(CGImageGetBitsPerPixel(image) / 8);
        source = CGImageGetDataProvider(image);
        CFDataRef data = CGDataProviderCopyData(source);
        png.width = (UINT32) CGImageGetWidth(image);
        png.height = (UINT32) CGImageGetHeight(image);
        png.buffer = NEW UINT8[CFDataGetLength(data)];
        UINT8* imageData = (UINT8*) CFDataGetBytePtr(data);
        memcpy(png.buffer, imageData, CFDataGetLength(data));
        long rowSize = CFDataGetLength(data) / png.height;
        for (UINT32 i = 0; i < png.height; i++) {
            long offset = rowSize * i;
            long sourceOffset = rowSize * (png.height - 1 - i);
            memcpy(png.buffer + offset, imageData + sourceOffset, rowSize);
        }
        CGDataProviderRelease(source);
        //CGImageRelease(image);
        CFRelease(data);
        return png;
    }
#endif
