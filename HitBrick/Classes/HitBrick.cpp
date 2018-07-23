#pragma execution_character_set("utf-8")
#include "HitBrick.h"
#include "cocos2d.h"
#include "SimpleAudioEngine.h"
#define database UserDefault::getInstance()

USING_NS_CC;
using namespace CocosDenshion;

void HitBrick::setPhysicsWorld(PhysicsWorld* world) { m_world = world; }

Scene* HitBrick::createScene() {
	srand((unsigned)time(NULL));
	auto scene = Scene::createWithPhysics();

	scene->getPhysicsWorld()->setAutoStep(true);

	// Debug 模式
	//scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
	scene->getPhysicsWorld()->setGravity(Vec2(0, -300.0f));
	auto layer = HitBrick::create();
	layer->setPhysicsWorld(scene->getPhysicsWorld());
	layer->setJoint();
	scene->addChild(layer);
	return scene;
}

// on "init" you need to initialize your instance
bool HitBrick::init() {
	//////////////////////////////
	// 1. super init first
	if (!Layer::init()) {
	return false;
	}
	visibleSize = Director::getInstance()->getVisibleSize();

	auto edgeSp = Sprite::create();  //创建一个精灵
	auto boundBody = PhysicsBody::createEdgeBox(visibleSize, PhysicsMaterial(0.0f, 1.0f, 0.0f), 3);  //edgebox是不受刚体碰撞影响的一种刚体，我们用它来设置物理世界的边界
	edgeSp->setPosition(visibleSize.width / 2, visibleSize.height / 2);  //位置设置在屏幕中央
	edgeSp->setPhysicsBody(boundBody);
	addChild(edgeSp);

	preloadMusic(); // 预加载音效

	addSprite();    // 添加背景和各种精灵
	addListener();  // 添加监听器 
	addPlayer();    // 添加球与板
	BrickGeneraetd();  // 生成砖块

	schedule(schedule_selector(HitBrick::update), 0.01f, kRepeatForever, 0.1f);

	onBall = true;
	isMove = false;

	spFactor = 0;
	return true;
}

// 关节连接，固定球与板子
// Todo
void HitBrick::setJoint() {
	joint1 = PhysicsJointDistance::construct(player->getPhysicsBody(), ball->getPhysicsBody(), Vec2(0 - player->getContentSize().width*0.1f/2, player->getContentSize().height*0.1f/2), Vec2(0, 0));
	joint1->setEnable(true);
	m_world->addJoint(joint1);
	joint2 = PhysicsJointDistance::construct(player->getPhysicsBody(), ball->getPhysicsBody(), Vec2(player->getContentSize().width*0.1f/2, player->getContentSize().height*0.1f / 2), Vec2(0, 0));
	joint2->setEnable(true);
	m_world->addJoint(joint2);
}



// 预加载音效
void HitBrick::preloadMusic() {
	auto sae = SimpleAudioEngine::getInstance();
	sae->preloadEffect("gameover.mp3");
	sae->preloadBackgroundMusic("bgm.mp3");
	sae->playBackgroundMusic("bgm.mp3", true);
}

// 添加背景和各种精灵
void HitBrick::addSprite() {
	// add background
	auto bgSprite = Sprite::create("bg.png");
	bgSprite->setPosition(visibleSize / 2);
	bgSprite->setScale(visibleSize.width / bgSprite->getContentSize().width, visibleSize.height / bgSprite->getContentSize().height);
	this->addChild(bgSprite, 0);


	// add ship
	ship = Sprite::create("ship.png");
	ship->setScale(visibleSize.width / ship->getContentSize().width * 0.97, 1.2f);
	ship->setPosition(visibleSize.width / 2, 0);
	auto shipbody = PhysicsBody::createBox(ship->getContentSize(), PhysicsMaterial(100.0f, 0.0f, 1.0f));
	shipbody->setCategoryBitmask(0xFFFFFFFF);
	shipbody->setCollisionBitmask(0xFFFFFFFF);
	shipbody->setContactTestBitmask(0xFFFFFFFF);
	shipbody->setTag(1);
	shipbody->setDynamic(false);  // ??????岻?????????, ????????????б??
	ship->setPhysicsBody(shipbody);
	this->addChild(ship, 1);

	// add sun and cloud
	auto sunSprite = Sprite::create("sun.png");
	sunSprite->setPosition(rand() % (int)(visibleSize.width - 200) + 100, 550);
	this->addChild(sunSprite);
	auto cloudSprite1 = Sprite::create("cloud.png");
	cloudSprite1->setPosition(rand() % (int)(visibleSize.width - 200) + 100, rand() % 100 + 450);
	this->addChild(cloudSprite1);
	auto cloudSprite2 = Sprite::create("cloud.png");
	cloudSprite2->setPosition(rand() % (int)(visibleSize.width - 200) + 100, rand() % 100 + 450);
	this->addChild(cloudSprite2);
}

// 添加监听器
void HitBrick::addListener() {
	auto keyboardListener = EventListenerKeyboard::create();
	keyboardListener->onKeyPressed = CC_CALLBACK_2(HitBrick::onKeyPressed, this);
	keyboardListener->onKeyReleased = CC_CALLBACK_2(HitBrick::onKeyReleased, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);

	auto contactListener = EventListenerPhysicsContact::create();
	contactListener->onContactBegin = CC_CALLBACK_1(HitBrick::onConcactBegin, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(contactListener, this);
}

// 创建角色
void HitBrick::addPlayer() {

	player = Sprite::create("bar.png");
	int xpos = visibleSize.width / 2;

	player->setScale(0.1f, 0.1f);
	player->setPosition(Vec2(xpos, ship->getContentSize().height - player->getContentSize().height*0.1f - 5));
	// 设置板的刚体属性
	// Todo
	player->setPhysicsBody(PhysicsBody::createBox(player->getContentSize(), PhysicsMaterial(0, 1, 0)));
	player->getPhysicsBody()->setDynamic(false);
	player->getPhysicsBody()->setCategoryBitmask(0x02);
	player->getPhysicsBody()->setCollisionBitmask(0x03);
	player->getPhysicsBody()->setContactTestBitmask(0x03);

	this->addChild(player, 2);
  
	ball = Sprite::create("ball.png");
	ball->setPosition(Vec2(xpos, player->getPosition().y + player->getContentSize().height*0.1f + ball->getContentSize().height*0.1f/2 - 5));
	ball->setScale(0.1f, 0.1f);
	// 设置球的刚体属性
	// Todo
	ball->setPhysicsBody(PhysicsBody::createCircle(ball->getContentSize().width/2, PhysicsMaterial(0, 1, 0)));
	ball->getPhysicsBody()->setGravityEnable(false);
	ball->getPhysicsBody()->setDynamic(true);
	ball->getPhysicsBody()->setCategoryBitmask(0x03);
	ball->getPhysicsBody()->setCollisionBitmask(0x03);
	ball->getPhysicsBody()->setContactTestBitmask(0x03);

	addChild(ball, 3);
  
}

// 实现简单的蓄力效果
// Todo
void HitBrick::update(float dt) {
	if (isMove) {
		if (dir == 'a') {
			player->getPhysicsBody()->setVelocity(Vec2(-500, 0));
		}
		else if (dir == 'd') {
			player->getPhysicsBody()->setVelocity(Vec2(500, 0));
		}
	}
	else {
		player->getPhysicsBody()->setVelocity(Vec2(0, 0));
	}
	if (spHolded && onBall == true) {
		if (spFactor < 100) spFactor++;
		auto power = ParticleSystemQuad::create("particle_power.plist");
		power->setPosition(player->getPosition());
		power->setScale(0.2);
		power->setDuration(0.1);
		power->setAutoRemoveOnFinish(true);
		this->addChild(power);
	}
	else if (spHolded == false && onBall == true && spFactor != 0) {
		onBall = false;
		joint1->setEnable(false);
		joint2->setEnable(false);
		int randomX = 0;
		while (randomX == 0) {
			randomX = random() % 400 - 200;
		}
		ball->getPhysicsBody()->setVelocity(Vec2(randomX + spFactor*3, 200 + spFactor*3));
		ball->getPhysicsBody()->setVelocityLimit(800);
		ball->getPhysicsBody()->setAngularVelocity(1.5);
	}
	if (player->getBoundingBox().getMaxX() > visibleSize.width - 30 && dir == 'd'|| 
		player->getBoundingBox().getMinX() < 30 && dir == 'a') {
		player->getPhysicsBody()->setVelocity(Vec2(0, 0));
	}
	if (onBall == false) {
		auto fly = ParticleSystemQuad::create("particle_fly.plist");
		fly->setPosition(ball->getPosition());
		fly->setDuration(0.05);
		fly->setScale(0.5);
		fly->setAutoRemoveOnFinish(true);
		this->addChild(fly);
	}
}




// Todo
void HitBrick::BrickGeneraetd() {

	for (int i = 0; i < 3; i++) {
		int cw = 0;
		while (cw <= visibleSize.width) {
			auto box = Sprite::create("box.png");
			// 为砖块设置刚体属性
			// Todo
			auto physicsBody = PhysicsBody::createBox(box->getContentSize(), PhysicsMaterial(0, 1, 0));
			physicsBody->setDynamic(false);
			box->setPhysicsBody(physicsBody);
			box->getPhysicsBody()->setCategoryBitmask(0x01);
			box->getPhysicsBody()->setCollisionBitmask(0x03);
			box->getPhysicsBody()->setContactTestBitmask(0x03);
			box->setTag(3);
			box->setPosition(Vec2(cw + 20, visibleSize.height - box->getContentSize().height*i - 14));
			this->addChild(box);
			cw += box->getContentSize().width;
		}

	}

}


// 左右
void HitBrick::onKeyPressed(EventKeyboard::KeyCode code, Event* event) {

	switch (code) {
	case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
		dir = 'a';
		isMove = true;
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
		// 左右移动
		// Todo
		dir = 'd';
		isMove = true;
		break;

	case cocos2d::EventKeyboard::KeyCode::KEY_SPACE: 
		// 开始蓄力
		spHolded = true;
		break;
	default:
		break;
	}
}

// 释放按键
void HitBrick::onKeyReleased(EventKeyboard::KeyCode code, Event* event) {
	switch (code) {
	case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
		if (dir == 'a')
			isMove = false;
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
		// 停止运动
		// Todo
		if (dir == 'd')
			isMove = false;
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_SPACE:   
		// 蓄力结束，小球发射
		spHolded = false;
		break;

	default:
		break;
	}
}

// 碰撞检测
// Todo
bool HitBrick::onConcactBegin(PhysicsContact & contact) {
	auto c1 = contact.getShapeA(), c2 = contact.getShapeB();
	if (c1->getCategoryBitmask() == 0x03 && c2->getCollisionBitmask() == 0x03) {
		if (c2->getBody()->getNode() != nullptr && c2->getBody()->getNode()->getTag() == 3) {
			auto explosion = ParticleSystemQuad::create("particle_explosion.plist");
			explosion->setPosition(c2->getBody()->getNode()->getPosition());
			explosion->setScale(0.4);
			explosion->setAutoRemoveOnFinish(true);
			addChild(explosion);
			c2->getBody()->getNode()->removeFromParentAndCleanup(true);
			ball->getPhysicsBody()->setVelocity(Vec2(ball->getPhysicsBody()->getVelocity().x + 20, ball->getPhysicsBody()->getVelocity().y + 20));
		}
	}
	else if (c1->getCategoryBitmask() == 0x03 && c2->getCollisionBitmask() == 0xFFFFFFFF) {
		GameOver();
	}
	return true;
}


void HitBrick::GameOver() {

	_eventDispatcher->removeAllEventListeners();
	ball->getPhysicsBody()->setVelocity(Vec2(0, 0));
	ball->getPhysicsBody()->setAngularVelocity(0);
	player->getPhysicsBody()->setVelocity(Vec2(0, 0));
	isMove = false;
	SimpleAudioEngine::getInstance()->stopBackgroundMusic("bgm.mp3");
	SimpleAudioEngine::getInstance()->playEffect("gameover.mp3", false);

	auto label1 = Label::createWithTTF("Game Over~", "fonts/STXINWEI.TTF", 60);
	label1->setColor(Color3B(0, 0, 0));
	label1->setPosition(visibleSize.width / 2, visibleSize.height / 2);
	this->addChild(label1);

	auto label2 = Label::createWithTTF("重玩", "fonts/STXINWEI.TTF", 40);
	label2->setColor(Color3B(0, 0, 0));
	auto replayBtn = MenuItemLabel::create(label2, CC_CALLBACK_1(HitBrick::replayCallback, this));
	Menu* replay = Menu::create(replayBtn, NULL);
	replay->setPosition(visibleSize.width / 2 - 80, visibleSize.height / 2 - 100);
	this->addChild(replay);

	auto label3 = Label::createWithTTF("退出", "fonts/STXINWEI.TTF", 40);
	label3->setColor(Color3B(0, 0, 0));
	auto exitBtn = MenuItemLabel::create(label3, CC_CALLBACK_1(HitBrick::exitCallback, this));
	Menu* exit = Menu::create(exitBtn, NULL);
	exit->setPosition(visibleSize.width / 2 + 90, visibleSize.height / 2 - 100);
	this->addChild(exit);
}

// 继续或重玩按钮响应函数
void HitBrick::replayCallback(Ref * pSender) {
	Director::getInstance()->replaceScene(HitBrick::createScene());
}

// 退出
void HitBrick::exitCallback(Ref * pSender) {
	Director::getInstance()->end();
	#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
		exit(0);
	#endif
}
