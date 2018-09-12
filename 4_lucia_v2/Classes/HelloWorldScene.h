#pragma once
#include "cocos2d.h"
using namespace cocos2d;

class HelloWorld : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
        
    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);
private:
	cocos2d::Sprite* player;
	cocos2d::Vector<SpriteFrame*> attack;
	cocos2d::Vector<SpriteFrame*> dead;
	cocos2d::Vector<SpriteFrame*> run;
	cocos2d::Vector<SpriteFrame*> idle;
	cocos2d::Size visibleSize;
	cocos2d::Vec2 origin;
	cocos2d::Label* time;
	cocos2d::Label* sNum;
	int dtime;
	int score;
	std::string lastDir;
	bool isAnimating;
	bool isAttacking;
	bool isDead;
	cocos2d::ProgressTimer* pT;
	void moveCallback(Ref* pSender, std::string direction);
	void attackCallback(Ref* pSender);
	void deadCallback(Ref* pSender);
	void update(float dt) override;
	void createMonster(float dt);
	void hitByMonster(float dt);
};
