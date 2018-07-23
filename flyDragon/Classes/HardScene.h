#pragma once
#include "cocos2d.h"
#include "Dragon.h"
#include "Barrier.h"
#include "SimpleAudioEngine.h"
#include "MenuScene.h"
#include "ui\CocosGUI.h"

using namespace std;
using namespace CocosDenshion;

class HardScene : public cocos2d::Layer {
public:
	static cocos2d::Scene* createScene();
	void addListener();
	void checkAll(float f);
	bool onConcactBegin(PhysicsContact & contact);
	void onKeyPressed(EventKeyboard::KeyCode code, Event* event);
	void onKeyReleased(EventKeyboard::KeyCode code, Event* event);
	void barrierMove(char c);
	void dargonMove(char c);
	virtual bool init();
	void gameover();

	CREATE_FUNC(HardScene);
private:
	Dragon * dragon;
	Barrier* barrier;
	bool DMove;
	bool BMove;
	char Dmovekey;
	char Bmovekey;
	Label* Score;
	Label* Best;
	int score = 0;
};