#include "GameScene.h"
#include "Monster.h"
#include "sqlite3.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#define database UserDefault::getInstance()
#pragma execution_character_set("utf-8")
USING_NS_CC;

Scene* GameScene::createScene()
{
	return GameScene::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool GameScene::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Scene::init() )
    {
        return false;
    }

    visibleSize = Director::getInstance()->getVisibleSize();
    origin = Director::getInstance()->getVisibleOrigin();

	// ���ñ���
	TMXTiledMap* tmx = TMXTiledMap::create("map.tmx");
	tmx->setPosition(visibleSize.width / 2 , visibleSize.height / 2);
	tmx->setAnchorPoint(Vec2(0.5, 0.5));
	tmx->setScale(Director::getInstance()->getContentScaleFactor());
	this->addChild(tmx, 0);

	//����һ����ͼ
	auto texture = Director::getInstance()->getTextureCache()->addImage("$lucia_2.png");
	//����ͼ�������ص�λ�и�����ؼ�֡
	auto frame0 = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(0, 0, 113, 113)));
	//ʹ�õ�һ֡��������
	player = Sprite::createWithSpriteFrame(frame0);
	player->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2));
	addChild(player, 2);

	//hp��
	Sprite* sp0 = Sprite::create("hp.png", CC_RECT_PIXELS_TO_POINTS(Rect(0, 320, 420, 47)));
	Sprite* sp = Sprite::create("hp.png", CC_RECT_PIXELS_TO_POINTS(Rect(610, 362, 4, 16)));

	//ʹ��hp������progressBar
	pT = ProgressTimer::create(sp);
	pT->setScaleX(90);
	pT->setAnchorPoint(Vec2(0, 0));
	pT->setType(ProgressTimerType::BAR);
	pT->setBarChangeRate(Point(1, 0));
	pT->setMidpoint(Point(0, 1));
	pT->setPercentage(100);
	pT->setPosition(Vec2(origin.x + 14 * pT->getContentSize().width, origin.y + visibleSize.height - 2 * pT->getContentSize().height));
	addChild(pT, 2);
	sp0->setAnchorPoint(Vec2(0, 0));
	sp0->setPosition(Vec2(origin.x + pT->getContentSize().width, origin.y + visibleSize.height - sp0->getContentSize().height));
	addChild(sp0, 1);

	// ��̬����
	idle.reserve(1);
	idle.pushBack(frame0);

	// ��������
	attack.reserve(17);
	for (int i = 0; i < 17; i++) {
		auto frame = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(113 * i, 0, 113, 113)));
		attack.pushBack(frame);
	}

	// ���Է��չ�������
	// ��������(֡����22֡���ߣ�90����79��
	auto texture2 = Director::getInstance()->getTextureCache()->addImage("$lucia_dead.png");
	// Todo
	dead.reserve(22);
	for (int i = 0; i < 22; i++) {
		auto frame = SpriteFrame::createWithTexture(texture2, CC_RECT_PIXELS_TO_POINTS(Rect(79 * i, 0, 90, 79)));
		dead.pushBack(frame);
	}

	// �˶�����(֡����8֡���ߣ�101����68��
	auto texture3 = Director::getInstance()->getTextureCache()->addImage("$lucia_forward.png");
	// Todo
	run.reserve(8);
	for (int i = 0; i < 8; i++) {
		auto frame = SpriteFrame::createWithTexture(texture3, CC_RECT_PIXELS_TO_POINTS(Rect(68 * i, 0, 68, 101)));
		run.pushBack(frame);
	}

	// ���ò�������
	auto labelW = Label::createWithTTF("W", "fonts/arial.ttf", 36);
	auto labelA = Label::createWithTTF("A", "fonts/arial.ttf", 36);
	auto labelS = Label::createWithTTF("S", "fonts/arial.ttf", 36);
	auto labelD = Label::createWithTTF("D", "fonts/arial.ttf", 36);
	auto labelX = Label::createWithTTF("X", "fonts/arial.ttf", 36);
	auto labelY = Label::createWithTTF("Y", "fonts/arial.ttf", 36);
	auto itemW = MenuItemLabel::create(labelW, CC_CALLBACK_0(GameScene::touchW, this));
	auto itemA = MenuItemLabel::create(labelA, CC_CALLBACK_0(GameScene::touchA, this));
	auto itemS = MenuItemLabel::create(labelS, CC_CALLBACK_0(GameScene::touchS, this));
	auto itemD = MenuItemLabel::create(labelD, CC_CALLBACK_0(GameScene::touchD, this));
	auto itemX = MenuItemLabel::create(labelX, CC_CALLBACK_0(GameScene::touchX, this));
	auto itemY = MenuItemLabel::create(labelY, CC_CALLBACK_0(GameScene::touchY, this));
	itemW->setPosition(80,100);
	itemA->setPosition(30,50);
	itemS->setPosition(Vec2(origin.x + 80, origin.y + 50));
	itemD->setPosition(130,50);
	itemX->setPosition(Vec2(visibleSize.width - 70, 50));
	itemY->setPosition(Vec2(visibleSize.width - 30, 100));

	auto menu = Menu::create(itemW, itemA, itemS, itemD, itemX, itemY, NULL);
	menu->setPosition(Vec2::ZERO);
	this->addChild(menu, 4);

	// ��������ʱ
	dtime = 120;
	time = Label::createWithTTF("120", "fonts/arial.ttf", 36);
	time->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height - 30));
	this->addChild(time);

	// ÿ���ʱ������������ƶ�
	schedule(schedule_selector(GameScene::updateTime), 1);
	// ÿ���ж��Ƿ��ܹ���
	schedule(schedule_selector(GameScene::hitByMonster), 1);
	// ÿ��3������һֻ��������ڳ�����
	schedule(schedule_selector(GameScene::addMonster), 3);
	// ÿ��3��������һ���ƶ�
	schedule(schedule_selector(GameScene::moveMonster), 3);

	// ��ʾ����
	score = 0;
	auto bScore = Label::createWithTTF("Best Score: ", "fonts/arial.ttf", 28);
	bScore->setPosition(Vec2(origin.x + visibleSize.width - 120, origin.y + visibleSize.height - 30));
	this->addChild(bScore, 4);

	if (!database->getBoolForKey("isExist")) {
		database->setBoolForKey("isExist", true);
	}
	// ��userdefault�л�ȡ��߷ּ�¼
	//int best = database->getIntegerForKey("best");

	//  ��SQLite�л�ȡ��߷ּ�¼
	int best = getTheBestScore();

	bestScore = Label::createWithTTF(Value(best).asString(), "fonts/arial.ttf", 28);
	bestScore->setPosition(Vec2(origin.x + visibleSize.width - 30, origin.y + visibleSize.height - 30));
	this->addChild(bestScore, 4);

	auto yScore = Label::createWithTTF("Your Score: ", "fonts/arial.ttf", 28);
	yScore->setPosition(Vec2(origin.x + visibleSize.width - 120, origin.y + visibleSize.height - 60));
	this->addChild(yScore, 4);
	yourScore = Label::createWithTTF("0", "fonts/arial.ttf", 28);
	yourScore->setPosition(Vec2(origin.x + visibleSize.width - 30, origin.y + visibleSize.height - 60));
	this->addChild(yourScore, 4);

	// ������Ӧ
	auto keyboardListener = EventListenerKeyboard::create();
	keyboardListener->onKeyPressed = CC_CALLBACK_2(GameScene::onKeyPressed, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);

    return true;
}

int GameScene::getTheBestScore() {
	sqlite3* pdb = NULL;
	std::string path = FileUtils::getInstance()->getWritablePath() + "save.db";
	int result = sqlite3_open(path.c_str(), &pdb);
	if (result == SQLITE_OK) {
		std::string sql = "create table score(BestScore int, id int primary key not null);";
		result = sqlite3_exec(pdb, sql.c_str(), NULL, NULL, NULL);
		sql = "insert into score values(0,1);";
		sqlite3_exec(pdb, sql.c_str(), NULL, NULL, NULL);
	}
	char **Score;
	int row, col;
	sqlite3_get_table(pdb, "select BestScore from score", &Score, &row, &col, NULL);
	int bestScore = atoi(Score[1]);
	sqlite3_free_table(Score);
	return bestScore;
}

void GameScene::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event) {
	switch (keyCode) {
		case EventKeyboard::KeyCode::KEY_W:
			touchW();
			break;
		case EventKeyboard::KeyCode::KEY_A:
			touchA();
			break;
		case EventKeyboard::KeyCode::KEY_S:
			touchS();
			break;
		case EventKeyboard::KeyCode::KEY_D:
			touchD();
			break;
		case EventKeyboard::KeyCode::KEY_X:
			touchX();
			break;
		case EventKeyboard::KeyCode::KEY_Y:
			touchY();
			break;
		default: break;
	}
}

void GameScene::touchW() {
	if (dtime == 0 || blood == 0 || player->getActionByTag(2)) return;
	player->stopAllActions();
	auto runAnimate = Animate::create(Animation::createWithSpriteFrames(run, 0.1f));
	if (player->getPosition().y + 80 < visibleSize.height) {
		auto moveto = MoveTo::create(0.8, Vec2(player->getPosition().x, player->getPosition().y + 40));
		auto move = Spawn::createWithTwoActions(moveto, runAnimate);
		player->runAction(move);
	}
	else {
		player->runAction(runAnimate);
	}
}

void GameScene::touchA() {
	if (dtime == 0 || blood == 0 || player->getActionByTag(2)) return;
	player->stopAllActions();
	cid = 'A';
	changeDirection();
	auto runAnimate = Animate::create(Animation::createWithSpriteFrames(run, 0.1f));
	if (player->getPosition().x - 80 > 0) {
		auto moveto = MoveTo::create(0.8, Vec2(player->getPosition().x - 40, player->getPosition().y));
		auto move = Spawn::createWithTwoActions(moveto, runAnimate);
		player->runAction(move);
	}
	else {
		player->runAction(runAnimate);
	}
}

void GameScene::touchS() {
	if (dtime == 0 || blood == 0 || player->getActionByTag(2)) return;
	player->stopAllActions();
	auto runAnimate = Animate::create(Animation::createWithSpriteFrames(run, 0.1f));
	if (player->getPosition().y - 80 > 0) {
		auto moveto = MoveTo::create(0.8, Vec2(player->getPosition().x, player->getPosition().y - 40));
		auto move = Spawn::createWithTwoActions(moveto, runAnimate);
		player->runAction(move);
	}
	else {
		player->runAction(runAnimate);
	}
}

void GameScene::touchD() {
	if (dtime == 0 || blood == 0 || player->getActionByTag(2)) return;
	player->stopAllActions();
	cid = 'D';
	changeDirection();
	auto runAnimate = Animate::create(Animation::createWithSpriteFrames(run, 0.1f));
	if (player->getPosition().x + 80 < visibleSize.width) {
		auto moveto = MoveTo::create(0.8, Vec2(player->getPosition().x + 40, player->getPosition().y));
		auto move = Spawn::createWithTwoActions(moveto, runAnimate);
		player->runAction(move);
	}
	else {
		player->runAction(runAnimate);
	}
}

void GameScene::touchX() {
	if (dtime == 0 || blood == 0 || player->getNumberOfRunningActions()) return;
	player->stopAllActions();
	auto deadAnimate = Animate::create(Animation::createWithSpriteFrames(dead, 0.1f));
	deadAnimate->setTag(2);
	player->runAction(deadAnimate);
	bloodSub();
}

void GameScene::touchY() {
	if (dtime == 0 || blood == 0 || player->getActionByTag(2)) return;
	player->stopAllActions();
	auto attackAnimate = Animate::create(Animation::createWithSpriteFrames(attack, 0.1f));
	attackAnimate->setTag(2);
	player->runAction(attackAnimate);
	attackJudge();
}

void GameScene::updateTime(float dt){
	dtime--;
	if (dtime < 0) {
		dtime = 0;
		player->stopAllActions();
		if (blood != 0) {
			win();
			unscheduleAllSelectors();
		}
		else {
			over();
			scheduleOnce(schedule_selector(GameScene::dying), 0);
		}
	}
	char str[10];
	sprintf(str, "%d", dtime); // ��int����ת��Ϊ�ַ���char*����
	time->setString(str);
}

void GameScene::dying(float dt) {
	player->stopAllActions();
	auto deadAnimate = Animate::create(Animation::createWithSpriteFrames(dead, 0.1f));
	player->runAction(deadAnimate);
	unscheduleAllSelectors();
}

void GameScene::addMonster(float dt) {
	auto monster = Factory::getInstance()->createMonster();
	float x = random(origin.x, visibleSize.width);
	float y = random(origin.y, visibleSize.height);
	monster->setPosition(x, y);
	this->addChild(monster, 3);
}

void GameScene::hitByMonster(float dt) {
	auto factory = Factory::getInstance();
	auto collision = factory->collider(player->getBoundingBox());
	if (collision != NULL) {
		// �Ƴ������ҵ�Ѫ
		factory->removeMonster(collision);
		bloodSub();
	}
}

// ����������ƶ���ÿ���ƶ�ʱ��Ϊ2��
void GameScene::moveMonster(float dt) {
	Factory::getInstance()->moveMonster(player->getPosition(), 2);
}

// ��Ѫ����������
void GameScene::bloodAdd() {
	if (blood + 20 < 100) {
		blood = blood + 20;
	}
	else {
		blood = 100;
	}
	auto plus = ProgressTo::create(1.0f, blood);
	pT->runAction(plus);
}

// ��Ѫ����������
void GameScene::bloodSub() {
	if (blood - 20 > 0) {
		blood = blood - 20;
	}
	else {
		blood = 0;
		// ��ʾgame over �Լ� ������������һ��, ������ͣ��ʱ
		scheduleOnce(schedule_selector(GameScene::dying), 0);
		over();
	}
	auto reduce = ProgressTo::create(1.0f, blood);
	pT->runAction(reduce);
}

// �ı���ҷ���
void GameScene::changeDirection() {
	if (cid == 'A') {
		if (last_cid != 'A') {
			player->setFlipX(true);
		}
		last_cid = 'A';
	}
	else if (cid == 'D') {
		if (last_cid != 'D') {
			player->setFlipX(false);
		}
		last_cid = 'D';
	}
}

void GameScene::attackJudge() {
	Rect playerRect = player->getBoundingBox();
	Rect attackRect = Rect(playerRect.getMinX() - 30, playerRect.getMinY(),
		playerRect.getMaxX() - playerRect.getMinX() + 60,
		playerRect.getMaxY() - playerRect.getMinY());
	auto factory = Factory::getInstance();
	auto collision = factory->collider(attackRect);
	if (collision != NULL) {
		// �Ƴ������һ�Ѫ
		factory->removeMonster(collision);
		bloodAdd();
		updateScore();
	}
}

void GameScene::over() {
	auto over = Label::createWithTTF("Game over!", "fonts/arial.ttf", 50);
	over->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2));
	this->addChild(over,1);
	
	//if (score > database->getIntegerForKey("best")) {
	//	database->setIntegerForKey("best", score);
	//}
	saveScore();
}

void GameScene::win() {
	auto win = Label::createWithTTF("You win!", "fonts/arial.ttf", 50);
	win->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2));
	this->addChild(win,1);
	
	//if (score > database->getIntegerForKey("best")) {
	//	database->setIntegerForKey("best", score);
	//}
	saveScore();
}

void GameScene::updateScore() {
	score++;
	char str[10];
	sprintf(str, "%d", score); // ��int����ת��Ϊ�ַ���char*����
	yourScore->setString(str);
}

void GameScene::saveScore() {
	std::string str = std::to_string(score);
	std::string sql = "update score set BestScore = " + str + " where id = 1; ";
	sqlite3* pdb = NULL;
	std::string path = FileUtils::getInstance()->getWritablePath() + "save.db";
	sqlite3_open(path.c_str(), &pdb);
	char **Score;
	int row, col;
	sqlite3_get_table(pdb, "select BestScore from score", &Score, &row, &col, NULL);
	int bestScore = atoi(Score[1]);
	if (score > bestScore) {
		sqlite3_exec(pdb, sql.c_str(), NULL, NULL, NULL);
	}
}