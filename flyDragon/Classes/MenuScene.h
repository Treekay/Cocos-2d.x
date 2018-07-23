#pragma once
#include <stdio.h>
#include "cocos2d.h"
#include "ui\CocosGUI.h"
#include "network\HttpClient.h"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace cocos2d::network;

class MenuScene : public cocos2d::Scene {
	public:
		static cocos2d::Scene* createScene();

		virtual bool init();
    
		// a selector callback
		//void startMenuCallback(cocos2d::Ref* pSender);
    
		// implement the "static create()" method manually
		CREATE_FUNC(MenuScene);

		void menuCloseCallback(cocos2d::Ref* pSender);

		void menuStartCallback(Ref * pSender);

		void getRank(Ref *pSender);

		void onHttpRequestCompleted(HttpClient* sender, HttpResponse* response);

		Label *messageBox;
};