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
	Node(string name, Resource* resource);
	~Node();

	const string& getName();
	Resource* getResource();

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
	float* getMatPos();
	float* getMatRot();
	float* getMatScale();
	void setParent(Node* parent);

	/**
	 * @return Pointer to parent node,
	 * or pointer to zero if no parent
	 * was specified.
	 */
	Node* getParent();
	void addChild(Node* child);

	/**
	 * @param child - child to be removed.
	 * @param deleteNode - set true to also delete node and it's
	 * childrend completely.
	 */
	void removeChild(Node* child, bool deleteNode);

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
	void print(string ident = "");
	void printNodes(string ident = "");

private:
	string name;
	Resource* resource;
	vector<Node*> children;
	Node* parent_;
	Vec3 pos_, scale_, center_;
	Quaternion rot_;
	Mat4 matPos_, matRot_, matScale_, matCombined_;
	int states_;
};

#endif /* NODE_H_ */
