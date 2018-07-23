#include "VersionScene.h"
#include "network\HttpClient.h"
#include "json\document.h"
#include "Utils.h"
#define database UserDefault::getInstance()

USING_NS_CC;
using namespace cocos2d::network;

void VersionScene::upgradeWithJson(const rapidjson::Document &doc) {
	// 传入json更新数据的方法
}

cocos2d::Scene * VersionScene::createScene() {
	return VersionScene::create();
}

bool VersionScene::init() {
	if (!Scene::init()) return false;

	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	auto backButton = MenuItemFont::create("Back", [](Ref* pSender) {
		Director::getInstance()->popScene();
	});
	if (backButton) {
		float x = origin.x + visibleSize.width / 2;
		float y = origin.y + visibleSize.height - backButton->getContentSize().height / 2;
		backButton->setPosition(Vec2(x, y));
	}

	auto upgradeButton = MenuItemFont::create("Upgrade", CC_CALLBACK_1(VersionScene::upgradeVersion, this));
	if (upgradeButton) {
		float x = origin.x + visibleSize.width / 2;
		float y = origin.y + upgradeButton->getContentSize().height / 2;
		upgradeButton->setPosition(Vec2(x, y));
	}

	auto leftButton = MenuItemFont::create("<-", [this](Ref *sender) { this->switchPicture(true); });
	if (leftButton) {
		float x = origin.x + visibleSize.width / 2 - 100;
		float y = origin.y + leftButton->getContentSize().height / 2 + 50;
		leftButton->setPosition(Vec2(x, y));
	}

	auto rightButton = MenuItemFont::create("->", [this](Ref *sender) { this->switchPicture(false); });
	if (rightButton) {
		float x = origin.x + visibleSize.width / 2 + 100;
		float y = origin.y + rightButton->getContentSize().height / 2 + 50;
		rightButton->setPosition(Vec2(x, y));
	}

	auto menu = Menu::create(upgradeButton, backButton, leftButton, rightButton, NULL);
	menu->setPosition(Vec2::ZERO);
	this->addChild(menu, 1);

	versionInput = TextField::create("Version Here", "arial", 24);
	if (versionInput) {
		float x = origin.x + visibleSize.width / 2;
		float y = origin.y + visibleSize.height - 100.0f;
		versionInput->setPosition(Vec2(x, y));
		this->addChild(versionInput, 1);
	}

	picture = Sprite::create("winged_kuriboh.jpg");
	if (picture) {
		float x = origin.x + visibleSize.width / 2;
		float y = origin.y + visibleSize.height / 2;
		picture->setPosition(Vec2(x, y));
		this->addChild(picture);
	}

	pictureName = Label::create();
	if (pictureName) {
		float x = origin.x + visibleSize.width / 2;
		float y = origin.y + visibleSize.height - 150.0f;
		pictureName->setPosition(Vec2(x, y));
		this->addChild(pictureName);
	}

	if (!database->getBoolForKey("isExist")) {
		database->setBoolForKey("isExist", true);
		database->setStringForKey("version", "1.0");
	}
	if (database->getIntegerForKey("num")) {
		std::vector<std::string> names;
		std::vector<std::string> filenames;
		splitString(database->getStringForKey("cardnames"), "|", names);
		splitString(database->getStringForKey("cardfilenames"), "|", filenames);
		picture->initWithFile(database->getStringForKey(filenames[currentPicIndex].c_str()));
		pictureName->setString(names[currentPicIndex]);
	}
	return true;
}

void VersionScene::upgradeVersion(Ref *sender) {
	// 按下升级时调用该方法
	HttpRequest* request = new HttpRequest();
	request->setUrl("https://easy-mock.com/mock/5b06434130326c5f05624462/api/assets?current_version=" + database->getStringForKey("version") + "&to_version=" + versionInput->getString());
	request->setRequestType(HttpRequest::Type::GET);
	request->setResponseCallback([this](HttpClient* sender, HttpResponse* response) {
		if (!response) {
			return;
		}
		if (!response->isSucceed()) {
			log("Upgrade Failed\n");
			log("Error Buffer: %s", response->getErrorBuffer());
		}
		else {
			log("Upgrade OK");
			auto buffer = response->getResponseData();
			rapidjson::Document doc;
			doc.Parse(buffer->data(), buffer->size());
			if (doc["status"] == true) {
				if (doc["data"] == NULL) return;
				const rapidjson::Value& data = doc["data"];
				database->setStringForKey("version", data["new_version"].GetString());

				const rapidjson::Value& assets = data["assets"];
				const rapidjson::Value& cards = assets["cards"];
				std::vector<std::string> names, filenames;
				for (rapidjson::Value::ConstMemberIterator itr = cards.MemberBegin(); itr != cards.MemberEnd(); ++itr) {
					
					rapidjson::Value jKey;
					rapidjson::Value jValue;
					rapidjson::Document::AllocatorType allocator;
					jValue.CopyFrom(itr->value, allocator);
					jKey.CopyFrom(itr->name, allocator);

					std::string name = jKey.GetString();
					std::string filename = jValue["filename"].GetString();

					filenames.push_back(filename);
					names.push_back(name);
					HttpRequest* req = new HttpRequest();
					req->setRequestType(HttpRequest::Type::GET);
					req->setUrl(itr->value["url"].GetString());
					req->setResponseCallback([this, filename](HttpClient* send, HttpResponse* resp) {
						if (!resp) return;
						if (!resp->isSucceed()){
							log("Request Failed\n");
							log("Error Buffer: %s", resp->getErrorBuffer());
							return;
						}
						auto buffer = resp->getResponseData();
						auto writablePath = FileUtils::getInstance()->getWritablePath();
						auto fullPath = writablePath + filename;
						writeFile(fullPath.c_str(), buffer->data(), buffer->size());
					});
					HttpClient::getInstance()->send(req);
					req->release();
					database->setStringForKey(filename.c_str(), FileUtils::getInstance()->getWritablePath() + filename);
				}
				if (filenames.size() != 0) {
					currentPicIndex = 0;
					database->setStringForKey("cardnames", joinStrings(names, "|"));
					database->setStringForKey("cardfilenames", joinStrings(filenames, "|"));
					database->setIntegerForKey("num", names.size());
					picture->initWithFile(database->getStringForKey(filenames[currentPicIndex].c_str()));
					pictureName->setString(names[currentPicIndex]);
				}
			}
		}
	});

	HttpClient::getInstance()->send(request);
	request->release();
}

void VersionScene::switchPicture(bool isLeft) {
	// 按下左/右按钮时调用该方法
	if (database->getIntegerForKey("num") == 0) return;
	std::vector<std::string> names;
	std::vector<std::string> filenames;
	splitString(database->getStringForKey("cardnames"), "|", names);
	splitString(database->getStringForKey("cardfilenames"), "|", filenames);
	if (isLeft) {
		if (currentPicIndex > 0) currentPicIndex--;
	}
	else {
		if (currentPicIndex < database->getIntegerForKey("num") - 1) currentPicIndex++;
	}
	picture->initWithFile(database->getStringForKey(filenames[currentPicIndex].c_str()));
	pictureName->setString(names[currentPicIndex]);
}
