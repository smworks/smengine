/*
 * PhysicsManager.cpp
 *
 *  Created on: 2013.04.11
 *      Author: Martynas Šustavičius
 */

#ifndef DISABLE_PHYSICS
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
#include "Resources/Attributes.h"

#include "../dependencies/includes/bullet/BulletCollision/NarrowPhaseCollision/btRaycastCallback.h"
#include "../dependencies/includes/bullet/BulletCollision/CollisionShapes/btShapeHull.h"
#include "../dependencies/includes/bullet/BulletCollision/Gimpact/btGImpactShape.h"
#include "../dependencies/includes/bullet/BulletCollision/Gimpact/btGImpactCollisionAlgorithm.h"
#include "../dependencies/includes/bullet/BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h"

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
	btGImpactCollisionAlgorithm::registerAlgorithm(dispatcher_);
	pairCache_ = new btDbvtBroadphase();
	solver_ = new btSequentialImpulseConstraintSolver();
	dynamicsWorld_ = new btDiscreteDynamicsWorld(dispatcher_, pairCache_, solver_, conf_);
	dynamicsWorld_->setGravity(btVector3(0.0f, GRAVITY, 0.0f));
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
	vector<btStridingMeshInterface*>::const_iterator itMesh = meshInterfaces_.begin();
	while (itMesh != meshInterfaces_.end()) {
		delete *itMesh;
		itMesh++;
	}
	meshInterfaces_.clear();
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
    if (rigidBodies_.size() == 0 && vehicles_.size() == 0) {
        return;
    }
	float delta = (float) time * 0.001f;
	int maxSteps = delta < STEP_DELTA ? 1 : (int) (delta / STEP_DELTA) + 1;
	dynamicsWorld_->stepSimulation(delta, maxSteps, STEP_DELTA);
	SIZE size = rigidBodies_.size();
	for (SIZE i = 0; i < size; i++) {
		btRigidBody* body = rigidBodies_[i];
		if (body && body->getMotionState()) {
			btTransform trans;
			body->getMotionState()->getWorldTransform(trans);
			Node* node = static_cast<Node*>(
				body->getUserPointer());
			btQuaternion rot = trans.getRotation();
			node->getRot().setXYZW(rot.getX(), rot.getY(), rot.getZ(), rot.getW());
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
	btVector3 dimensions = btVector3(
		toFloat(model->getAttribute(Resource::ATTR_WIDTH).c_str()) * 0.5f * node->getScale().getX(),
		toFloat(model->getAttribute(Resource::ATTR_HEIGHT).c_str()) * 0.5f * node->getScale().getY(),
		toFloat(model->getAttribute(Resource::ATTR_DEPTH).c_str()) * 0.5f * node->getScale().getZ());
	btCollisionShape* shape = 0;
	if (fabs(dimensions.y()) < GHOST_DELTA &&
		(fabs(dimensions.x()) > MAX_SIZE || fabs(dimensions.y()) > MAX_SIZE)) {
		shape = new btStaticPlaneShape(btVector3(0, 1, 0), node->getPos().getY());
		LOGD("Using infinite plate instead of box, because box height is 0 and other "
			"dimensions are bigger than maximum recommended %f.", MAX_SIZE);
	}
	else {
		shape = new btBoxShape(dimensions);
	}
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
		shape->calculateLocalInertia(
			toFloat(model->getAttribute(Resource::ATTR_MASS).c_str()), localInertia);
	}
	btDefaultMotionState* myMotionState =
		new btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(
		mass, myMotionState, shape, localInertia);
	btRigidBody* body = new btRigidBody(rbInfo);
	body->setUserPointer(static_cast<void*>(node));
	setupAttributes(body, model->getAttributes());
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
	btCollisionShape* colShape = new btSphereShape(radius);
	btTransform startTransform;
	startTransform.setIdentity();
	startTransform.setOrigin(btVector3(
		node->getPos().getX(), node->getPos().getY(), node->getPos().getZ()));
	Quaternion& q = node->getRot();
	float* arr = q.toArray();
	startTransform.setRotation(btQuaternion(arr[0], arr[1], arr[2], arr[3]));
	btVector3 localInertia(0.0f, 0.0f, 0.0f);
	if (mass > GHOST_DELTA) {
		colShape->calculateLocalInertia(mass, localInertia);
	}
	btDefaultMotionState* motionState = new btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, colShape, localInertia);
	btRigidBody* body = new btRigidBody(rbInfo);
	body->setUserPointer(static_cast<void*>(node));
	setupAttributes(body, model->getAttributes());
	//if (pa->getDisableRotation()) {
	//	//body->setInvInertiaDiagLocal(btVector3(0.0f, 0.0f, 0.0f));
	//	//body->updateInertiaTensor();
	//	body->setSleepingThresholds(0.0f, 0.0f);
	//	body->setAngularFactor(0.0);
	//}
	//body->setCcdMotionThreshold(radius);
	//body->setCcdSweptSphereRadius(0.9f * radius);
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
	ModelData* modelData = model->getModel();
	SIZE vertexCount = modelData->getVertexCount();
	float* data = reinterpret_cast<float*>(modelData->getVertices());
	float mass = toFloat(model->getAttribute(Resource::ATTR_MASS).c_str());
	if (toBool(model->getAttribute(Resource::ATTR_APPROXIMATION))) {
		LOGD("Loading approximated convex hull shape.");
		btConvexHullShape* hullShape = new btConvexHullShape(
			data, (int) vertexCount, modelData->getVertexStride());
		btShapeHull* hull = new btShapeHull(hullShape);
		btScalar margin = hullShape->getMargin();
		hull->buildHull(margin);
		colShape = new btConvexHullShape((btScalar*) hull->getVertexPointer(), hull->numVertices());
		delete hull;
		delete hullShape;
	}
	else {
		if (model->getModel()->getIndexCount() == 0) {
			LOGD("Loading triangle mesh with no indices.");
			btTriangleMesh* mesh = new btTriangleMesh;
			meshInterfaces_.push_back(mesh);
			SIZE offset1, offset2, offset3;
			SIZE floatStride = model->getVertexStride() / sizeof(float);
			LOGD("VertexCount: %d", vertexCount);
			for (SIZE i = 0; i < vertexCount; i += 3) {

				offset1 = i * floatStride;
				offset2 = (i + 1) * floatStride;
				offset3 = (i + 2) * floatStride;
				mesh->addTriangle(
					btVector3(data[offset1 + 0], data[offset1 + 1], data[offset1 + 2]),
					btVector3(data[offset2 + 0], data[offset2 + 1], data[offset2 + 2]),
					btVector3(data[offset3 + 0], data[offset3 + 1], data[offset3 + 2]),
					true);
			}
			colShape = new btBvhTriangleMeshShape(mesh, true);
		}
		else {
			UINT8* vertexArray = reinterpret_cast<UINT8*>(modelData->getVertices());
			PHY_ScalarType type;
			UINT8* indexArray;
			if (modelData->getIndexType() == Renderable::INDEX_TYPE_USHORT) {
				type = PHY_SHORT;
				indexArray = reinterpret_cast<UINT8*>(modelData->getIndicesShort());
			}
			else {
				type = PHY_INTEGER;
				indexArray = reinterpret_cast<UINT8*>(modelData->getIndicesInt());
			}
			btTriangleIndexVertexArray* meshInterface = new btTriangleIndexVertexArray();
			meshInterfaces_.push_back(meshInterface);
			btIndexedMesh part;
			// Vertices
			part.m_numVertices = (int) vertexCount;
			part.m_vertexBase = vertexArray;
			part.m_vertexStride = modelData->getVertexStride();
			part.m_vertexType = PHY_FLOAT;
			// Indices.
			part.m_numTriangles = (int) modelData->getIndexCount() / 3;
			part.m_triangleIndexBase = indexArray;
			part.m_triangleIndexStride = modelData->getIndexStride() * 3;
			part.m_indexType = type;
			// Add mesh to mesh interface.
			meshInterface->addIndexedMesh(part, type);
			if (mass > GHOST_DELTA) {
				LOGD("Loading gimpact mesh shape.");
				btGImpactMeshShape* impactMeshShape = new btGImpactMeshShape(meshInterface);
				impactMeshShape->setMargin(0);	
				impactMeshShape->updateBound();
				colShape = impactMeshShape;
			}
			else {
				LOGD("Loading triangle mesh shape.");
				colShape = new btBvhTriangleMeshShape(meshInterface, true);
			}
		}
	}
	Vec3& scale = node->getScale();
	colShape->setLocalScaling(btVector3(scale.getX(), scale.getY(), scale.getZ()));
	btVector3 localInertia(0.0f, 0.0f, 0.0f);
	if (mass > GHOST_DELTA) {
		colShape->calculateLocalInertia(mass, localInertia);
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
	setupAttributes(body, model->getAttributes());
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
		vert.push_back(data[i * floatStride + 1]);
		if (minHeight > vert[i]) {
			minHeight = vert[i];
		}
		if (maxHeight < vert[i]) {
			maxHeight = vert[i];
		}
	}
	TextureRGBA* map = static_cast<TextureRGBA*>(services_->getRM()->get(
		Resource::TEXTURE_2D, model->getAttribute(Resource::ATTR_FILE)));
	btHeightfieldTerrainShape* heightfieldShape = new btHeightfieldTerrainShape(
		map->getWidth(),
		map->getHeight(),
		&vert[0],
		1.0f,
		minHeight, maxHeight,
		1, // 1 - y up.
		PHY_FLOAT, false);
	Vec3& scale = node->getScale();
	heightfieldShape->setLocalScaling(
		btVector3(0.5f * scale.getX(), 1.0f * scale.getY(), 0.5f * scale.getZ()));
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
	setupAttributes(body, model->getAttributes());
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

void PhysicsManager::setupAttributes(btRigidBody* body, Attributes& attributes) {
	float friction = attributes.getString(Resource::ATTR_FRICTION) == Resource::VAL_NOT_SET ?
		DEFAULT_FRICTION : toFloat(attributes.getString(Resource::ATTR_FRICTION).c_str());
	float linearDamping = attributes.getString(Resource::ATTR_LINEAR_DAMPING) == Resource::VAL_NOT_SET ?
		DEFAULT_LINEAR_DAMPING : toFloat(attributes.getString(Resource::ATTR_LINEAR_DAMPING).c_str());
	float angularDamping = attributes.getString(Resource::ATTR_ANGULAR_DAMPING) == Resource::VAL_NOT_SET ?
		DEFAULT_ANGULAR_DAMPING : toFloat(attributes.getString(Resource::ATTR_ANGULAR_DAMPING).c_str());
	float restitution = attributes.getString(Resource::ATTR_RESTITUTION) == Resource::VAL_NOT_SET ?
		DEFAULT_RESTITUTION : toFloat(attributes.getString(Resource::ATTR_RESTITUTION).c_str());
	body->setFriction(friction);
	body->setDamping(linearDamping, angularDamping);
	body->setRestitution(restitution);
}
#endif