/*
 * Node.h
 *
 *  Created on: 2012.06.20
 *      Author: MS
 */

#ifndef NODE_H_
#define NODE_H_

#include "Multiplatform/Ghost.h"
#include "Matrix.h"
#include "Quaternion.h"
class Resource;

class Node {
public:
	enum States {
		UPDATABLE = 0x01, // Node data update.
		RENDERABLE = 0x02, // Node rendering on screen.
		TRANSFORMABLE = 0x04, // Position, rotation and scaling update.
		POSITION = 0x08, // Position update.
		ROTATION = 0x10, // Rotation update.
		SCALING = 0x20 // Scaling update.
	};
public:
	Node();
	~Node();

	/**
	 * @param name - name of the node.
	 */
	void setName(const string &name);

	/**
	 * @return Name of the node.
	 */
	const string& getName();

	/**
	 * @param state - one of the possible States enum
	 * values.
	 * @param value - value to set state to.
	 */
	void setState(States state, bool value);

	/**
	 * @param state - integer containing single
	 * bit state.
	 * @return True if state set to 1, false otherwise.
	 */
	bool getState(States state);

	/**
	 * @return Reference to position vector.
	 */
	Vec3& getPos();

	/**
	 * Unlike simple getPos(), this method goes through node
	 * hierarchy up till it reaches root node and adds position
	 * from each hierarchy level. After method is finished,
	 * specified vector will contain absolute node coordinates.
	 * @param pos - vector that will contain absolute position.
	 */
	void getPosAbs(Vec3& vec);

	/**
	 * @return Reference to rotation quaternion.
	 */
	Quaternion& getRot();

	/**
	 * @return Reference to scale vector.
	 */
	Vec3& getScale();

	/**
	 * Setting matrix implies, that no other transformations
	 * need to be calculated. Position, rotation
	 * and scale are ignored.
	 * @param mat - combined transformation matrix.
	 */
	void setMatrix(Mat4 mat);

	/**
	 * @return Combined transformation matrix.
	 */
	float* getMatrix();

	/**
	 * @return Position matrix.
	 */
	float* getMatPos();

	/**
	 * @return Rotation matrix.
	 */
	float* getMatRot();

	/**
	 * @return Scaling matrix.
	 */
	float* getMatScale();

	/**
	 * @parent - parent node.
	 */
	void setParent(Node* parent);

	/**
	 * @return Pointer to parent node,
	 * or pointer to zero if no parent
	 * was specified.
	 */
	Node* getParent();

	/**
	 * @param child - child node.
	 */
	void addChild(Node* child);

	/**
	 * @param child - child to be removed.
	 */
	void removeChild(Node* child);

	/**
	 * @param name - name of the child node.
	 * @return Pointer to node, or pointer to zero
	 * if no child with specified name was found.
	 */
	Node* getChild(const string& name);

	/**
	 * Returns number of children nodes contained
	 * in this node, as well in childrens children
	 * nodes.
	 * (also includes this node to the counter).
	 * @return Number of children nodes.
	 */
	UINT32 getChildCount();

	/**
	 * Difference between this and
	 * getChild() method is that
	 * this method searches not
	 * only among children, but also
	 * among childrens children and so on.
	 * @param name - name of the child node.
	 * @return Pointer to node, or pointer to zero
	 * if no child with specified name was found.
	 */
	Node* search(const string&);

	/**
	 * @return Vector with children nodes.
	 */
	const vector<Node*>& getChildren();

	/**
	 * Adds this and all children scene nodes
	 * that are renderable to specified vector.
	 * @param arr - vector containing node array.
	 */
	void toRenderArray(vector<Node*>& arr);

	/**
	 * Adds this and all children scene nodes
	 * that are part of GUI to specified vector.
	 * @param arr - vector containing node array.
	 */
	void toGUIArray(vector<Node*>& arr);

	/**
	 * @param resource - pointer to resource.
	 */
	void addResource(Resource* resource);

	/**
	 * @param type - resource type.
	 * @return Pointer to resource, or 0 if resource not found.
	 */
	Resource* getResource(int type);

	/**
	 * @param type - resource type.
	 * @return Number of specified type resources.
	 */
	SIZE getResourceCount(int type);

	/**
	 * @return Number of all resources.
	 */
	SIZE getResourceCount();

	/**
	 * Returns node resources of specified type.
	 * If no type is specified, all resources will be returned.
	 * @param type - resource type.
	 * @return Vector containing pointers to resources
	 * of specified type.
	 */
	vector<Resource*> getResources(int type = -1);

	/**
	 * @param type - resource type.
	 * @return True if resource with
	 * specified type was found.
	 */
	bool hasResource(int type);

	/**
	 * Prints basic information about node.
	 * @param space - identation string.
	 */
	void print(string space = "");

	/**
	 * Prints brief current and children node
	 * hierarchy with basic information.
	 * @param space - identation string.
	 */
	void printNodes(const string& space);

private:
	string name_;
	vector<Node*> children_;
	Node* parent_;
	Vec3 pos_, scale_, center_;
	Quaternion rot_;
	Mat4 matPos_, matRot_, matScale_, matCombined_;
	vector<Resource*> resources_;
	int states_;
};

#endif /* NODE_H_ */
