#include "GameScene.h"

USING_NS_CC;

Scene* GameScene::createScene()
{
	return GameScene::create();
}

// on "init" you need to initialize your instance
bool GameScene::init()
{
	//////////////////////////////
	// 1. super init first
	if (!Scene::init())
	{
		return false;
	}

	//add touch listener
	EventListenerTouchOneByOne* listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);
	listener->onTouchBegan = CC_CALLBACK_2(GameScene::onTouchBegan, this);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	auto bg = Sprite::create("level-background-0.jpg");
	bg->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
	this->addChild(bg, 0);

	// Stone
	stone = Sprite::create("stone.png");
	stone->setPosition(Vec2(560, 480));
	// StoneLayer
	stoneLayer = Layer::create();
	stoneLayer->setAnchorPoint(Vec2(0, 0));
	stoneLayer->setPosition(Vec2(0, 0));
	stoneLayer->ignoreAnchorPointForPosition(false);
	stoneLayer->addChild(stone);
	this->addChild(stoneLayer, 1);

	// Mouse
	mouse = Sprite::createWithSpriteFrameName("gem-mouse-0.png");
	mouse->setPosition(Vec2(origin.x + visibleSize.width / 2, 0));
	Animate* mouseAnimate = Animate::create(AnimationCache::getInstance()->getAnimation("mouseAnimation"));
	mouse->runAction(RepeatForever::create(mouseAnimate));
	// MouseLayer
	mouseLayer = Layer::create();
	mouseLayer->setAnchorPoint(Vec2(0, 0));
	mouseLayer->setPosition(Vec2(0, origin.y + visibleSize.height / 2));
	mouseLayer->ignoreAnchorPointForPosition(false);
	mouseLayer->addChild(mouse);
	this->addChild(mouseLayer, 1);

	auto shoot = Label::createWithSystemFont("Shoot", "Marker Felt", 80);
	auto menuItem = MenuItemLabel::create(shoot, CC_CALLBACK_1(GameScene::shootMenuCallback, this));
	menuItem->setPosition(Vec2(visibleSize.width + origin.x - 150, visibleSize.height + origin.y - 140));

	auto menu = Menu::create(menuItem, NULL);
	menu->setAnchorPoint(Vec2(0, 0));
	menu->setPosition(Vec2(0, 0));
	this->addChild(menu, 1);

	return true;
}

void GameScene::mouseChangeDirection(int x) {
	if (mouse->isFlipX() && mouse->getPositionX() < x) {
		mouse->setFlippedX(false);
	}
	else if (mouse->isFlippedX() == false && mouse->getPositionX() > x) {
		mouse->setFlippedX(true);
	}
}

void GameScene::shootMenuCallback(cocos2d::Ref* pSender) {
	if (eating || shooting) return;
	shooting = true;
	Sprite* stone = Sprite::create("stone.png");
	stone->setPosition(Vec2(560, 480));
	stoneLayer->addChild(stone);
	// move the stone to the mouse
	auto location = mouseLayer->convertToWorldSpace(mouse->getPosition());
	auto moveToMouse = CallFunc::create([this, location, stone]() {
		stone->runAction(MoveTo::create(0.5, stoneLayer->convertToNodeSpace(location)));
		// remove the stone when it strike on the mouse
		auto removeStone = CallFunc::create([this,stone]() {
			stone->runAction(FadeOut::create(0.5));
			auto deleteStone = CallFunc::create([this, stone]() {
				stoneLayer->removeChild(stone);
				shooting = false;
			});
			this->runAction(Sequence::create(DelayTime::create(0.5), deleteStone, NULL));
		});
		this->runAction(Sequence::create(DelayTime::create(0.5), removeStone, NULL));
	});
	this->runAction(moveToMouse);

	auto mouseEscape = CallFunc::create([this, location]() {
		// the mouse move to a random position
		int random_x = random(0,960);
		int random_y = random(0,640);
		mouseChangeDirection(random_x);
		auto moveRandom = MoveTo::create(1, mouseLayer->convertToNodeSpace(Vec2(random_x,random_y)));
		mouse->runAction(moveRandom);

		// leave a diamond
		auto diamond = Sprite::create("diamond.png");
		Animate* diamondAnimate = Animate::create(AnimationCache::getInstance()->getAnimation("diamondAnimation"));
		diamond->runAction(RepeatForever::create(diamondAnimate));
		diamond->setPosition(this->convertToNodeSpace(location));
		this->addChild(diamond);
	});
	this->runAction(mouseEscape);
}

bool GameScene::onTouchBegan(Touch *touch, Event *unused_event) {
	if (shooting) return false;
	eating = true;
	// judge if there has a cheese 
	if (this->getChildByTag(1) != NULL) return false;

	// if the touch location valid
	// the touch location put a cheese
	auto location = touch->getLocation();
	mouseChangeDirection(location.x);

	auto cheese = Sprite::create("cheese.png");
	cheese->setPosition(location);
	this->addChild(cheese,0,1);

	// move the mouse to the cheese
	auto moveToCheese = MoveTo::create(2, mouseLayer->convertToNodeSpace(location));
	mouse->runAction(moveToCheese);

	auto removeCheese = CallFunc::create([this, cheese]() {
		cheese->runAction(FadeOut::create(0.5));
		auto deleteCheese = CallFunc::create([this, cheese]() {
			cheese->removeFromParent();
			eating = false;
		});
		this->runAction(Sequence::create(DelayTime::create(0.5), deleteCheese, NULL));
	});
	auto seq = Sequence::create(DelayTime::create(2), removeCheese, NULL);
	this->runAction(seq);

	return true;
}
