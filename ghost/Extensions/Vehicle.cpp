/*
 * Vehicle.cpp
 *
 *  Created on: 2013.05.09
 *      Author: Martynas Ðustavièius
 */

#include "Vehicle.h"
#include "../../dependencies/includes/bullet/BulletDynamics/Vehicle/btRaycastVehicle.h"
#include "../../dependencies/includes/bullet/BulletCollision/CollisionShapes/btCompoundShape.h"
#include "../../dependencies/includes/bullet/BulletCollision/CollisionShapes/btShapeHull.h"
#include "../../dependencies/includes/bullet/BulletCollision/CollisionShapes/btConvexHullShape.h"
#include "../../dependencies/includes/bullet/LinearMath/btDefaultMotionState.h"
#include "../../dependencies/includes/bullet/BulletDynamics/Dynamics/btDynamicsWorld.h"
#include "../Node.h"
#include "../Resources/Resource.h"
#include "../Resources/StaticObject.h"
#include "../ModelData.h"
#include "../BoundingVolume.h"
#include "../ScriptManager.h"
#include "../Utils.h"
#include "../Multiplatform/ServiceLocator.h"

const string Vehicle::MASS = "mass";
const string Vehicle::MAX_ENGINE_FORCE = "max_engine_force";
const string Vehicle::MAX_BREAK_FORCE = "max_break_force";
const string Vehicle::STEERING_INCREMENT = "steering_increment";
const string Vehicle::STEERING_CLAMP = "steering_clamp";
const string Vehicle::WHEEL_RADIUS = "wheel_radius";
const string Vehicle::WHEEL_WIDTH = "wheel_width";
const string Vehicle::WHEEL_FRICTION = "wheel_friction";
const string Vehicle::MAX_WHEEL_ROTATION = "max_wheel_rotation";
const string Vehicle::SUSPENSION_REST_LENGTH = "suspension_rest_length";
const string Vehicle::SUSPENSION_STIFFNESS = "suspension_stiffness";
const string Vehicle::SUSPENSION_DAMPING = "suspension_damping";
const string Vehicle::SUSPENSION_COMPRESSION ="suspension_compression";
const string Vehicle::ROLL_INFLUENCE = "roll_influence";

btVector3 wheelDirectionCS0(0, -1, 0);
btVector3 wheelAxleCS(-1, 0, 0);

Vehicle::Vehicle(Node* container, btDynamicsWorld* dynamicsWorld, vector<btRigidBody*>* rigidBodies) :
	motionState_(0),
	rigidBody_(0),
	container_(container),
	vehicle_(0),
	vehicleCompoundShape_(0),
	vehicleRayCaster_(0),
	vehicleBodyShape_(0),
	suspensionRestLength_(0.0f),
	suspensionStiffness_(0.0f),
	suspensionDamping_(0.0f),
	suspensionCompression_(0.0f),
	wheelTurn_(0.0f),
	wheelRadius_(0.0f),
	wheelFriction_(0.0f),
	rollInfluence_(0.0f)
{
	// Read vehicle attributes.
	if (!container->hasResource(Resource::STATIC_OBJECT)) {
		LOGW("Vehicle does not contain model.");
		return;
	}
	StaticObject* model = static_cast<StaticObject*>(container->getResource(Resource::STATIC_OBJECT));
	mass_ = toFloat(model->getAttribute(MASS).c_str());
	suspensionRestLength_ = toFloat(model->getAttribute(SUSPENSION_REST_LENGTH).c_str());
	suspensionStiffness_ = toFloat(model->getAttribute(SUSPENSION_STIFFNESS).c_str());
	suspensionDamping_ = toFloat(model->getAttribute(SUSPENSION_DAMPING).c_str());
	suspensionCompression_ = toFloat(model->getAttribute(SUSPENSION_COMPRESSION).c_str());
	wheelTurn_ = toFloat(model->getAttribute(Vehicle::MAX_WHEEL_ROTATION).c_str());
	wheelRadius_ = toFloat(model->getAttribute(WHEEL_RADIUS).c_str());
	wheelFriction_ = toFloat(model->getAttribute(WHEEL_FRICTION).c_str());
	rollInfluence_ = toFloat(model->getAttribute(ROLL_INFLUENCE).c_str());
	// Create compound shape to contain vehicle body shape.
	vehicleCompoundShape_ = new btCompoundShape();
	ModelData* modelData = model->getModel();
	SIZE vertexCount = modelData->getVertexCount();
	UINT32 floatStride = modelData->getVertexStride() / sizeof(float);
	float* data = reinterpret_cast<float*>(modelData->getVertices());
	vector<float> vert;
	for (UINT32 i = 0; i < vertexCount; i++) {
		vert.push_back(data[i * floatStride + 0] * container->getScale().getX());
		vert.push_back(data[i * floatStride + 1] * container->getScale().getY());
		vert.push_back(data[i * floatStride + 2] * container->getScale().getZ());
	}
	vector<UINT32> indices;
	if (modelData->getIndexCount() > 0 && modelData->getIndexType() == Renderable::INDEX_TYPE_USHORT) {
		UINT16* ind = modelData->getIndicesShort();
		for (UINT32 i = 0; i < modelData->getIndexCount(); i++) {
			indices.push_back(ind[i]);
		}
	}
	else if (model->getIndexCount() > 0) {
		UINT32* ind = modelData->getIndicesInt();
		for (UINT32 i = 0; i < modelData->getIndexCount(); i++) {
			indices.push_back(ind[i]);
		}
	}
	btConvexHullShape* originalConvexShape = new btConvexHullShape;
	SIZE size = vert.size();
	if (model->getIndexCount() == 0) {
		for (SIZE i = 0; i < size; i += 3) {
			originalConvexShape->addPoint(
				btVector3(
					vert[i + 0],
					vert[i + 1],
					vert[i + 2]));
		}
	}
	else {
		SIZE size = indices.size();
		for (SIZE i = 0; i < size; i++) {
			int ind = indices[i] * 3;
			originalConvexShape->addPoint(
				btVector3(
					vert[ind + 0],
					vert[ind + 1],
					vert[ind + 2]));
		}
	}
	// Create a hull approximation.
	btShapeHull* hull = new btShapeHull(originalConvexShape);
	btScalar margin = originalConvexShape->getMargin();
	hull->buildHull(margin);
	vehicleBodyShape_ = new btConvexHullShape(
		(btScalar*) hull->getVertexPointer(), hull->numVertices());
	delete hull;
	delete originalConvexShape;
	// LocalTrans effectively shifts the center
	// of mass with respect to the chassis.
	btTransform localTrans;
	localTrans.setIdentity();
	localTrans.setOrigin(btVector3(0.0f, 0.0f, 0.0f));
	vehicleCompoundShape_->addChildShape(localTrans, vehicleBodyShape_);
	btTransform tr;
	tr.setIdentity();
	tr.setOrigin(btVector3(
		container->getPos().getX(),
		container->getPos().getY(),
		container->getPos().getZ()));
	Quaternion qt = container->getRot();
	tr.setRotation(
		btQuaternion(qt.getX(), qt.getY(), qt.getZ(), qt.getLength()));
	bool isDynamic = (mass_ != 0.f);
	btVector3 localInertia(0,0,0);
	if (isDynamic)
		vehicleCompoundShape_->calculateLocalInertia(mass_, localInertia);
	btDefaultMotionState* myMotionState =
		new btDefaultMotionState(tr);
	motionState_ = myMotionState;
	btRigidBody::btRigidBodyConstructionInfo cInfo(
		mass_, myMotionState, vehicleCompoundShape_, localInertia);
	rigidBody_ = new btRigidBody(cInfo);
	rigidBody_->setUserPointer(static_cast<void*>(container));
	btRaycastVehicle::btVehicleTuning tunning;
	vehicleRayCaster_ = new btDefaultVehicleRaycaster(dynamicsWorld);
	vehicle_ = new btRaycastVehicle(tunning, rigidBody_, vehicleRayCaster_);
	rigidBody_->setActivationState(DISABLE_DEACTIVATION);
	bool isFrontWheel = true;
	//choose coordinate system
	vehicle_->setCoordinateSystem(0, 1, 2);
	Node* wheel = container->getChild("wheel1");
	wheels_.push_back(wheel);
	btVector3 connectionPointCS0(
		wheel->getPos().getX(),
		wheel->getPos().getY(),
		wheel->getPos().getZ());
	vehicle_->addWheel(
		connectionPointCS0, wheelDirectionCS0, wheelAxleCS,
		suspensionRestLength_, wheelRadius_, tunning, isFrontWheel);
	wheel = container->getChild("wheel2");
	wheels_.push_back(wheel);
	connectionPointCS0 = btVector3(
		wheel->getPos().getX(),
		wheel->getPos().getY(),
		wheel->getPos().getZ());
	vehicle_->addWheel(
		connectionPointCS0, wheelDirectionCS0, wheelAxleCS,
		suspensionRestLength_, wheelRadius_, tunning, isFrontWheel);
	isFrontWheel = false;
	wheel = container->getChild("wheel3");
	wheels_.push_back(wheel);
	connectionPointCS0 = btVector3(
		wheel->getPos().getX(),
		wheel->getPos().getY(),
		wheel->getPos().getZ());
	vehicle_->addWheel(
		connectionPointCS0, wheelDirectionCS0, wheelAxleCS,
		suspensionRestLength_, wheelRadius_, tunning, isFrontWheel);
	wheel = container->getChild("wheel4");
	wheels_.push_back(wheel);
	connectionPointCS0 = btVector3(
		wheel->getPos().getX(),
		wheel->getPos().getY(),
		wheel->getPos().getZ());
	vehicle_->addWheel(
		connectionPointCS0, wheelDirectionCS0, wheelAxleCS,
		suspensionRestLength_, wheelRadius_, tunning, isFrontWheel);
	for (int i = 0; i < vehicle_->getNumWheels(); i++) {
		btWheelInfo& wheel = vehicle_->getWheelInfo(i);
		wheel.m_suspensionStiffness = suspensionStiffness_;
		wheel.m_wheelsDampingRelaxation = suspensionDamping_;
		wheel.m_wheelsDampingCompression = suspensionCompression_;
		wheel.m_frictionSlip = wheelFriction_;
		wheel.m_rollInfluence = rollInfluence_;
		wheel.m_deltaRotation = 0.1f;
	}
    rigidBodies->push_back(rigidBody_);
	dynamicsWorld->addRigidBody(rigidBody_);
	dynamicsWorld->addVehicle(vehicle_);
}

Vehicle::~Vehicle() {
	delete vehicleBodyShape_;
	delete vehicle_;
	delete vehicleRayCaster_;
	//delete vehicleCompoundShape_;
}

void Vehicle::update() {
	float f1 = vehicle_->getWheelInfo(2).m_engineForce;
	float time = static_cast<float>(
		ScriptManager::getServiceLocator()->getFrameTime() / 10.0f);
	float reductionDelta = time * 50.0f;
	if (f1 > 0.0f) {
		f1 -= reductionDelta;
		if (f1 < 0.0f) {
			f1 = 0.0f;
		}
	}
	else {
		f1 += reductionDelta;
		if (f1 > 0.0f) {
			f1 = 0.0f;
		}
	}
	float r1 = vehicle_->getSteeringValue(0);
	reductionDelta = time * 0.0001f * vehicle_->getCurrentSpeedKmHour();
	if (reductionDelta < 0.0f) {
		reductionDelta *= -1.0f;
	}
	if (r1 > 0.0f) {
		r1 -= reductionDelta;
		if (r1 < 0.0f) {
			r1 = 0.0f;
		}
	}
	else {
		r1 += reductionDelta;
		if (r1 > 0.0f) {
			r1 = 0.0f;
		}
	}
	vehicle_->setSteeringValue(r1, 0);
	vehicle_->setSteeringValue(r1, 1);
	vehicle_->applyEngineForce(f1, 2);
	vehicle_->applyEngineForce(f1, 3);
	UINT32 wheelCount = (UINT32) vehicle_->getNumWheels();
	Mat4 mat, matPosRot, matScale;
	for (UINT32 i = 0; i < wheelCount; i++) {
		vehicle_->updateWheelTransform(i, true);
		if (wheels_.size() > i) {
			btTransform& worldTrans =
				vehicle_->getWheelInfo(i).m_worldTransform;
			worldTrans.getOpenGLMatrix(matPosRot);
			Vec3 vScale = wheels_[i]->getScale();
			Matrix::scale(matScale, vScale.getX(), vScale.getY(), vScale.getZ());
			Matrix::multiply(matPosRot, matScale, mat);
			wheels_[i]->setMatrix(mat);
			wheels_[i]->setState(Node::TRANSFORMABLE, false);
			//btQuaternion rot = worldTrans.getRotation();
			//wheels_[i]->getRot().setXYZW(
			//	rot.getX(), rot.getY(), rot.getZ(), rot.getW());
			//btVector3 pos = worldTrans.getOrigin();
			//wheels_[i]->getPos().setXYZ(
			//	pos.getX(), pos.getY(), pos.getZ());

		}
	}
	vehicle_->setBrake(100.0f, 0);
	vehicle_->setBrake(100.0f, 1);
}

vector<Node*>& Vehicle::getWheels() {
	return wheels_;
}

void Vehicle::setRaycastVehicle(btRaycastVehicle* vehicle) {
	vehicle_ = vehicle;
}

Node* Vehicle::getContainer() {
	return container_;
}

btRaycastVehicle* Vehicle::getRaycastVehicle() {
	return vehicle_;
}

float Vehicle::getMass() {
	return mass_;
}

float Vehicle::getSuspensionRestLength() {
	return suspensionRestLength_;
}

float Vehicle::getSuspensionStiffness() {
	return suspensionStiffness_;
}

float Vehicle::getSuspensionDamping() {
	return suspensionDamping_;
}

float Vehicle::getSuspensionCompression() {
	return suspensionCompression_;
}

float Vehicle::getWheelTurn() {
	return wheelTurn_;
}

float Vehicle::getWheelRadius() {
	return wheelRadius_;
}

float Vehicle::getWheelFriction() {
	return wheelFriction_;
}

float Vehicle::getRollInfluence() {
	return rollInfluence_;
}