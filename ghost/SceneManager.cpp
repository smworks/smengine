/*
 * SceneManager.cpp
 *
 *  Created on: 2014.01.31
 *      Author: Martynas Šustavičius
 */

#include "SceneManager.h"
#include "Multiplatform/ServiceLocator.h"
#include "Resources/Texture.h"
#include "Resources/Sprite.h"
#include "ResourceManager.h"
#include "Camera.h"
#include "Node.h"

SceneManager::SceneManager(ServiceLocator* services) :
	services_(services), target_(0), sceneHeight_(0)
{
	LOGD("Created scene manager.");
	Background bckg = {};
	fill(backgrounds_, backgrounds_ + MAX_BACKGROUND_COUNT, bckg);
}

SceneManager::~SceneManager() {
	LOGD("Deleted scene manager.");
}

void SceneManager::update(double delta) {
	if (target_ == 0 || backgrounds_[0].sprite == 0) {
		return;
	}
	Vec3& camPos = services_->getCamera()->getPos();
	Vec3& targetPos = target_->getPos();
	float halfScreenWidth = services_->getScreenWidth() * 0.5f;
	if (targetPos.getX() <= halfScreenWidth) {
		camPos.setX(0);
	}
	else if (targetPos.getX() > backgrounds_[0].sprite->getScale().getX() - halfScreenWidth) {
		camPos.setX(backgrounds_[0].sprite->getScale().getX() - halfScreenWidth * 2.0f);
	}
	else {
		camPos.setX(targetPos.getX() - halfScreenWidth);
	}
	for (SIZE i = 0; i < MAX_BACKGROUND_COUNT; i++) {
		Node* sprite = backgrounds_[i].sprite;
		if (sprite != 0) {
			if (sceneHeight_ > 0) {
				float heightRatio = (float) services_->getScreenHeight() / (float) sceneHeight_;
				Sprite* spriteRes = static_cast<Sprite*>(sprite->getResource(Resource::SPRITE));
				Texture* texture = spriteRes->getPointerToTexture();
				sprite->getScale().setX(texture->getWidth() * heightRatio);
				sprite->getScale().setY(texture->getHeight() * heightRatio);
			}
			sprite->getPos().setX(sprite->getScale().getX() * 0.5f);
			sprite->getPos().setY(sprite->getScale().getY() * 0.5f);
		}
		if (sprite != 0 && fabs(backgrounds_[i].scrollRatio - 1.0f) > GHOST_DELTA) {
			sprite->getPos().setX(camPos.getX() * backgrounds_[i].scrollRatio
				+ sprite->getScale().getX() * 0.5f);
		}
	}
}

void SceneManager::setBackground(Node* sprite, SIZE index, float scrollRatio) {
	ASSERT(index < MAX_BACKGROUND_COUNT,
		"Attempt to add background with index higher than: %d.", MAX_BACKGROUND_COUNT - 1);
	backgrounds_[index].sprite = sprite;
	backgrounds_[index].scrollRatio = scrollRatio;
}

void SceneManager::setTarget(Node* target) {
	target_ = target;
}

void SceneManager::setSceneHeight(SIZE height) {
	sceneHeight_ = height;
}

float SceneManager::getBackgroundWidth() {
	if (backgrounds_[0].sprite == 0) {
		LOGE("No main background specified for scene.");
		return 0;
	}
	return backgrounds_[0].sprite->getScale().getX();
}

float SceneManager::getBackgroundHeight() {
	if (backgrounds_[0].sprite == 0) {
		LOGE("No main background specified for scene.");
		return 0;
	}
	return backgrounds_[0].sprite->getScale().getY();
}