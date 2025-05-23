﻿/*
 * PhysicsManager.h
 *
 *  Created on: 2013.02.05
 *      Author: Martynas Šustavičius
 */

#ifndef PHYSICSMANAGER_H_
#define PHYSICSMANAGER_H_

#include "Multiplatform/Ghost.h"
#ifndef ENABLE_PHYSICS
class Node;
class Vec3;
class ServiceLocator;
class GraphicsManager;

class PhysicsManager {
public:
	PhysicsManager(ServiceLocator* services) {}
	~PhysicsManager() {}
	void reset() {}
	void update(double time) {}
	void add(Node* node) {}
	void move(Node* node, Vec3& force, bool linear = false) {}
	void rotate(
		Node* node, Vec3& angularMomentum, bool linear = false) {}
	void remove(Node* node) {}
	bool has(Node* node) { return false; }
	void updateNode(Node* node) {}
    Node* rayCast(Vec3& start, Vec3& end) { return nullptr; }
	void setDebug(bool state) {}
	void setGraphicsManager(GraphicsManager* gm) {}
	void setDebugRendering(bool state) {}
	bool isDebugRenderingEnabled() { return false; }
};
#else

#include "../dependencies/includes/bullet/btBulletDynamicsCommon.h"

class Node;
class Vec3;
class Vehicle;
class ServiceLocator;
class GraphicsManager;
class Attributes;

#define STEP_DELTA 0.01666666f
#define GRAVITY -9.8f
#define DEFAULT_FRICTION 0.1f
#define DEFAULT_LINEAR_DAMPING 0.1f
#define DEFAULT_ANGULAR_DAMPING 0.1f
#define DEFAULT_RESTITUTION 0.1f
#define MAX_SIZE 10.0f
#define MIN_SIZE 0.05f

class DebugDraw : public btIDebugDraw {
public:
	DebugDraw(GraphicsManager* gm);

	void drawLine(
		const btVector3& from, const btVector3& to,
		const btVector3& color);

	void reportErrorWarning(const char* warningString) {
		LOGE("Error while drawing bullet debug world: %s", warningString);
	}

	void setDebugMode(int debugMode) {
		debugMode_ = debugMode;
	}

	int getDebugMode() const {
		return debugMode_;
	}

	void draw3dText(const btVector3& location,const char* textString) {
		LOGD("Bullet: draw3dText.");
	}

	void drawContactPoint(const btVector3& pointOnB,
		const btVector3& normalOnB,  btScalar distance,
		int lifeTime, const btVector3& color)
	{
		LOGD("Bullet: drawContactPoint.");
	}
private:
	GraphicsManager* gm_;
	int debugMode_;
};

class PhysicsManager {
public:
	PhysicsManager(ServiceLocator* services);
	~PhysicsManager();

	/**
	 * Release all data.
	 * Set physics manager, to initial state.
	 */
	void reset();

	/**
	 * @param time - time elapsed since last frame,
	 * in milliseconds.
	 */
	void update(double time);

	void add(Node* node);
	void remove(Node* node);
	bool has(Node* node);

	/**
	 * Force is measured in newtons.
	 * @param node - node containing physics resource.
	 * @param force - vector indicating direction and amount of force.
	 * Force is measured in Newtons.
	 * @param linear - true if object only needs to be moved
	 * in specified direction, without physic laws.
	 */
	void move(Node* node, Vec3& force, bool linear = false);

	/**
	 * Momentum is measured in newtons.
	 * @param node - node containing physics resource.
	 * @param angularMomentum - vector whose components
	 * specify force that should be applied for each axis.
	 * Force is measured in Newtons.
	 * @param linear - true if object only needs to be moved
	 * by specified amount of rotation, without physic laws.
	 */
	void rotate(
		Node* node, Vec3& angularMomentum, bool linear = false);



	/**
	 * Removes node from world if it exists.
	 * Adds node to world with its current configuration.
	 * @param node - node containing physics resource.
	 */
	void updateNode(Node* node);

	/**
	 * @param start - ray starting position.
	 * @param end - ray end position.
	 * @return Pointer to node, that is hit by ray first.
	 */
	Node* rayCast(Vec3& start, Vec3& end);

	void setDebugRendering(bool state);
	bool isDebugRenderingEnabled();

    /**
     * While debuging, graphics manager is needed to draw
     * physical world imeplementations.
     * @param gm - graphics manager object.
     */
	void setGraphicsManager(GraphicsManager* gm);

private:
	/**
	 * @param node - node containing vehicle.
	 * @return Vehicle object.
	 */
	Vehicle* getVehicle(Node* node);

	/**
	 * Adds parameters like friction, damping and restitution to body.
	 * @param body - body that will have attributes applied.
	 * @param attributes - attribute object.
	 */
	void setupAttributes(btRigidBody* body, Attributes& attributes);
	void addBox(Node* node);
	void addSphere(Node* node);
	void addMesh(Node* node);
	void addCapsule(Node* node);
	void addVehicle(Node* node);
	void addTerrain(Node* node);
private:
	ServiceLocator* services;
	btDefaultCollisionConfiguration* conf_;
	btCollisionDispatcher* dispatcher_;
	btBroadphaseInterface* pairCache_;
	btSequentialImpulseConstraintSolver* solver_;
	btDiscreteDynamicsWorld* dynamicsWorld_;
	vector<btStridingMeshInterface*> meshInterfaces_;
	vector<Vehicle*> vehicles_;
	vector<btRigidBody*> rigidBodies_;
	vector<vector<float> > heightMaps_;
	GraphicsManager* gm_;
	DebugDraw* dd_;
	bool debug_;
};

#endif
#endif
