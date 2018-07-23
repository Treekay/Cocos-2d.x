#pragma once
#include "cocos2d.h"
using namespace cocos2d;

class GameScene : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();

	void touchW();
	void touchA();
	void touchS();
	void touchD();
	void touchX();
	void touchY();
	void updateTime(float dt);
	void addMonster(float dt);
	void moveMonster(float dt);
	void hitByMonster(float dt);
	void bloodSub(); // Ѫ�����ټ�Ѫ������
	void bloodAdd(); // Ѫ�����Ӽ�Ѫ������
	void changeDirection();
	void attackJudge();
	void over(); // ֪ͨ��ʾ��Ϸ����
	void win(); // ֪ͨ��ʾ��Ϸʤ��
	void updateScore(); // ���ڸ��µ÷�
	void dying(float dt); // ��������ֹ���ж����͵�����
	void onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event);
	int getTheBestScore(); // �����ݿ��л����߷ּ�¼
	void saveScore(); // ������߷ּ�¼�����ݿ�
	void reset(); // ���¿�ʼ��Ϸ
        
    // implement the "static create()" method manually
    CREATE_FUNC(GameScene);
private:
	cocos2d::Sprite* player;
	cocos2d::Vector<SpriteFrame*> attack;
	cocos2d::Vector<SpriteFrame*> dead;
	cocos2d::Vector<SpriteFrame*> run;
	cocos2d::Vector<SpriteFrame*> idle;
	cocos2d::Size visibleSize;
	cocos2d::Vec2 origin;
	cocos2d::Label* time;
	cocos2d::Label* yourScore;
	cocos2d::Label* bestScore;
	int dtime;
	int score = 0; // ��¼��ǰ����
	cocos2d::ProgressTimer* pT;
	float blood = 100; // ��¼��ǰѪ��
	char cid; // ��¼��ǰ����
	char last_cid = 'D'; // ��¼��һ�ΰ����ĳ���
};
