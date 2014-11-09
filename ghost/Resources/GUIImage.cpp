/*
 * GUIImage.cpp
 *
 *  Created on: 2013.07.07
 *      Author: Martynas Ðustavièius
 */

#include "GUIImage.h"
#include "Vertex.h"
#include "TextureRGBA.h"
#include "../Shapes.h"
#include "../ResourceManager.h"
#include "../Multiplatform/ServiceLocator.h"
#include "../Node.h"

const string GUIImage::TAG_SRC = "src";

GUIImage::GUIImage(ServiceLocator* services) :
	GUISurface(services),
	src_(0)
{}

GUIImage::~GUIImage() {}

void GUIImage::hasFocus() {

}

void GUIImage::update() {
//	string name = getNode()->getName() + "_src";
//    if (force && src_ != 0) {
//        getServiceLocator()->getRM()->remove(
//			Resource::TEXTURE_2D, name);
//        src_ = 0;
//    }
//    if (src_ == 0 && getAttribute(TAG_SRC).length() > 0) {
//		vector<ResourceAttribute> attributes;
//		attributes.push_back(ResourceAttribute(ATTR_FILE, getAttribute(TAG_SRC)));
//        src_ = static_cast<TextureRGBA*>(
//			getServiceLocator()->getRM()->get(
//				Resource::TEXTURE_2D, name, attributes));
//		src_->resize(surface_->getWidth(), surface_->getHeight());
//        surface_->blend(src_, false);
//        surface_->commit();
//    }
}
