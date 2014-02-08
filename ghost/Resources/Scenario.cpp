/*
 * Scenario.cpp
 *
 *  Created on: 2014 01 28
 *      Author: Martynas Šustavičius
 */

#include "Scenario.h"
#include "../Multiplatform/ServiceLocator.h"
#include "../Node.h"

class ActionMove : public Scenario::Action {
public:
	ActionMove(Node* node, Vec3 start, Vec3 end, float time) :
		Action(time), node_(node), start_(start), end_(end) {}
	void run(float progress) {
		float dX = end_.getX() - start_.getX();
		float dY = end_.getY() - start_.getY();
		float dZ = end_.getZ() - start_.getZ();
		node_->getPos().setXYZ(
			start_.getX() + dX * progress,
			start_.getY() + dY * progress,
			start_.getZ() + dZ * progress);
		node_->setState(Node::POSITION, true);
	}
	Action* clone() const { return new ActionMove(*this); }
private:
	Node* node_;
	Vec3 start_;
	Vec3 end_;
};

class ActionWait : public Scenario::Action {
public:
	ActionWait(float time) : Action(time)  {}
	void run(float progress) {
	}
	Action* clone() const { return new ActionWait(*this); }
};

Scenario::Scenario(ServiceLocator* services) :
	Resource(services), progress_(0.0f), index_(0)
{}

Scenario::Scenario(const Scenario& right) : Resource(right) {
	SIZE size = right.actions_.size();
	for (SIZE i = 0; i < size; i++) {
		actions_.push_back(right.actions_[i]->clone());
	}
	progress_ = right.progress_;
	index_ = right.index_;
}

Scenario::~Scenario() {
	release();
}

bool Scenario::create() {
	return true;
}

void Scenario::release() {
	vector<Action*>::iterator it = actions_.begin();
	while (it != actions_.end()) {
		delete (*it);
		it++;
	}
	actions_.clear();
}

SIZE Scenario::getSize() {
	return sizeof(Scenario);
}

Resource::Type Scenario::getType() {
	return Resource::SCENARIO;
}

bool Scenario::isValid() {
	return true;
}

void Scenario::move(Node* node, Vec3 start, Vec3 end, float time) {
	actions_.push_back(new ActionMove(node, start, end, time));
}

void Scenario::wait(float time) {
	actions_.push_back(new ActionWait(time));
}

vector<Scenario::Action*>& Scenario::getActions() {
	return actions_;
}

void Scenario::setProgress(float progress) {
	progress_ = progress;
}

float Scenario::getProgress() {
	return progress_;
}

void Scenario::setIndex(SIZE index) {
	index_ = index;
}

SIZE Scenario::getIndex() {
	return index_;
}