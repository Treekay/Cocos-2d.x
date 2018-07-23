#pragma once
#include "cocos2d.h"
#include "Dragon.h"
#include "Barrier.h"
#include "SimpleAudioEngine.h"
#include "MenuScene.h"
#include "ui\CocosGUI.h"

using namespace std;
using namespace CocosDenshion;

class MediumScene : public cocos2d::Layer {
public:
	static cocos2d::Scene* createScene();
	void addListener();
	void checkAll(float f);
	bool onConcactBegin(PhysicsContact & contact);
	void onKeyPressed(EventKeyboard::KeyCode code, Event* event);
	void onKeyReleased(EventKeyboard::KeyCode code, Event* event);
	void move(char c);
	virtual bool init();
	void gameover();

	CREATE_FUNC(MediumScene);
private:
	Dragon* dragon;
	Barrier* barrier;
	bool over;
	bool isMove;
	char movekey;
	Label* Score;
	Label* Best;
	int score = 0;
};