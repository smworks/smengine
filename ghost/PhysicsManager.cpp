/*
 * PhysicsManager.cpp
 *
 *  Created on: 2013.04.11
 *      Author: Martynas Šustavičius
 */

#include "PhysicsManager.h"
#include "Multiplatform/ServiceLocator.h"
#include "Multiplatform/GraphicsManager.h"
#include "Utils.h"
#include "Quaternion.h"
#include "Node.h"
#include "ModelData.h"
#include "Vec3.h"
#include "ScriptManager.h"
#include "ResourceManager.h"
#include "Resources/Shader.h"
#include "Resources/StaticObject.h"
#include "Resources/TextureRGBA.h"
#include "Extensions/Vehicle.h"

DebugDraw::DebugDraw(GraphicsManager* gm) : gm_(gm), debugMode_(0) {}

void DebugDraw::drawLine(
	const btVector3& from, const btVector3& to,
	const btVector3& color)
{
	gm_->color(color.getX(), color.getY(), color.getZ());
	gm_->vertex(from.getX(), from.getY(), from.getZ());
	gm_->vertex(to.getX(), to.getY(), to.getZ());
}

PhysicsManager::PhysicsManager(ServiceLocator* services) :
	services_(services),
	gm_(0),
	dd_(0),
	debug_(false),
	rigidBodies_(0)
{
	conf_ = new btDefaultCollisionConfiguration();
	dispatcher_ = new btCollisionDispatcher(conf_);
	pairCache_ = new btDbvtBroadphase();
	solver_ = new btSequentialImpulseConstraintSolver();
	dynamicsWorld_ = new btDiscreteDynamicsWorld(
		dispatcher_, pairCache_, solver_, conf_);
	dynamicsWorld_->setGravity(btVector3(0.0f, -9.8f, 0.0f));
	LOGD("Created physics manager.");
}

PhysicsManager::~PhysicsManager() {
	reset();
	delete dd_;
	delete dynamicsWorld_;
	delete solver_;
	delete pairCache_;
	delete dispatcher_;
	delete conf_;
	vector<Vehicle*>::const_iterator itVehicle =
		vehicles_.begin();
	while (itVehicle != vehicles_.end()) {
		delete *itVehicle;
		itVehicle++;
	}
	vehicles_.clear();
	LOGD("Deleted physics manager.");
}

void PhysicsManager::reset() {
	vector<btTriangleMesh*>::const_iterator itMesh = triangleMeshes_.begin();
	while (itMesh != triangleMeshes_.end()) {
		delete *itMesh;
		itMesh++;
	}
	triangleMeshes_.clear();
	for (SIZE i = 0; i < rigidBodies_.size(); i++) {
		dynamicsWorld_->removeCollisionObject(rigidBodies_[i]);
		btMotionState* motionState = rigidBodies_[i]->getMotionState();
		btCollisionShape* shape = rigidBodies_[i]->getCollisionShape();
		delete rigidBodies_[i];
		delete shape;
		delete motionState;
	}
	rigidBodies_.clear();
}

void PhysicsManager::update(double time) {
	static float delta = 1.0f / 60.0f;
	time *= 0.001f;
	int maxSteps = time < delta ? 1 : (int) (time / delta);
	dynamicsWorld_->stepSimulation(btScalar(time), maxSteps);
	SIZE size = rigidBodies_.size();
	for (SIZE i = 0; i < size; i++) {
		btRigidBody* body = rigidBodies_[i];
		if (body && body->getMotionState()) {
			btTransform trans;
			body->getMotionState()->getWorldTransform(trans);
			Node* node = static_cast<Node*>(
				body->getUserPointer());
			btQuaternion rot = trans.getRotation();
			node->getRot().setXYZW(
				rot.getX(), rot.getY(), rot.getZ(), rot.getW());
			btVector3 pos = trans.getOrigin();
			node->getPos().setXYZ(pos.getX(), pos.getY(), pos.getZ());
			node->setState(Node::POSITION, true);
			node->setState(Node::ROTATION, true);
		}
	}
	vector<Vehicle*>::const_iterator vit = vehicles_.begin();
	while (vit != vehicles_.end()) {
		(*vit)->update();
		vit++;
	}
	if (debug_ && gm_ != 0) {
		dynamicsWorld_->debugDrawWorld();
	}
}

void PhysicsManager::add(Node* node) {
	if (!node->hasResource(Resource::STATIC_OBJECT)) {
		LOGW("Unable to add node \"%s\" to physics manager, becaus model is missing.",
			node->getName().c_str());
		return;
	}
	StaticObject* model = static_cast<StaticObject*>(node->getResource(Resource::STATIC_OBJECT));
	string modelType = model->getAttribute(StaticObject::ATTR_TYPE);
	if (model->getAttribute("physics_object") == "vehicle") {
		PROFILE("Adding vehicle to physics manager.");
		addVehicle(node);
		return;
	}
	else if (modelType == StaticObject::VAL_TERRAIN) {
		PROFILE("Adding terrain to physics manager.");
		addTerrain(node);
	}
	else if (modelType == StaticObject::VAL_PLANE) {
		PROFILE("Adding box to physics manager.");
		addBox(node);
	}
	else if (modelType == StaticObject::VAL_SPHERE) {
		PROFILE("Adding sphere to physics manager.");
		addSphere(node);
	}
	else {
		PROFILE("Adding mesh to physics manager.");
		addMesh(node);
	}
	PROFILE("Finished adding.");
}

void PhysicsManager::move(Node* node, Vec3& force, bool linear) {
	btCollisionObjectArray arr = dynamicsWorld_->getCollisionObjectArray();
	int size = arr.size();
	for (int i = 0; i < size; i++) {
		if (arr[i]->getUserPointer() == node) {
			//btTransform tr;
			//tr.setIdentity();
			//tr.setOrigin(btVector3(pos.getX(), pos.getY(), pos.getZ()));
			btRigidBody* body = btRigidBody::upcast(arr[i]);
			body->activate();
			//body->applyCentralForce(btVector3(0.0f, 10000.0f, 0.0f));
			body->setLinearVelocity(
				btVector3(force.getX(), force.getY(), force.getZ()));
			//body->setCenterOfMassTransform(tr);
			return;
		}
	}
}

void PhysicsManager::rotate(
	Node* node, Vec3& force, bool linear)
{
	//Physics* phys = static_cast<Physics*>(
	//	node->getResource(Resource::PHYSICS));
	//if (phys->getSpecial() == Physics::VEHICLE) {
	//	for (UINT32 i = 0; i < vehicles_.size(); i++) {
	//		btRaycastVehicle* vehicle = vehicles_[i]->getRaycastVehicle();
	//		float maxRot = DEG_TO_RAD(
	//			toFloat(phys->getAttribute(Vehicle::MAX_WHEEL_ROTATION).c_str()));
	//		if (vehicle->getRigidBody()->getUserPointer() == node) {
	//			float rot = vehicle->getSteeringValue(0) + DEG_TO_RAD(force.getY());
	//			if (rot > maxRot) {
	//				rot = maxRot;
	//			} else if (rot < -maxRot) {
	//				rot = -maxRot;
	//			}
	//			vehicle->setSteeringValue(rot, 0);
	//			vehicle->setSteeringValue(rot, 1);
	//			return;
	//		}
	//	}
	//	return;
	//}
}

void PhysicsManager::remove(Node* node) {
	btCollisionObjectArray arr = dynamicsWorld_->
		getCollisionObjectArray();
	int size = arr.size();
	for (int i = 0; i < size; i++) {
		if (arr[i]->getUserPointer() == node) {
			btRigidBody* body = btRigidBody::upcast(arr[i]);
			dynamicsWorld_->removeCollisionObject(body);
			btMotionState* motionState = body->getMotionState();
			btCollisionShape* shape = body->getCollisionShape();
			delete body;
			delete shape;
			delete motionState;
			return;
		}
	}
}

void PhysicsManager::updateNode(Node* node) {
	remove(node);
	add(node);
}

Node* PhysicsManager::rayCast(Vec3& start, Vec3& end) {
	btVector3 rayFrom(start.getX(), start.getY(), start.getZ());
	btVector3 rayTo(end.getX(), end.getY(), end.getZ());
	btCollisionWorld::ClosestRayResultCallback rayCallback(rayFrom, rayTo);
	rayCallback.m_flags |= btTriangleRaycastCallback::kF_FilterBackfaces;
	dynamicsWorld_->rayTest(rayFrom, rayTo, rayCallback);
	if (rayCallback.hasHit()) {
		Node* node = static_cast<Node*>(
			rayCallback.m_collisionObject->getUserPointer());
		btRigidBody* pBody = btRigidBody::upcast(
			(btCollisionObject*) rayCallback.m_collisionObject);
		if (pBody && pBody->getMotionState()) {
			//pos.print();
			//rot.print();
			LOGD("Raycast to: %s", node->getName().c_str());
			return node;
		}
		return 0;
	}
	return 0;
}

void PhysicsManager::setDebug(bool state) {
	debug_ = state;
}

void PhysicsManager::setGraphicsManager(GraphicsManager* gm) {
    dd_ = new DebugDraw(gm);
    gm_ = gm;
    dynamicsWorld_->setDebugDrawer(dd_);
    dynamicsWorld_->getDebugDrawer()->setDebugMode(
        btIDebugDraw::DBG_DrawWireframe);
}

void PhysicsManager::addBox(Node* node) {
	StaticObject* model = static_cast<StaticObject*>(node->getResource(Resource::STATIC_OBJECT));
	btCollisionShape* groundShape = new btBoxShape(btVector3(
		toFloat(model->getAttribute(Resource::ATTR_WIDTH).c_str()) * 0.5f * node->getScale().getX(),
		toFloat(model->getAttribute(Resource::ATTR_HEIGHT).c_str()) * 0.5f * node->getScale().getY(),
		toFloat(model->getAttribute(Resource::ATTR_DEPTH).c_str()) * 0.5f * node->getScale().getZ()));
	btTransform startTransform;
	startTransform.setIdentity();
	startTransform.setOrigin(
		btVector3(
			node->getPos().getX(),
			node->getPos().getY(),
			node->getPos().getZ()));
	Quaternion& q = node->getRot();
	float* arr = q.toArray();
	startTransform.setRotation(btQuaternion(
		arr[0], arr[1], arr[2], arr[3]));
	btVector3 localInertia(0.0f, 0.0f, 0.0f);
	float mass = toFloat(model->getAttribute(Resource::ATTR_MASS).c_str());
	if (mass > GHOST_DELTA) {
		groundShape->calculateLocalInertia(
			toFloat(model->getAttribute(Resource::ATTR_MASS).c_str()), localInertia);
	}
	btDefaultMotionState* myMotionState =
		new btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(
		mass, myMotionState, groundShape, localInertia);
	rbInfo.m_friction = toFloat(model->getAttribute(Resource::ATTR_FRICTION).c_str());
	btRigidBody* body = new btRigidBody(rbInfo);
	body->setUserPointer(static_cast<void*>(node));
	//if (pa->getDisableRotation()) {
	//	body->setInvInertiaDiagLocal(btVector3(0.0f, 0.0f, 0.0f));
	//	body->updateInertiaTensor();
	//}
	dynamicsWorld_->addRigidBody(body);
	rigidBodies_.push_back(body);
}

void PhysicsManager::addSphere(Node* node) {
	StaticObject* model = static_cast<StaticObject*>(node->getResource(Resource::STATIC_OBJECT));
	float mass = toFloat(model->getAttribute(Resource::ATTR_MASS).c_str());
	float scale = node->getScale().getMaxVal();
	float radius = toFloat(model->getAttribute(Resource::ATTR_RADIUS).c_str()) * scale;
	btCollisionShape* colShape = new btSphereShape(
		btScalar(radius));
	btTransform startTransform;
	startTransform.setIdentity();
	startTransform.setOrigin(
		btVector3(
			node->getPos().getX(),
			node->getPos().getY(),
			node->getPos().getZ()));
	Quaternion& q = node->getRot();
	float* arr = q.toArray();
	startTransform.setRotation(btQuaternion(
		arr[0], arr[1], arr[2], arr[3]));
	btVector3 localInertia(0.0f, 0.0f, 0.0f);
	if (mass > GHOST_DELTA) {
		colShape->calculateLocalInertia(mass, localInertia);
	}
	btDefaultMotionState* myMotionState =
		new btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(
		mass, myMotionState, colShape, localInertia);
	rbInfo.m_angularDamping = mass;
	btRigidBody* body = new btRigidBody(rbInfo);
	body->setUserPointer(static_cast<void*>(node));
	body->setDamping(0.8f, 0.0f);
	//if (pa->getDisableRotation()) {
	//	//body->setInvInertiaDiagLocal(btVector3(0.0f, 0.0f, 0.0f));
	//	//body->updateInertiaTensor();
	//	body->setSleepingThresholds(0.0f, 0.0f);
	//	body->setAngularFactor(0.0);
	//}
	body->setCcdMotionThreshold(radius);
	body->setCcdSweptSphereRadius(0.9f * radius);
	dynamicsWorld_->addRigidBody(body);
	rigidBodies_.push_back(body);
}

void PhysicsManager::addMesh(Node* node) {
	StaticObject* model = static_cast<StaticObject*>(node->getResource(Resource::STATIC_OBJECT));
	if (model->getModel() == 0 || model->getModel()->getVertexCount() == 0) {
		LOGW("Adding mesh from node \"%s\", that has no vertex data.",
			node->getName().c_str());
		return;
	}
	btCollisionShape* colShape = 0;
	btVector3 localInertia(0.0f, 0.0f, 0.0f);
	ModelData* modelData = model->getModel();
	SIZE vertexCount = modelData->getVertexCount();
	SIZE floatStride = model->getVertexStride() / sizeof(float);
	float* data = reinterpret_cast<float*>(modelData->getVertices());
	vector<float> vert;
	for (SIZE i = 0; i < vertexCount; i++) {
		vert.push_back(data[i * floatStride + 0] * node->getScale().getX());
		vert.push_back(data[i * floatStride + 1] * node->getScale().getY());
		vert.push_back(data[i * floatStride + 2] * node->getScale().getZ());
	}
	vector<UINT32> indices;
	if (model->getIndexCount() > 0 && model->getModel()->getIndexType() == Renderable::INDEX_TYPE_USHORT) {
		UINT16* ind = modelData->getIndicesShort();
		for (UINT32 i = 0; i < model->getIndexCount(); i++) {
			indices.push_back(ind[i]);
		}
	}
	else if (model->getIndexCount() > 0) {
		UINT32* ind = modelData->getIndicesInt();
		for (UINT32 i = 0; i < model->getIndexCount(); i++) {
			indices.push_back(ind[i]);
		}
	}
	float mass = toFloat(model->getAttribute(Resource::ATTR_MASS).c_str());
	float friction = toFloat(model->getAttribute(Resource::ATTR_FRICTION).c_str());
	if (toBool(model->getAttribute(Resource::ATTR_APPROXIMATION))) {
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
		btShapeHull* hull = new btShapeHull(originalConvexShape);
		btScalar margin = originalConvexShape->getMargin();
		hull->buildHull(margin);
		colShape = new btConvexHullShape(
			(btScalar*) hull->getVertexPointer(), hull->numVertices());
		delete hull;
		delete originalConvexShape;
		if (mass > GHOST_DELTA) {
			colShape->calculateLocalInertia(mass, localInertia);
		}
	}
	else {
		// For indices use: btTriangleIndexVertexArray* mesh = new btTriangleIndexVertexArray(...)
		btTriangleMesh* mesh = new btTriangleMesh;
		triangleMeshes_.push_back(mesh);
		SIZE size = vert.size();
		if (model->getIndexCount() == 0) {
			for (SIZE i = 0; i < size; i += 9) {
				mesh->addTriangle(
					btVector3(vert[i + 0], vert[i + 1], vert[i + 2]),
					btVector3(vert[i + 3], vert[i + 4], vert[i + 5]),
					btVector3(vert[i + 6], vert[i + 7], vert[i + 8]),
					true);
			}
		}
		else {
			SIZE faceCount = indices.size() / 3;
			for (SIZE i = 0; i < faceCount; i++) {
				int in = indices[i * 3 + 0];
				int in2 = indices[i * 3 + 1];
				int in3 = indices[i * 3 + 2];
				mesh->addTriangle(
					btVector3(vert[in * 3], vert[in * 3 + 1], vert[in * 3 + 2]),
					btVector3(vert[in2 * 3], vert[in2 * 3 + 1], vert[in2 * 3 + 2]),
					btVector3(vert[in3 * 3], vert[in3 * 3 + 1], vert[in3 * 3 + 2]),
					true);
			}
		}
		colShape = new btBvhTriangleMeshShape(mesh, true);
	}
	btTransform startTransform;
	startTransform.setIdentity();
	startTransform.setOrigin(btVector3(
		node->getPos().getX(),
		node->getPos().getY(),
		node->getPos().getZ()));
	Quaternion& q = node->getRot();
	float* arr = q.toArray();
	startTransform.setRotation(btQuaternion(arr[0], arr[1], arr[2], arr[3]));
	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);
	btRigidBody* body = new btRigidBody(rbInfo);
	body->setCenterOfMassTransform(startTransform);
	body->setUserPointer(static_cast<void*>(node));
	rbInfo.m_friction = friction;
	dynamicsWorld_->addRigidBody(body);
	rigidBodies_.push_back(body);
}

//void PhysicsManager::addCapsule(Node* node) {
//	Physics* pa = static_cast<Physics*>(node->getResource(Resource::PHYSICS));
//	BoundingCapsule* bc = static_cast<BoundingCapsule*>(
//		pa->getBoundingVolume());
//	btCollisionShape* colShape = new btCapsuleShape(
//		btScalar(bc->getRadius(node->getScale())), btScalar(bc->getHeight()));
//	btTransform startTransform;
//	startTransform.setIdentity();
//	startTransform.setOrigin(
//		btVector3(
//			node->getPos().getX(),
//			node->getPos().getY(),
//			node->getPos().getZ()));
//	Quaternion& q = node->getRot();
//	float* arr = q.toArray();
//	startTransform.setRotation(btQuaternion(
//		arr[0], arr[1], arr[2], arr[3]));
//	btVector3 localInertia(0.0f, 0.0f, 0.0f);
//	if (pa->getMass() > GHOST_DELTA) {
//		colShape->calculateLocalInertia(pa->getMass(), localInertia);
//	}
//	btDefaultMotionState* myMotionState =
//		new btDefaultMotionState(startTransform);
//	btRigidBody::btRigidBodyConstructionInfo rbInfo(
//		pa->getMass(), myMotionState, colShape,localInertia);
//	rbInfo.m_angularDamping = pa->getFriction();
//	btRigidBody* body = new btRigidBody(rbInfo);
//	body->setUserPointer(static_cast<void*>(node));
//	if (pa->getDisableRotation()) {
//		//body->setInvInertiaDiagLocal(btVector3(0.0f, 0.0f, 0.0f));
//		//body->updateInertiaTensor();
//		body->setSleepingThresholds(0.0f, 0.0f);
//		body->setAngularFactor(0.0);
//	}
//	dynamicsWorld_->addRigidBody(body);
//	rigidBodies_.push_back(body);
//}

void PhysicsManager::addVehicle(Node* node) {
	Vehicle* baw = new Vehicle(node, dynamicsWorld_, &rigidBodies_);
	vehicles_.push_back(baw);
}

void PhysicsManager::addTerrain(Node* node) {
	StaticObject* model = static_cast<StaticObject*>(node->getResource(Resource::STATIC_OBJECT));
	if (model->getModel() == 0 || model->getModel()->getVertexCount() == 0) {
		LOGW("Adding mesh from node \"%s\", that has no vertex data.",
			node->getName().c_str());
		return;
	}
	float minHeight = FLT_MAX, maxHeight = -FLT_MAX;
	SIZE vertexCount = model->getVertexCount();
	SIZE floatStride = model->getVertexStride() / sizeof(float);
	float* data = reinterpret_cast<float*>(model->getModel()->getVertices());
	heightMaps_.push_back(vector<float>());
	vector<float>& vert = heightMaps_[heightMaps_.size() - 1];
	for (SIZE i = 0; i < vertexCount; i++) {
		vert.push_back(data[i * floatStride + 1] * node->getScale().getY());
		if (minHeight > vert[i]) {
			minHeight = vert[i];
		}
		if (maxHeight < vert[i]) {
			maxHeight = vert[i];
		}
	}
	TextureRGBA* map = static_cast<TextureRGBA*>(services_->getRM()->get(
		Resource::TEXTURE_2D, model->getAttribute(Resource::ATTR_FILE)));
	btHeightfieldTerrainShape * heightfieldShape = new btHeightfieldTerrainShape(
		map->getWidth(),
		map->getHeight(),
		&vert[0],
		1.0f,
		minHeight, maxHeight,
		1, // 1 - y up.
		PHY_FLOAT, false);
	heightfieldShape->setLocalScaling(btVector3(0.75f, 1.0f, 0.75f));
	btTransform tr;
	tr.setIdentity();
	tr.setOrigin(btVector3(
		node->getPos().getX(),
		node->getPos().getY(),
		node->getPos().getZ()));
	btVector3 localInertia(0,0,0);
	btDefaultMotionState* myMotionState =
		new btDefaultMotionState(tr);
	btRigidBody::btRigidBodyConstructionInfo cInfo(
		0.0f, myMotionState, heightfieldShape, localInertia);
	btRigidBody* body = new btRigidBody(cInfo);
	body->setUserPointer(static_cast<void*>(node));
	dynamicsWorld_->addRigidBody(body);
	rigidBodies_.push_back(body);
}

Vehicle* PhysicsManager::getVehicle(Node* node) {
	vector<Vehicle*>::iterator it = vehicles_.begin();
	while (it != vehicles_.end()) {
		if ((*it)->getContainer() == node) {
			return (*it);
		}
		it++;
	}
	return 0;
}