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
	void bloodSub(); // 血量减少及血条动画
	void bloodAdd(); // 血量增加及血条动画
	void changeDirection();
	void attackJudge();
	void over(); // 通知显示游戏结束
	void win(); // 通知显示游戏胜利
	void updateScore(); // 用于更新得分
	void dying(float dt); // 死亡后终止所有动作和调度器
	void onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event);
	int getTheBestScore(); // 从数据库中获得最高分记录
	void saveScore(); // 保存最高分记录到数据库
	void reset(); // 重新开始游戏
        
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
	int score = 0; // 记录当前分数
	cocos2d::ProgressTimer* pT;
	float blood = 100; // 记录当前血量
	char cid; // 记录当前朝向
	char last_cid = 'D'; // 记录上一次按键的朝向
};
