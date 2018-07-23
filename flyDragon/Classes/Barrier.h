#pragma once

#ifndef BARRIER
#define BARRIER
#include "cocos2d.h"

USING_NS_CC;

// 该类会完成addChild
class Barrier
{
public:
	// 传入柱子的数量（以对为单位)，模式(0,1,2)，传入场景this
	Barrier(int count, int _mode, Layer *scene);

	// 检查柱子是否出界
	bool check(Vec2 pos);

	// 开始游戏
	void begin();

	// 获取所有柱子
	std::vector<std::vector<Sprite*>> get();

	// 小鸟的位置
	void move(Vec2 pos, int dir);

	void stop();

	//游戏结束停止柱子
	void end();
private:
	Sprite * createOne();

	std::vector<std::vector<Sprite*>> all;

	Layer *sc;

	// visibleSize
	Size vs;
	// 是否自动上下移动
	int mode;
};
#endif