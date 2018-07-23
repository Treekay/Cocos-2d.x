#include "MenuScene.h"
#include "GameScene.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;

Scene* MenuScene::createScene()
{
	auto scene = Scene::create();
	auto layer = MenuScene::create();
	scene->addChild(layer);
	return scene;
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
	printf("Error while loading: %s\n", filename);
	printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool MenuScene::init()
{
	//////////////////////////////
	// 1. super init first
	if (!Scene::init())
	{
		return false;
	}

	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	auto bg_sky = Sprite::create("menu-background-sky.jpg");
	bg_sky->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y + 150));
	this->addChild(bg_sky, 0);

	auto bg = Sprite::create("menu-background.png");
	bg->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y - 60));
	this->addChild(bg, 0);

	auto cave = Sprite::createWithSpriteFrameName("cave-0.png");
	Animate* caveAnimate = Animate::create(AnimationCache::getInstance()->getAnimation("caveAnimation"));
	cave->runAction(RepeatForever::create(caveAnimate));
	cave->setPosition(Vec2(visibleSize.width / 2 + origin.x + 190, visibleSize.height / 2 + origin.y - 40));
	this->addChild(cave, 1);

	auto miner = Sprite::create("menu-miner.png");
	miner->setPosition(Vec2(150 + origin.x, visibleSize.height / 2 + origin.y - 60));
	this->addChild(miner, 1);

	auto leg = Sprite::createWithSpriteFrameName("miner-leg-0.png");
	Animate* legAnimate = Animate::create(AnimationCache::getInstance()->getAnimation("legAnimation"));
	leg->runAction(RepeatForever::create(legAnimate));
	leg->setPosition(110 + origin.x, origin.y + 102);
	this->addChild(leg, 1);

	auto face = Sprite::createWithSpriteFrameName("miner-face-normal.png");
	Animate* faceAnimate = Animate::create(AnimationCache::getInstance()->getAnimation("faceAnimation"));
	face->runAction(RepeatForever::create(faceAnimate));
	face->setPosition(163 + origin.x, origin.y + 366);
	this->addChild(face, 1);

	auto stone = Sprite::create("menu-start-gold.png");
	stone->setPosition(Vec2(origin.x + visibleSize.width - 180, visibleSize.height / 2 + origin.y - 180));
	this->addChild(stone, 1);

	auto start = MenuItemImage::create("start-0.png", "start-1.png", CC_CALLBACK_1(MenuScene::startMenuCallback, this));
	start->setPosition(Vec2(origin.x + visibleSize.width - 170, visibleSize.height / 2 + origin.y - 130));

	auto menu = Menu::create(start, NULL);
	menu->setPosition(Vec2::ZERO);
	this->addChild(menu, 2);

	auto title = Sprite::create("gold-miner-text.png");
	title->setPosition(Vec2(460, 480));
	this->addChild(title);

	return true;
}

void MenuScene::startMenuCallback(Ref * pSender) {
	auto gameScene = GameScene::createScene();
	Director::getInstance()->replaceScene(TransitionFlipY::create(1, gameScene));
}