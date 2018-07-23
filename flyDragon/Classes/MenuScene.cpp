#include "MenuScene.h"
#include "SimpleAudioEngine.h"
#include "EasyScene.h"
#include "MediumScene.h"
#include "HardScene.h"
#include "network\HttpClient.h"
#include "json\document.h"
#include "string.h"
#include "ui\CocosGUI.h"

using namespace cocos2d::network;
using namespace rapidjson;
using namespace cocos2d::ui;

USING_NS_CC;

Scene* MenuScene::createScene()
{
    return MenuScene::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool MenuScene::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Scene::init() )
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

	// Ìí¼Ó±³¾°
	auto bg = Sprite::create("background.jpg");
	bg->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
	this->addChild(bg, 0);

	auto closeItem = MenuItemImage::create(
		"CloseNormal.png",
		"CloseSelected.png",
		CC_CALLBACK_1(MenuScene::menuCloseCallback, this));

	if (closeItem == nullptr ||
		closeItem->getContentSize().width <= 0 ||
		closeItem->getContentSize().height <= 0) {
		problemLoading("'CloseNormal.png' and 'CloseSelected.png'");
	}
	else {
		float x = origin.x + visibleSize.width - closeItem->getContentSize().width / 2;
		float y = origin.y + closeItem->getContentSize().height / 2;
		closeItem->setPosition(Vec2(x, y));
	}

	auto gotoEasyScene = MenuItemFont::create("Classical Mode", [](Ref *pSender) {
		Director::getInstance()->replaceScene(TransitionCrossFade::create(0.8, EasyScene::createScene
		()));
	});
	if (gotoEasyScene) {
		float x = origin.x + visibleSize.width / 2;
		float y = origin.y + visibleSize.height / 2 + gotoEasyScene->getContentSize().height / 2 + 50;
		gotoEasyScene->setPosition(Vec2(x, y));
	}

	auto gotoMediumScene = MenuItemFont::create("Arcade Mode", [](Ref *pSender) {
		Director::getInstance()->replaceScene(TransitionCrossFade::create(0.8, MediumScene::createScene
		()));
	});
	if (gotoMediumScene) {
		float x = origin.x + visibleSize.width / 2;
		float y = origin.y + visibleSize.height / 2;
		gotoMediumScene->setPosition(Vec2(x, y));
	}

	auto gotoHardScene = MenuItemFont::create("DoublePlayers Mode", [](Ref *pSender) {
		Director::getInstance()->replaceScene(TransitionCrossFade::create(0.8, HardScene::createScene
		()));
	});
	if (gotoHardScene) {
		float x = origin.x + visibleSize.width / 2;
		float y = origin.y + visibleSize.height / 2 - gotoHardScene->getContentSize().height / 2 - 50;
		gotoHardScene->setPosition(Vec2(x, y));
	}

	//auto gotoTestScene = MenuItemFont::create("Test", [](Ref *pSender) {
	//	Director::getInstance()->replaceScene(TransitionCrossFade::create(0.8, GameSence::create
	//	()));
	//});
	//if (gotoTestScene) {
	//	float x = origin.x + visibleSize.width / 2;
	//	float y = origin.y + visibleSize.height - gotoTestScene->getContentSize().height / 2 - 150;
	//	gotoTestScene->setPosition(Vec2(x, y));
	//}

	// create menu, it's an autorelease object
	auto menu = Menu::create(closeItem, gotoEasyScene, gotoMediumScene, gotoHardScene, /*gotoTestScene, */ NULL);
	menu->setPosition(Vec2::ZERO);
	this->addChild(menu, 1);

	/////////////////////////////
	// 3. add your codes below...
	messageBox = Label::create("", "arial", 20);
	if (messageBox) {
		float x = origin.x + visibleSize.width / 2 + 300;
		float y = origin.y + visibleSize.height / 2 + 100;
		messageBox->setPosition(Vec2(x, y));
		this->addChild(messageBox, 1);
	}
	getRank(this);

	return true;
}


void MenuScene::menuCloseCallback(Ref* pSender) {
	//Close the cocos2d-x game scene and quit the application
	Director::getInstance()->end();
}

void MenuScene::getRank(Ref *pSender) {
	HttpRequest* request = new HttpRequest();
	request->setUrl("127.0.0.1:8080");
	request->setRequestType(HttpRequest::Type::GET);
	request->setResponseCallback(CC_CALLBACK_2(MenuScene::onHttpRequestCompleted, this));

	// write the post data
	request->setTag("Get Rank");
	cocos2d::network::HttpClient::getInstance()->send(request);
	request->release();
}

void MenuScene::onHttpRequestCompleted(HttpClient* sender, HttpResponse* response) {
	if (!response) {
		return;
	}
	if (!response->isSucceed()) {
		log("%s Failed", response->getHttpRequest()->getTag());
		log("Error Buffer: %s", response->getErrorBuffer());
	}
	else {
		log("%s OK", response->getHttpRequest()->getTag());
		auto buffer = response->getResponseData();
		rapidjson::Document doc;
		doc.Parse(buffer->data(), buffer->size());
		if (doc["status"] == true) {
			std::string str;
			str += "Rank List\n";
			const rapidjson::Value& rank = doc["rank"];
			assert(rank.IsArray());
			for (rapidjson::SizeType i = 0; i < rank.Size(); i++) {
				assert(rank[i].IsObject());
				str += (std::string)rank[i]["name"].GetString() + "  " + (std::string)rank[i]["score"].GetString() + "\n";
			}
			this->messageBox->setString(str);
		}
	}
}

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	exit(0);
#endif


