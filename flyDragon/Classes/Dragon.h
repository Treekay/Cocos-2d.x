#pragma once

#ifndef DRAGON
#define DRAGON
#include "cocos2d.h"

USING_NS_CC;

#define DRAGONBLUE 1
#define DRAGONBLACK 2

class Dragon
{
public:
	// 不是单例模式，应该保存在场景中声明实例，传入颜色
	Dragon(int color, Vec2 position);
	
	// 跳一下
	void jump();

	// 死了
	void die();

	// 上下移动，1为上，-1为下
	void move(int dir);

	// 停止
	void stop();

	// 得到精灵
	Sprite *get();

	// 开始游戏，传入是否为重力模式
	void begin(bool gravityEnabled);

	// 检查
	void check();

private:
	Sprite * dragon;
	Vector<SpriteFrame*> flying;
};
#endif