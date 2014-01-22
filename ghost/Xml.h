/*
 * Xml.h
 *
 *  Created on: 2012.06.22
 *      Author: MS
 */

#ifndef XML_H_
#define XML_H_

#include "Multiplatform/Ghost.h"

struct Attribute {
	Attribute(string name, string value) :
		name_(name), value_(value) {}
	string name_;
	string value_;
};

struct Tag {
	Tag(int type, string name, SIZE pos,
		vector<Attribute>* attributes) :
			type_(type),
			name_(name),
			pos_(pos),
			attributes_(attributes)
	{
		//LOGI("TAG: type=%d, name=%s, pos=%d, attrCount=%d",
		//	type, name.c_str(), pos, attributes == 0 ? 0 : attributes->size());
	}
	Tag(Tag& tag) {
		type_ = tag.type_;
		name_ = tag.name_;
		pos_ = tag.pos_;
		attributes_ =
			tag.attributes_ == 0 ? 0 : NEW vector<Attribute>(*(tag.attributes_));
	}
	~Tag() {
		delete attributes_;
	}
	enum {
		START, END, EMPTY, STANDALONE
	};
	int type_;
	string name_;
	SIZE pos_; // Starting position of tag.
	vector<Attribute>* attributes_;

	/**
	 * Returns last tag symbol index.
	 */
	SIZE endPos() {
		return pos_ + name_.length() + (type_ == Tag::START ? 1 : 2);
	}
};

/**
 * Class holds basic XML elements.
 * Tag attribute is used to indicate XML tag name and content is used to refer to the text
 * between starting and ending tags.
 */
class XmlNode {
public:
	Tag* tag_;
	string content_;
	vector<XmlNode*>* children_;
public:
	XmlNode(Tag* tag, string content,
		vector<XmlNode*>* children) :
			tag_(tag),
			content_(content),
			children_(children)
	{
	}

	/**
	 * @param tag - XML tag whose content to look for.
	 * @return Node object if found, 0 otherwise.
	 */
	XmlNode* getNode(const string& tag) {
		if (children_ != 0 && children_->size() > 0) {
			for (vector<XmlNode*>::const_iterator it = children_->begin(); it != children_->end(); ++it) {
				if ((*it)->tag_->name_ == tag) {
					return (*it);
				}
			}
		}
		return 0;
	}

	/**
	 * Returns vector with children nodes that contain specified tag.
	 * @param tag - XML tag to look for.
	 */
	vector<XmlNode*> getNodes(const string& tag) {
		vector<XmlNode*> nodes;
		if (children_ != 0) {
			for (vector<XmlNode*>::const_iterator it = children_->begin();
					it != children_->end(); ++it)
			{
				if ((*it)->tag_->name_ == tag) {
					nodes.push_back(*it);
				}
			}
		}
		return nodes;
	}

	/**
	 * Returns number of children nodes that contain specified tag.
	 * @param tag - XML tag to look for.
	 */
	int countNodes(const string& tag) {
		int count = 0;
		if (children_ != 0 && children_->size() > 0) {
			for (vector<XmlNode*>::const_iterator it = children_->begin();
					it != children_->end(); ++it)
			{
				if ((*it)->tag_->name_ == tag) {
					count++;
				}
			}
		}
		return count;
	}

	/**
	 * Returns text contained between specified tags.
	 * @param tag - XML tag containing text.
	 * @return string containing tag content.
	 */
	string* getContent(const string& tag) {
		XmlNode* n = getNode(tag);
		if (n != 0 && n->content_.length() > 0) {
			return &n->content_;
		}
		return 0;
	}

	/**
	 * Returns attribute value.
	 * @param attr - attribute name.
	 * @return string containing attribute value.
	 */
	string* getAttr(const string& attr) {
		vector<Attribute>* values = tag_->attributes_;
		if (values == 0) {
			return 0;
		}
		for (UINT32 i = 0; i < values->size(); i++)
		{
			Attribute* at = &(*values)[i];
			if (attr == at->name_) {
				return &(at->value_);
			}
		}
		return 0;
	}
};

class Xml {
public:
	/**
	 * @param xml - pointer to string containing XML data.
	 */
	Xml(const string& xml);
	~Xml();

	/**
	 * @return Pointer to root node.
	 */
	XmlNode* getRootNode();

	/**
	 * Prints node hierarchy.
	 * @param node - root node.
	 * @param space - symbol sequence before outputing information.
	 */
	static void printNodes(XmlNode* node, string space);
private:
	/**
	 * Parses XML document int separate nodes.
	 * @param xml - pointer to string containing XML data.
	 * @param i - index position.
	 * @return pointer to XmlNode object.
	 */
	XmlNode* parseNode(const string& xml, SIZE& i);

	/**
	* Finds first tag from specified index position.
	* @param xml -> pointer to string containing XML data.
	* @param i -> index position.
	* @return Pointer to Tag object.
	*/
	Tag* findTag(const string& xml, SIZE i);

	/**
	 * @param node - pointer to root node.
	 */
	void deleteNodes(XmlNode* node);
private:
	XmlNode* rootNode_;
};

#endif /* XML_H_ */
