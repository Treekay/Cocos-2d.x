#pragma once

#ifndef __VERSION_SCENE_H__
#define __VERSION_SCENE_H__

#include "cocos2d.h"
#include "ui\CocosGUI.h"
#include "json\document.h"

using namespace cocos2d::ui;
USING_NS_CC;

class VersionScene: public cocos2d::Scene {
  int currentPicIndex = 0;
public:
  void upgradeWithJson(const rapidjson::Document &);
  static cocos2d::Scene* createScene();

  virtual bool init();

  void upgradeVersion(Ref *sender);
  void switchPicture(bool isLeft);

  // implement the "static create()" method manually
  CREATE_FUNC(VersionScene);

  TextField *versionInput;
  Label *pictureName;
  Sprite *picture;
  int num;
};

#endif
