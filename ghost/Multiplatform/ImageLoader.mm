#include "ImageLoader.h"
#include "ServiceLocator.h"

#ifdef __MACH__
#include <CoreData/CoreData.h>
#include <ApplicationServices/ApplicationServices.h>
#include <CoreFoundation/CoreFoundation.h>
    
ImageLoader::RawImage ImageLoader::loadPng(ServiceLocator* services, const char* path, bool upperLeft) {
    ASSERT(path != 0, "No image file path specified. Unable to load image.");
    RawImage png;
    CGDataProviderRef source = 0;
    source = CGDataProviderCreateWithFilename(path);
    if (source == 0) {
        LOGW("Image not found under path: \"%s\"", path);
        return png;
    }
    CGImageRef image = CGImageCreateWithPNGDataProvider(source, NULL, YES, kCGRenderingIntentDefault);
    UINT32 bytesPerPixel = (UINT32) CGImageGetBitsPerPixel(image) / 8;
    LOGD("Image has %u bytes per pixel", bytesPerPixel);
    LOGD("Image has %zu bits per component",
         CGImageGetBitsPerComponent(image));
    LOGD("Image has %zu bytes per row",
         CGImageGetBytesPerRow(image));
    bool alpha = CGImageGetAlphaInfo(image);
    png.alpha = alpha;
    LOGD("Image has alfa: %d", alpha ? 1 : 0);
    source = CGImageGetDataProvider(image);
    CFDataRef data = CGDataProviderCopyData(source);
    UINT8* imageData = (UINT8*) CFDataGetBytePtr(data);
    png.width = (UINT32) CGImageGetWidth(image);
    png.height = (UINT32) CGImageGetHeight(image);
    long rowSize = CFDataGetLength(data) / png.height;
    LOGD("Row size is %ld", rowSize);
    if (bytesPerPixel == 4 || bytesPerPixel == 3) {
        png.buffer = NEW UINT8[CFDataGetLength(data)];
        for (UINT32 i = 0; i < png.height; i++) {
            long offset = rowSize * i;
            long sourceOffset = rowSize * (png.height - 1 - i);
            memcpy(png.buffer + offset,
                   imageData + sourceOffset, rowSize);
        }
    } else {
        LOGW("Unsuported bytes per pixel: %u", bytesPerPixel);
    }

    CGDataProviderRelease(source);
    //CGImageRelease(image);
    CFRelease(data);
    return png;
}
#endif
