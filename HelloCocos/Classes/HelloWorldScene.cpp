#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;

Scene* HelloWorld::createScene()
{
    return HelloWorld::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Scene::init() )
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

	auto sprite = Sprite::create("see.jpg");
	sprite->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
	this->addChild(sprite, 0);

    // add a "close" icon to exit the progress. it's an autorelease object
    auto closeItem = MenuItemImage::create("CloseNormal.png","CloseSelected.png",
                                           CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));

    if (closeItem == nullptr ||
        closeItem->getContentSize().width <= 0 ||
        closeItem->getContentSize().height <= 0)
    {
        problemLoading("'CloseNormal.png' and 'CloseSelected.png'");
    }
    else
    {
        float x = origin.x + visibleSize.width - closeItem->getContentSize().width/2;
        float y = origin.y + closeItem->getContentSize().height/2;
        closeItem->setPosition(Vec2(x,y));
    }

	auto playItem = MenuItemImage::create("PlayNormal.png", "PlaySelected.png",
										  CC_CALLBACK_1(HelloWorld::menuPlayCallback, this, sprite));

    // create menu, it's an autorelease object
    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

	auto menu_2 = Menu::create(playItem, NULL);
	menu_2->setPosition(Vec2(origin.x + visibleSize.width - playItem->getContentSize().width/2,
						origin.y + closeItem->getContentSize().height + playItem->getContentSize().height/2));
	this->addChild(menu_2, 1);
    /////////////////////////////
    // 3. add your codes below...

    // add a label shows "Hello World"
    // create and initialize a label

	CCDictionary* info = CCDictionary::createWithContentsOfFile("string.xml");
	const char* name = info->valueForKey("name")->getCString();
	const char* sid = info->valueForKey("sid")->getCString();
	auto nameLabel = Label::createWithTTF(name, "fonts/circle.ttf", 24);
	auto sidLabel = Label::createWithTTF(sid, "fonts/circle.ttf", 24);

    
    // position the label on the center of the screen
    nameLabel->setPosition(Vec2(origin.x + visibleSize.width/2,
                            origin.y + visibleSize.height - nameLabel->getContentSize().height));
	sidLabel->setPosition(Vec2(origin.x + visibleSize.width / 2,
		origin.y + visibleSize.height - sidLabel->getContentSize().height - nameLabel->getContentSize().height));
    // add the label as a child to this layer
    this->addChild(nameLabel, 1);
	this->addChild(sidLabel, 1);

    return true;
}


void HelloWorld::menuCloseCallback(Ref* pSender)
{
    //Close the cocos2d-x game scene and quit the application
    Director::getInstance()->end();

    #if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif

    /*To navigate back to native iOS screen(if present) without quitting the application  ,do not use Director::getInstance()->end() and exit(0) as given above,instead trigger a custom event created in RootViewController.mm as below*/

    //EventCustom customEndEvent("game_scene_close_event");
    //_eventDispatcher->dispatchEvent(&customEndEvent);


}

void HelloWorld::menuPlayCallback(Ref* pSender, Sprite* sprite) {
	int val = sprite->getOpacity();
	if (val <= 0) {
		sprite->setOpacity(255);
	}
	else {
		sprite->setOpacity(val-51);
	}
}
