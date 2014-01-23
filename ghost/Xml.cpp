/*
 * Xml.cpp
 *
 *  Created on: 2012.06.22
 *      Author: MS
 */

#include "Xml.h"

Xml::Xml(const string& xml) {
	SIZE i = 0;
	rootNode_ = parseNode(xml, i);
}

Xml::~Xml() {
	if (rootNode_ != 0) {
		deleteNodes(rootNode_);
	}
}

XmlNode* Xml::getRootNode() {
	return rootNode_;
}

XmlNode* Xml::parseNode(const string& xml, SIZE& i) {
    if (xml.length() == 0) {
        LOGE("Attempt to parse empty string.");
        return 0;
    }
	Tag* first = findTag(xml, i);
	if (first->type_ == Tag::END) {
		LOGE("Unexpected end tag started at position %u.", (UINT32) first->pos_);
		return 0;
	}
	i = first->endPos() + 1;
	vector<XmlNode*>* children = NEW vector<XmlNode*>();
	bool run = true;
	bool hasContent = true;
	bool fail = false;
	string content = "";
	while (run) {
		Tag* second = findTag(xml, i);
		if (i >= xml.length()) { // Index exceeded file length
			run = false;
			fail = true;
		}
		else if (second == 0) { // Unable to find second tag.
			run = false;
			fail = true;
		}
		else if (second->type_ == Tag::STANDALONE) { // Tag is a standalone child.
			XmlNode* tmp = NEW XmlNode(NEW Tag(*second), "", 0);
			children->push_back(tmp);
			i = second->endPos();
		}
		else if (second->type_ == Tag::END && first->name_ == second->name_) { // Found end tag.
			if (hasContent) {
				SIZE pos = first->pos_ + first->name_.length() + 2;
				content = xml.substr(pos, second->pos_ - pos);
			}
			i = second->endPos();
			run = false;
		}
		else if (second->type_ == Tag::START) { // Found tag indicating child node.
			XmlNode* node = parseNode(xml, i);
			if (node == 0) {
				run = false;
				fail = true;
			}
			else {
				children->push_back(node);
			}
		}
		else { // Unknown error.
			LOGW("Type: %d", second->type_);
			LOGW("Text: %s",
				string(xml, first->pos_, xml.length() - first->pos_ - 1).c_str());
			LOGE("Unknown error occurred while parsing nodes."
				" Make sure XML is well formated."
				" First tag: %s, second tag: %s.",
				first->name_.c_str(), second->name_.c_str());
			fail = true;
			run = false;
		}
		hasContent = false;
		delete second;
	}

	XmlNode* node = NEW XmlNode(first, content, children);
	if (fail) {
		deleteNodes(node);
		node = 0;
	}
	return node;
}

Tag* Xml::findTag(const string& xml, SIZE i) {
	bool search = true;
	SIZE start = 0;
	SIZE end = 0;
	vector<Attribute>* attr = 0;
	while (search) {
		start = xml.find('<', i);
		SIZE quote = xml.find('"', start);
		end = xml.find('>', start);
		if (quote < end && xml[start + 1] != '/') {
			SIZE tempStart = start;
			while (true) {
				end = xml.find('>', tempStart);
				if (xml[end - 1] == '"' ||
					xml[end - 1] == '/' || xml[end - 1] == '?')
				{
					break;
				}
				tempStart = end + 1;
			}
		}
		if (xml[start + 1] == '?') {
			i = end + 1;
		}
		else if (start == string::npos || end == string::npos) {
			LOGW("Tag not found.");
			return 0;
		}
		else {
			search = false;
		}
	}

	int type;
	string name;
	if (xml[start + 1] == '/') { // Simple end tag </tag>
		type = Tag::END;
		name = xml.substr(start + 2, end - start - 2);
	}
	else { // Start tag. Could be <tag> or <tag attr="val">
		if (xml[end - 1] == '/') {
			type = Tag::STANDALONE;
		}
		else {
			type = Tag::START;
		}
		SIZE attrPos = xml.find('=', start);
		if (attrPos < end && attrPos != string::npos) {
			attr = NEW vector<Attribute>();
			SIZE ns = xml.find(' ', start);
			name = xml.substr(start + 1, ns - start - 1);
			while (attrPos < end) {
				string attrName = xml.substr(ns + 1, attrPos - ns - 1);
				SIZE ve = xml.find('"', attrPos + 2);
				if (ve > end) {
					LOGE("Badly formatted tag \"%s\".", name.c_str());
				}
				string attrValue = xml.substr(attrPos + 2, ve - attrPos - 2);
				attr->push_back(Attribute(attrName, attrValue));
				attrPos = xml.find('=', ve);
				if (attrPos == string::npos) {
					break;
				}
				ns = xml.find(' ', ve);
			}
		}
		else {
			name = xml.substr(start + 1, end - start - 1);
		}
	}
	i = end;
	Tag* tag = NEW Tag(type, name, start, attr);
	return tag;
}

void Xml::deleteNodes(XmlNode* node) {
	if (node->children_ != 0) {
		for (vector<XmlNode*>::const_iterator it = node->children_->begin();
				it != node->children_->end(); it++) {
			deleteNodes(*it);
		}
		delete node->children_;
		node->children_ = 0;
	}
	delete node->tag_;
	node->tag_ = 0;
	delete node;
	node = 0;
}

void Xml::printNodes(XmlNode* node, string space) {
	if (node->content_.length() > 0) {
		LOGD("%s%s: %s", space.c_str(), node->tag_->name_.c_str(), node->content_.c_str());
	}
	else {
		LOGD("%s%s", space.c_str(), node->tag_->name_.c_str());
	}
	if (node->children_ != 0) {
		for (vector<XmlNode*>::const_iterator it = node->children_->begin();
				it != node->children_->end(); it++) {
			printNodes(*it, space + "  ");
		}
	}
}
