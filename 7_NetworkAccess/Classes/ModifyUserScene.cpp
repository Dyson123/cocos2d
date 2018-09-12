#include "ModifyUserScene.h"
#include "network\HttpClient.h"
#include "json\document.h"
#include "json\rapidjson.h"
#include "json\stringbuffer.h"
#include "json\writer.h"
#include "Utils.h"

using namespace cocos2d::network;
using namespace rapidjson;

cocos2d::Scene * ModifyUserScene::createScene() {
  return ModifyUserScene::create();
}

bool ModifyUserScene::init() {
  if (!Scene::init()) return false;
  
  auto visibleSize = Director::getInstance()->getVisibleSize();
  Vec2 origin = Director::getInstance()->getVisibleOrigin();

  auto postDeckButton = MenuItemFont::create("Post Deck", CC_CALLBACK_1(ModifyUserScene::putDeckButtonCallback, this));
  if (postDeckButton) {
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + postDeckButton->getContentSize().height / 2;
    postDeckButton->setPosition(Vec2(x, y));
  }

  auto backButton = MenuItemFont::create("Back", [](Ref* pSender) {
    Director::getInstance()->popScene();
  });
  if (backButton) {
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + visibleSize.height - backButton->getContentSize().height / 2;
    backButton->setPosition(Vec2(x, y));
  }

  auto menu = Menu::create(postDeckButton, backButton, NULL);
  menu->setPosition(Vec2::ZERO);
  this->addChild(menu, 1);

  deckInput = TextField::create("Deck json here", "arial", 24);
  if (deckInput) {
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + visibleSize.height - 100.0f;
    deckInput->setPosition(Vec2(x, y));
    this->addChild(deckInput, 1);
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

void ModifyUserScene::putDeckButtonCallback(Ref * pSender) {
	std::string buffer = "{\"deck\":[" + deckInput->getString() + "]\n}";
	HttpRequest* request = new HttpRequest();
	request->setUrl("http://127.0.0.1:8000/users");
	request->setRequestType(HttpRequest::Type::PUT);
	request->setResponseCallback(CC_CALLBACK_2(ModifyUserScene::OnPutDeckCompleted, this));
	request->setTag("Modify");
	request->setRequestData(buffer.c_str(), buffer.length());
	HttpClient::getInstance()->send(request);
	request->release();
}

void ModifyUserScene::OnPutDeckCompleted(HttpClient *sender, HttpResponse *response) {
	if (!response) {
		return;
	}
	if (!response->isSucceed()) {
		CCLOG("Response Failed");
		CCLOG("error buffer: %s", response->getErrorBuffer());
		return;
	}
	auto data = response->getResponseData();
	std::string json(data->begin(), data->end());
	Document d;
	d.Parse<0>(json.c_str());
	if (d["status"] == false)
	{
		std::string failMessage = "Put Failed\n";
		messageBox->setString(failMessage + d["msg"].GetString());
	}
	else
	{
		messageBox->setString("Put OK\n");
	}
}
