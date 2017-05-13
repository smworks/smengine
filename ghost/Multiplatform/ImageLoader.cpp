#include "ImageLoader.h"
#include "ServiceLocator.h"

#ifdef __ANDROID__
ImageLoader::RawImage ImageLoader::loadPng(ServiceLocator* services, const char* path, bool upperLeft) {
    //services->getFileManager()->
    ASSERT(path != 0, "No image file path specified. Unable to load image.");
    RawImage png;
    return png;
}
#endif