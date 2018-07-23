#include "MediumScene.h"
#include "network\HttpClient.h"
#include "json\document.h"
#include "string.h"

#define database UserDefault::getInstance()

using namespace cocos2d::network;

USING_NS_CC;

cocos2d::Scene * MediumScene::createScene() {
	auto scene = Scene::createWithPhysics();
	//debug
	//scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);

	scene->getPhysicsWorld()->setAutoStep(true);
	auto layer = MediumScene::create();
	scene->addChild(layer);
	return scene;
}

bool MediumScene::init() {
	if (!Layer::init()) {
		return false;
	}

	auto visibleSize = Director::getInstance()->getVisibleSize();
	auto origin = Director::getInstance()->getVisibleOrigin();

	// 添加背景
	auto bg = Sprite::create("background.jpg");
	bg->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
	this->addChild(bg, 0);

	//添加龙
	dragon = new Dragon(DRAGONBLUE, Vec2(100, visibleSize.height / 2));
	this->addChild(dragon->get());
	//添加柱子
	barrier = new Barrier(4, 1, this);

	// 获取历史最高分数
	if (!database->getBoolForKey("isExist")) {
		database->setBoolForKey("isExist", true);
	}
	int best = database->getIntegerForKey("mediumbest");

	// 添加分数
	auto yBest = Label::createWithTTF("Best Score: ", "fonts/arial.ttf", 28);
	yBest->setPosition(Vec2(origin.x + visibleSize.width - 120, origin.y + visibleSize.height - 30));
	this->addChild(yBest, 4);

	Best = Label::createWithTTF(Value(best).asString(), "fonts/arial.ttf", 28);
	Best->setPosition(Vec2(origin.x + visibleSize.width - 30, origin.y + visibleSize.height - 30));
	this->addChild(Best, 4);

	auto yScore = Label::createWithTTF("Your Score: ", "fonts/arial.ttf", 28);
	yScore->setPosition(Vec2(origin.x + visibleSize.width - 120, origin.y + visibleSize.height - 60));
	this->addChild(yScore, 4);

	Score = Label::createWithTTF("0", "fonts/arial.ttf", 28);
	Score->setPosition(Vec2(origin.x + visibleSize.width - 30, origin.y + visibleSize.height - 60));
	this->addChild(Score, 4);

	// 添加监听器
	addListener();

	// 调度器
	schedule(schedule_selector(MediumScene::checkAll), 0.04f, kRepeatForever, 0);

	//游戏开始
	dragon->begin(false);
	barrier->begin();

	return true;
}

void MediumScene::addListener() {
	// 添加键盘事件监听器
	auto Keyboardlistener = EventListenerKeyboard::create();
	Keyboardlistener->onKeyPressed = CC_CALLBACK_2(MediumScene::onKeyPressed, this);
	Keyboardlistener->onKeyReleased = CC_CALLBACK_2(MediumScene::onKeyReleased, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(Keyboardlistener, this);
	// 添加碰撞监听器
	auto contactListener = EventListenerPhysicsContact::create();
	contactListener->onContactBegin = CC_CALLBACK_1(MediumScene::onConcactBegin, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(contactListener, this);
}

void MediumScene::onKeyPressed(EventKeyboard::KeyCode code, Event* event) {
	switch (code) {
	case EventKeyboard::KeyCode::KEY_UP_ARROW:
	case EventKeyboard::KeyCode::KEY_CAPITAL_W:
	case EventKeyboard::KeyCode::KEY_W:
		movekey = 'W';
		isMove = true;
		break;
	case EventKeyboard::KeyCode::KEY_DOWN_ARROW:
	case EventKeyboard::KeyCode::KEY_CAPITAL_S:
	case EventKeyboard::KeyCode::KEY_S:
		movekey = 'S';
		isMove = true;
		break;
	default:
		break;
	}
}

void MediumScene::onKeyReleased(EventKeyboard::KeyCode code, Event* event) {
	switch (code) {
	case EventKeyboard::KeyCode::KEY_UP_ARROW:
	case EventKeyboard::KeyCode::KEY_W:
	case EventKeyboard::KeyCode::KEY_CAPITAL_W:
		if (movekey == 'W') {
			dragon->stop();
			isMove = false;
		}
		break;
	case EventKeyboard::KeyCode::KEY_DOWN_ARROW:
	case EventKeyboard::KeyCode::KEY_S:
	case EventKeyboard::KeyCode::KEY_CAPITAL_S:
		if (movekey == 'S') {
			dragon->stop();
			isMove = false;
		}
		break;
	default:
		break;
	}
}


void MediumScene::checkAll(float f) {
	// check if the dragon pass a barrier, if so, get one point
	if(barrier->check(dragon->get()->getPosition())) {
		score++;
		char str[10];
		sprintf(str, "%d", score);
		Score->setString(str);
	}
	dragon->check();

	if (isMove) {
		this->move(movekey);
	}
}

void MediumScene::move(char c) {
	switch (c) {
	case 'W':
		dragon->move(1);
		break;
	case 'S':
		dragon->move(-1);
		break;
	default:
		break;
	}
}

bool MediumScene::onConcactBegin(PhysicsContact & contact) {
	barrier->end();
	dragon->die();
	_eventDispatcher->removeAllEventListeners();
	unschedule(schedule_selector(MediumScene::checkAll));
	delete barrier;
	delete dragon;

	gameover();
	// 记录分数
	if (database->getIntegerForKey("mediumbest") < score) {
		database->setIntegerForKey("mediumbest", score);
	}

	return true;
}

void MediumScene::gameover() {
	// gameover提示
	auto visibleSize = Director::getInstance()->getVisibleSize();
	Sprite* gameover = Sprite::create("gameover.png");
	gameover->setPosition(visibleSize.width / 2, visibleSize.height / 2);
	this->addChild(gameover, 3);

	auto restart = MenuItemFont::create("Restart", [](Ref *pSender) {
		Director::getInstance()->replaceScene(TransitionCrossFade::create(0.8, MediumScene::createScene
		()));
	});
	if (restart) {
		restart->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2 - restart->getContentSize().height / 2 - 100));
	}

	auto back = MenuItemFont::create("Back", [](Ref *pSender) {
		Director::getInstance()->replaceScene(TransitionCrossFade::create(0.8, MenuScene::createScene
		()));
	});
	if (back) {
		back->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2 - restart->getContentSize().height / 2 - back->getContentSize().height - 120));
	}
	auto menu = Menu::create(restart, back, NULL);
	menu->setPosition(Vec2::ZERO);
	this->addChild(menu, 1);
}
