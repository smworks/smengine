/*
 * Node.cpp
 *
 *  Created on: 2012.06.20
 *      Author: MS
 */

#include "Node.h"
#include "ScriptManager.h"
#include "Resources/Resource.h"

Node::Node() :
		parent_(0),
		states_(TRANSFORMABLE | RENDERABLE |
			UPDATABLE | POSITION | ROTATION | SCALING)
{
	Matrix::identity(matCombined_);
	Matrix::identity(matPos_);
	Matrix::identity(matRot_);
	Matrix::identity(matScale_);
	scale_.setXYZ(1.0f);
}

Node::~Node() {
	parent_ = 0;
	if (getChildren().size() > 0) {
		vector<Node*>::const_iterator it = children_.begin();
		while (it != children_.end()) {
			delete (*it);
			++it;
		}
	}
}

void Node::setName(const string& name) {
	if (name.length() == 0) {
		LOGW("Node name not specified while setting new one.");
		return;
	}
	name_ = name;
}

const string& Node::getName() {
	return name_;
}

void Node::setState(States state, bool value) {
	if (value) {
		states_ |= state;
	}
	else {
		states_ &= ~state;
	}
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
	children_.push_back(node);
}

void Node::removeChild(Node* node) {
	static vector<Node*>::iterator it;
	it = children_.begin();
	while (it != children_.end()) {
		if ((*it) == node) {
			children_.erase(it);
			delete node;
			return;
		}
		it++;
	}
}

Node* Node::getChild(const string& name) {
	vector<Node*>::const_iterator it = children_.begin();
	while (it != children_.end()) {
		if (name == (*it)->getName()) {
			return (*it);
		}
		++it;
	}
	return 0;
}

UINT32 Node::getChildCount() {
	int count = 0;
	vector<Node*>::const_iterator it = children_.begin();
	while (it != children_.end()) {
		count += (*it)->getChildCount();
		++it;
	}
	return ++count;
}

Node* Node::search(const string& name) {
	if (name_ == name) {
		return this;
	}
	Node* node = 0;
	vector<Node*>::const_iterator it = children_.begin();
	while (it != children_.end()) {
		node = (*it)->search(name);
		if (node != 0) {
			return node;
		}
		++it;
	}
	return 0;
}

const vector<Node*>& Node::getChildren() {
	return children_;
}

void Node::toRenderArray(vector<Node*>& arr) {
	for (vector<Resource*>::const_iterator it = resources_.begin();
		it != resources_.end(); it++)
	{
		if ((*it)->isRenderable()) {
			arr.push_back(this);
			break;
		}
	}
	for (vector<Node*>::const_iterator it = children_.begin();
		it != children_.end(); it++) {
		(*it)->toRenderArray(arr);
	}
}

void Node::toGUIArray(vector<Node*>& arr) {
	for (vector<Resource*>::const_iterator it = resources_.begin();
		it != resources_.end(); it++)
	{
		if ((*it)->isGUIElement()) {
			arr.push_back(this);
			break;
		}
	}
	for (vector<Node*>::const_iterator it = children_.begin();
		it != children_.end(); it++) {
		(*it)->toGUIArray(arr);
	}
}

void Node::addResource(Resource* resource) {
	resources_.push_back(resource);
}

Resource* Node::getResource(int type) {
	for (UINT32 i = 0; i < resources_.size(); i++) {
		if (resources_[i]->getType() == type) {
			return resources_[i];
		}
	}
	return 0;
}

SIZE Node::getResourceCount(int type) {
	return getResources(type).size();
}

SIZE Node::getResourceCount() {
	return resources_.size();
}

vector<Resource*> Node::getResources(int type) {
	if (type == -1) {
		return resources_;
	}
	vector<Resource*> resources;
	for (UINT32 i = 0; i < resources_.size(); i++) {
		if (resources_[i]->getType() == type) {
			resources.push_back(resources_[i]);
		}
	}
	return resources;
}

bool Node::hasResource(int type) {
	return getResource(type) == 0 ? false : true;
}

void Node::print(string space) {
	LOGD("%sNode name: %s.", space.c_str(), name_.c_str());
	for (UINT32 i = 0; i < resources_.size(); i++) {
		LOGD("%sResource(%d): %s",
			space.c_str(),
			resources_[i]->getType(),
			resources_[i]->getName().c_str());
	}
}

void Node::printNodes(const string& space) {
	print(space);
	if (children_.size() != 0) {
		for (vector<Node*>::const_iterator it = children_.begin();
				it != children_.end(); ++it) {
			(*it)->printNodes(space + "  ");
		}
	}
}