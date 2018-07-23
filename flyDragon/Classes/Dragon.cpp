#include "Dragon.h"

Dragon::Dragon(int color, Vec2 position)
{
	char *name = color == DRAGONBLUE ? "bluedra_fly.png" : "blackdra_fly.png";
	auto texture = Director::getInstance()->getTextureCache()->addImage(name);
	flying.reserve(8);
	for (int i = 0; i < 7; ++i) {
		auto frame = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(100 * i, 0, 100, 100)));
		flying.pushBack(frame);
	}
	auto animation = Animation::createWithSpriteFrames(flying, 0.1f);
	auto animate = Animate::create(animation);

	dragon = Sprite::createWithSpriteFrame(flying.at(0));

	dragon->setPosition(position);
	dragon->setFlippedX(true);
	dragon->runAction(RepeatForever::create(animate));

	auto body = PhysicsBody::createBox(Size(40, 90), PhysicsMaterial(100.0f, 0.0f, 1.0f));
	body->setDynamic(false);
	body->setContactTestBitmask(0xFFFFFFFF);
	body->setCollisionBitmask(0xFFFFFFFF);
	body->setContactTestBitmask(0xFFFFFFFF);
	body->setGroup(1);
	dragon->setPhysicsBody(body);
}

void Dragon::jump()
{
	dragon->getPhysicsBody()->setVelocity(Vec2(0, 150));
}

void Dragon::die()
{
	dragon->getPhysicsBody()->setVelocity(Vec2::ZERO);
	dragon->stopAllActions();
}

void Dragon::move(int dir)
{
	dragon->getPhysicsBody()->setVelocity(Vec2(0, dir * 100));
}

void Dragon::stop()
{
	dragon->getPhysicsBody()->setVelocity(Vec2::ZERO);
}

Sprite * Dragon::get()
{
	return dragon;
}

void Dragon::begin(bool gravityEnabled)
{
	if (gravityEnabled) {
		dragon->getPhysicsBody()->setDynamic(true);
	}
}

void Dragon::check()
{
	if (dragon->getBoundingBox().getMaxY() > Director::getInstance()->getVisibleSize().height
		|| dragon->getBoundingBox().getMinY() < 0) {
		dragon->getPhysicsBody()->setVelocity(Vec2::ZERO);
	}
}
