#include "LoginRegisterScene.h"
#include "ui\CocosGUI.h"
#include "network\HttpClient.h"
#include "json\document.h"
#include "json\rapidjson.h"
#include "json\stringbuffer.h"
#include "json\writer.h"
#include "Utils.h"

USING_NS_CC;
using namespace cocos2d::network;
using namespace cocos2d::ui;
using namespace rapidjson;

cocos2d::Scene * LoginRegisterScene::createScene() {
  return LoginRegisterScene::create();
}

bool LoginRegisterScene::init() {
  if (!Scene::init()) {
    return false;
  }

  auto visibleSize = Director::getInstance()->getVisibleSize();
  Vec2 origin = Director::getInstance()->getVisibleOrigin();

  auto loginButton = MenuItemFont::create("Login", CC_CALLBACK_1(LoginRegisterScene::loginButtonCallback, this));
  if (loginButton) {
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + loginButton->getContentSize().height / 2;
    loginButton->setPosition(Vec2(x, y));
  }

  auto registerButton = MenuItemFont::create("Register", CC_CALLBACK_1(LoginRegisterScene::registerButtonCallback, this));
  if (registerButton) {
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + registerButton->getContentSize().height / 2 + 100;
    registerButton->setPosition(Vec2(x, y));
  }

  auto backButton = MenuItemFont::create("Back", [] (Ref* pSender) {
    Director::getInstance()->popScene();
  });
  if (backButton) {
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + visibleSize.height - backButton->getContentSize().height / 2;
    backButton->setPosition(Vec2(x, y));
  }

  auto menu = Menu::create(loginButton, registerButton, backButton, NULL);
  menu->setPosition(Vec2::ZERO);
  this->addChild(menu, 1);

  usernameInput = TextField::create("username", "arial", 24);
  if (usernameInput) {
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + visibleSize.height - 100.0f;
    usernameInput->setPosition(Vec2(x, y));
    this->addChild(usernameInput, 1);
  }

  passwordInput = TextField::create("password", "arial", 24);
  if (passwordInput) {
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + visibleSize.height - 130.0f;
    passwordInput->setPosition(Vec2(x, y));
    this->addChild(passwordInput, 1);
  }

  messageBox = Label::create("", "arial", 30);
  if (messageBox) {
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + visibleSize.height - 200.0f;
    messageBox->setPosition(Vec2(x, y));
    this->addChild(messageBox, 1);
  }

  return true;
}

void LoginRegisterScene::loginButtonCallback(cocos2d::Ref * pSender) {
  // Your code here
	rapidjson::Document d;
	d.SetObject();
	rapidjson::Document::AllocatorType& allocator = d.GetAllocator();
	rapidjson::Value username, password;
	username.SetString(usernameInput->getString().c_str(), allocator);
	password.SetString(passwordInput->getString().c_str(), allocator);
	d.AddMember("username", username, allocator);
	d.AddMember("password", password, allocator);

	StringBuffer buffer;
	rapidjson::Writer<StringBuffer> writter(buffer);
	d.Accept(writter);
	CCLOG("%s", buffer.GetString());

	auto request = new HttpRequest();
	request->setUrl("http://127.0.0.1:8000/auth");
	request->setRequestType(HttpRequest::Type::POST);
	request->setRequestData(buffer.GetString(), buffer.GetSize());
	request->setTag("Register");
	request->setResponseCallback(CC_CALLBACK_2(LoginRegisterScene::OnLoginCompleted, this));
	HttpClient::getInstance()->enableCookies("cookies");
	HttpClient::getInstance()->send(request);
	request->release();
}

void LoginRegisterScene::registerButtonCallback(Ref * pSender) {
  // Your code here
	rapidjson::Document d;
	d.SetObject();
	rapidjson::Document::AllocatorType& allocator = d.GetAllocator();
	rapidjson::Value username, password;
	username.SetString(usernameInput->getString().c_str(), allocator);
	password.SetString(passwordInput->getString().c_str(), allocator);
	d.AddMember("username", username, allocator);
	d.AddMember("password", password, allocator);

	StringBuffer buffer;
	rapidjson::Writer<StringBuffer> writter(buffer);
	d.Accept(writter);
	CCLOG("%s", buffer.GetString());

	auto request = new HttpRequest();
	request->setUrl("http://127.0.0.1:8000/users");
	request->setRequestType(HttpRequest::Type::POST);
	request->setRequestData(buffer.GetString(), buffer.GetSize());
	request->setTag("Register");
	request->setResponseCallback(CC_CALLBACK_2(LoginRegisterScene::OnRegisterCompleted, this));
	
	HttpClient::getInstance()->send(request);
	//HttpClient::getInstance()->enableCookies("cookies");
	request->release();
}

void LoginRegisterScene::OnRegisterCompleted(HttpClient *sender, HttpResponse *response) {
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
		std::string fail = "Register Failed\n";
		messageBox->setString(fail + d["msg"].GetString());
	}
	else
	{
		messageBox->setString("Register OK");
	}
}

void LoginRegisterScene::OnLoginCompleted(HttpClient *sender, HttpResponse *response) {
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
		std::string fail = "Login Failed\n";
		messageBox->setString(fail + d["msg"].GetString());
	}
	else
	{
		messageBox->setString(d["msg"].GetString());
	}
}