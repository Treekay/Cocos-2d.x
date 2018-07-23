#include "UsersInfoScene.h"
#include "network\HttpClient.h"
#include "json\document.h"
#include "Utils.h"
#include "string.h"

using namespace cocos2d::network;
using namespace rapidjson;

cocos2d::Scene * UsersInfoScene::createScene() {
  return UsersInfoScene::create();
}

bool UsersInfoScene::init() {
  if (!Scene::init()) return false;

  auto visibleSize = Director::getInstance()->getVisibleSize();
  Vec2 origin = Director::getInstance()->getVisibleOrigin();

  auto getUserButton = MenuItemFont::create("Get User", CC_CALLBACK_1(UsersInfoScene::getUserButtonCallback, this));
  if (getUserButton) {
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + getUserButton->getContentSize().height / 2;
    getUserButton->setPosition(Vec2(x, y));
  }

  auto backButton = MenuItemFont::create("Back", [](Ref* pSender) {
    Director::getInstance()->popScene();
  });
  if (backButton) {
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + visibleSize.height - backButton->getContentSize().height / 2;
    backButton->setPosition(Vec2(x, y));
  }

  auto menu = Menu::create(getUserButton, backButton, NULL);
  menu->setPosition(Vec2::ZERO);
  this->addChild(menu, 1);

  limitInput = TextField::create("limit", "arial", 24);
  if (limitInput) {
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + visibleSize.height - 100.0f;
    limitInput->setPosition(Vec2(x, y));
    this->addChild(limitInput, 1);
  }

  messageBox = Label::create("", "arial", 30);
  if (messageBox) {
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + visibleSize.height / 2;
    messageBox->setPosition(Vec2(x, y));
    this->addChild(messageBox, 1);
  }

  return true;
}

void UsersInfoScene::getUserButtonCallback(Ref * pSender) {
  // Your code here
	HttpRequest* request = new HttpRequest();
	request->setUrl("127.0.0.1:8000/users?limit=" + limitInput->getStringValue());
	request->setRequestType(HttpRequest::Type::GET);
	request->setResponseCallback(CC_CALLBACK_2(UsersInfoScene::onHttpRequestCompleted, this));

	request->setTag("GET INFO");
	cocos2d::network::HttpClient::getInstance()->send(request);
	request->release();
}

void UsersInfoScene::onHttpRequestCompleted(HttpClient* sender, HttpResponse* response) {
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
			const rapidjson::Value& data = doc["data"];
			assert(data.IsArray());
			for (rapidjson::SizeType i = 0; i < data.Size(); i++) {
				str += "Username: " + (std::string)data[i]["username"].GetString() + "\n" + "Deck:\n";
				const rapidjson::Value& deck = data[i]["deck"];
				assert(deck.IsArray());
				for (rapidjson::SizeType j = 0; j < deck.Size(); j++) {
					assert(deck[j].IsObject());
					for (rapidjson::Value::ConstMemberIterator itr = deck[j].MemberBegin(); itr != deck[j].MemberEnd(); ++itr) {
						str += "  " + (std::string)itr->name.GetString() + ": " + StringUtils::toString(itr->value.GetInt()) +"\n";
					}
					str += "  ---\n";
				}
				str += "---\n";
			}
			this->messageBox->setString(str);
		}
		else {
			this->messageBox->setString(response->getHttpRequest()->getTag() + std::string(" Failed\n") + doc["msg"].GetString());
		}
	}
}
