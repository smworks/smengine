/*
 * Console.cpp
 *
 *  Created on: 2012.10.15
 *      Author: MS
 */

#include "Console.h"
#include "Multiplatform/ServiceLocator.h"
#include "Resources/GUIImage.h"
#include "Resources/GUIText.h"
#include "Node.h"
#include "ResourceManager.h"

Console::Console(ServiceLocator* services) {
	//Node* node = new Node();
	//node->setName("Console");
	//GUIText* image = new GUIText(services);
	//image->setNode(node);
	//image->setWidth(256.0f);
	//image->setHeight(128.0f);
	//image->setBackground("1.png");
	//image->setText("YOLO");
	//image->create();
	//node->addResource(image);
	//services->getRM()->add("Console", image);
	//node->setParent(services->getRootNode());
	//services->getRootNode()->addChild(node);

	//    Node* debugNode_ = NEW Node;
 //       debugNode_->setName("random");
 //       GUIText* debugText_ = NEW GUIText(services);
 //       debugText_->setNode(debugNode_);
 //       debugText_->getAttributes().setString(GUIText::ATTR_WIDTH, "512px");
 //       debugText_->getAttributes().setString(GUIText::ATTR_HEIGHT, "32px");
 //       debugText_->getAttributes().setString(GUIText::ATTR_TEXT, "RANDOM TEXT");
 //       debugText_->getAttributes().setString(GUIText::ATTR_COLOR, "#FF00FFFF");
 //       debugText_->getAttributes().setString(GUIText::ATTR_BACKGROUND, "#00000088");
 //       debugText_->getAttributes().setString(GUIText::ATTR_SIZE, "12px");
 //       debugText_->getAttributes().setString(GUIText::ATTR_SCREEN_LEFT, "true");
 //       debugText_->getAttributes().setString(GUIText::ATTR_SCREEN_TOP, "true");
 //       debugText_->create();
 //       services->getRM()->add("debug text 2", debugText_);
 //       debugNode_->addResource(debugText_);
 //       debugNode_->setState(Node::RENDERABLE, true);
 //       services->getRootNode()->addChild(debugNode_);
}

Console::~Console() {
	// TODO Auto-generated destructor stub
}

