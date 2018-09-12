#include "UsersInfoScene.h"
#include "network\HttpClient.h"
#include "json\document.h"
#include "json\rapidjson.h"
#include "json\stringbuffer.h"
#include "json\writer.h"
#include "Utils.h"

using namespace cocos2d::network;
using namespace rapidjson;
using namespace std;

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

  // Your code here
void UsersInfoScene::getUserButtonCallback(Ref * pSender) {
	auto request = new HttpRequest();
	request->setRequestType(HttpRequest::Type::GET);
	request->setUrl("http://127.0.0.1:8000/users?limit=" + limitInput->getString());
	request->setResponseCallback(CC_CALLBACK_2(UsersInfoScene::OnUserInfoButtonCliked, this));
	HttpClient::getInstance()->send(request);
	request->release();
}

void UsersInfoScene::OnUserInfoButtonCliked(HttpClient *sender, HttpResponse *response) {
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
	if (d["status"].GetBool() == false)
	{
		std::string failMessage = "Get Users Failed\n";
		messageBox->setString(failMessage + d["msg"].GetString());
	}
	else
	{
		string usersInfo;
		for (auto& user : d["data"].GetArray())
		{
			usersInfo.append(string("Username: ") + user["username"].GetString() + "\nDeck:\n");
			for (auto& deck : user["deck"].GetArray())
			{
				for(auto& itr : deck.GetObjectW())
				{
					usersInfo.append(" ");
					usersInfo.append(itr.name.GetString());
					usersInfo.append(": ");
					usersInfo.append(to_string(itr.value.GetInt()));
					usersInfo.append("\n");
				}
				usersInfo.append(" ---\n");
			}
			usersInfo.append("---\n");
		}
		messageBox->setString(usersInfo);
	}
}