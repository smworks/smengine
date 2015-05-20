/*
 * Node.cpp
 *
 *  Created on: 2012.06.20
 *      Author: MS
 */

#include "Node.h"
#include "ScriptManager.h"
#include "Resources/Resource.h"

Node::Node(string name, Resource* resource) :
	name(name),
	resource(resource),
	parent_(0),
	states_(TRANSFORMABLE | RENDERABLE | UPDATABLE | POSITION | ROTATION | SCALING)
{
	ASSERT(name.length() != 0, "No name specified for node.");
	ASSERT(resource != 0, "No resource specified for node: %s", name.c_str());
	Matrix::identity(matCombined_);
	Matrix::identity(matPos_);
	Matrix::identity(matRot_);
	Matrix::identity(matScale_);
	scale_.setXYZ(1.0f);
}

Node::~Node() {
	parent_ = 0;
	if (getChildren().size() > 0) {
		vector<Node*>::const_iterator it = children.begin();
		while (it != children.end()) {
			delete (*it);
			++it;
		}
	}
}

const string& Node::getName() {
	return name;
}

Resource* Node::getResource() {
	return resource;
}

void Node::setState(States state, bool value) {
	value ? states_ |= state : states_ &= ~state;
}

bool Node::getState(States state) {
	return (states_ & state) != 0;
}

Vec3& Node::getPos() {
	return pos_;
}

void Node::getPosAbs(Vec3& pos) {
	if (parent_ != 0 && getState(TRANSFORMABLE)) {
		parent_->getPosAbs(pos);
	}
	pos.addXYZ(pos_);
}

Quaternion& Node::getRot() {
	return rot_;
}

Vec3& Node::getScale() {
	return scale_;
}

void Node::setMatrix(Mat4 mat) {
	memcpy(matCombined_, mat, sizeof(Mat4));
}

float* Node::getMatrix() {
	return matCombined_;
}

float* Node::getMatPos() {
	return matPos_;
}

float* Node::getMatRot() {
	return matRot_;
}

float* Node::getMatScale() {
	return matScale_;
}

void Node::setParent(Node* node) {
	parent_ = node;
}

Node* Node::getParent() {
	return parent_;
}

void Node::addChild(Node* node) {
	children.push_back(node);
}

void Node::removeChild(Node* node, bool deleteNode) {
	static vector<Node*>::iterator it;
	it = children.begin();
	while (it != children.end()) {
		if ((*it) == node) {
			children.erase(it);
			if (deleteNode) {
				delete node;
			}
			return;
		}
		it++;
	}
}

Node* Node::getChild(const string& name) {
	vector<Node*>::const_iterator it = children.begin();
	while (it != children.end()) {
		if (name == (*it)->getName()) {
			return (*it);
		}
		++it;
	}
	return 0;
}

UINT32 Node::getChildCount() {
	int count = 0;
	vector<Node*>::const_iterator it = children.begin();
	while (it != children.end()) {
		count += (*it)->getChildCount();
		++it;
	}
	return ++count;
}

Node* Node::search(const string& name) {
	if (name == name) {
		return this;
	}
	Node* node = 0;
	vector<Node*>::const_iterator it = children.begin();
	while (it != children.end()) {
		node = (*it)->search(name);
		if (node != 0) {
			return node;
		}
		++it;
	}
	return 0;
}

const vector<Node*>& Node::getChildren() {
	return children;
}

void Node::toRenderArray(vector<Node*>& arr) {
	if (resource->isRenderable()) {
		arr.push_back(this);
	}
	for (vector<Node*>::const_iterator it = children.begin();
		it != children.end(); it++) {
		(*it)->toRenderArray(arr);
	}
}

void Node::toGUIArray(vector<Node*>& arr) {
	if (resource->isGUIElement()) {
		arr.push_back(this);
	}
	for (vector<Node*>::const_iterator it = children.begin();
		it != children.end(); it++) {
		(*it)->toGUIArray(arr);
	}
}

void Node::print(string ident) {
	LOGD("%sNode name: %s, Resource: %s(%d)", ident.c_str(),
		name.c_str(), resource->getName().c_str(), resource->getType());
}

void Node::printNodes(string ident) {
	print(ident);
	vector<Node*>::const_iterator it = children.begin();
	for (; it != children.end(); ++it) {
		(*it)->printNodes(ident + ident);
	}
}