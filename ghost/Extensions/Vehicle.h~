/*
 * Vehicle.h
 *
 *  Created on: 2013.05.09
 *      Author: Martynas Ðustavièius
 */

#ifndef VEHICLE_H_
#define VEHICLE_H_

#include "../Multiplatform/Ghost.h"
class Resource;
class Node;
class btDynamicsWorld;
class btRaycastVehicle;
class btCompoundShape;
class btDefaultVehicleRaycaster;
class btConvexHullShape;
class btMotionState;
class btRigidBody;
struct lua_State;

class Vehicle {
public:
	static const string MASS;
	static const string MAX_ENGINE_FORCE;
	static const string MAX_BREAK_FORCE;
	static const string STEERING_INCREMENT;
	static const string STEERING_CLAMP;
	static const string WHEEL_RADIUS;
	static const string WHEEL_WIDTH;
	static const string WHEEL_FRICTION;
	static const string MAX_WHEEL_ROTATION;
	static const string SUSPENSION_REST_LENGTH;
	static const string SUSPENSION_STIFFNESS;
	static const string SUSPENSION_DAMPING;
	static const string SUSPENSION_COMPRESSION;
	static const string ROLL_INFLUENCE;
public:
	Vehicle(Node* container, btDynamicsWorld* dynamicsWorld, vector<btRigidBody*>* rigidBodies);
	~Vehicle();
	void update();
	vector<Node*>& getWheels();
	void setRaycastVehicle(btRaycastVehicle* vehicle);
	Node* getContainer();
	btRaycastVehicle* getRaycastVehicle();
	float getMass();
	float getSuspensionRestLength();
	float getSuspensionStiffness();
	float getSuspensionDamping();
	float getSuspensionCompression();
	float getWheelTurn();
	float getWheelRadius();
	float getWheelFriction();
	float getRollInfluence();
private:
	btMotionState* motionState_;
	btRigidBody* rigidBody_;
    Node* container_;
	btRaycastVehicle* vehicle_;
	btCompoundShape* vehicleCompoundShape_;
	btDefaultVehicleRaycaster* vehicleRayCaster_;
	btConvexHullShape* vehicleBodyShape_;
	vector<Node*> wheels_;
	float mass_;
	float suspensionRestLength_;
	float suspensionStiffness_;
	float suspensionDamping_;
	float suspensionCompression_;
	float wheelTurn_;
	float wheelRadius_;
	float wheelFriction_;
	float rollInfluence_;
};

#endif
