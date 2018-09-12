#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
#include "Monster.h"
#define database UserDefault::getInstance()

#pragma execution_character_set("utf-8")
using std::string;

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

	//数据库
	if (!database->getBoolForKey("isExist")) {
		database->setBoolForKey("isExist", true);
	}
	score = database->getIntegerForKey("score");

    visibleSize = Director::getInstance()->getVisibleSize();
    origin = Director::getInstance()->getVisibleOrigin();
	lastDir = "";
	isAnimating = false;
	isAttacking = false;
	isDead = false;
	
	TMXTiledMap *tmx = TMXTiledMap::create("map.tmx");
	tmx->setPosition(visibleSize.width / 2, visibleSize.height / 2);
	tmx->setAnchorPoint(Vec2(0.5, 0.5));
	tmx->setScale(Director::getInstance()->getContentScaleFactor());
	this->addChild(tmx, 0);
	
	//分数
	auto sLable = Label::create("Score:", "arial", 36);
	sLable->setPosition(origin.x + visibleSize.width - 150, origin.y + visibleSize.height - 50);
	this->addChild(sLable, 1);
	char str[5] = { 0 };
	sprintf(str, "%d", score);
	sNum = Label::create(str, "arial", 36);
	sNum->setPosition(origin.x + visibleSize.width - 50, origin.y + visibleSize.height - 50);
	this->addChild(sNum, 1);


	//创建时间标签
	time = Label::create("180", "arial", 36);
	time->setPosition(origin.x + visibleSize.width / 2, origin.y + visibleSize.height - 50);
	this->addChild(time, 1);
	//创建调度器
	schedule(schedule_selector(HelloWorld::update), 1.0f, 180, 0);
	schedule(schedule_selector(HelloWorld::createMonster), 2.0f);
	schedule(schedule_selector(HelloWorld::hitByMonster), 2.0f);


	//创建一张贴图
	auto texture1 = Director::getInstance()->getTextureCache()->addImage("$lucia_2.png");
	//从贴图中以像素单位切割，创建关键帧
	auto frame0 = SpriteFrame::createWithTexture(texture1, CC_RECT_PIXELS_TO_POINTS(Rect(0, 0, 113, 113)));
	//使用第一帧创建精灵
	player = Sprite::createWithSpriteFrame(frame0);
	player->setPosition(Vec2(origin.x + visibleSize.width / 2,
		origin.y + visibleSize.height / 2));
	addChild(player, 3);

	//hp条
	Sprite* sp0 = Sprite::create("hp.png", CC_RECT_PIXELS_TO_POINTS(Rect(0, 320, 420, 47)));
	Sprite* sp = Sprite::create("hp.png", CC_RECT_PIXELS_TO_POINTS(Rect(610, 362, 4, 16)));

	//使用hp条设置progressBar
	pT = ProgressTimer::create(sp);
	pT->setScaleX(90);
	pT->setAnchorPoint(Vec2(0, 0));
	pT->setType(ProgressTimerType::BAR);
	pT->setBarChangeRate(Point(1, 0));
	pT->setMidpoint(Point(0, 1));
	pT->setPercentage(100);
	pT->setPosition(Vec2(origin.x + 14 * pT->getContentSize().width, origin.y + visibleSize.height - 2 * pT->getContentSize().height));
	addChild(pT, 1);
	sp0->setAnchorPoint(Vec2(0, 0));
	sp0->setPosition(Vec2(origin.x + pT->getContentSize().width, origin.y + visibleSize.height - sp0->getContentSize().height));
	addChild(sp0, 0);

	// 静态动画(帧数：1)
	idle.reserve(1);
	idle.pushBack(frame0);

	// 攻击动画(帧数：17帧，高：113，宽：113）
	attack.reserve(17);
	for (int i = 0; i < 17; i++) {
		auto frame = SpriteFrame::createWithTexture(texture1, CC_RECT_PIXELS_TO_POINTS(Rect(113 * i, 0, 113, 113)));
		attack.pushBack(frame);
	}
	auto attackAnimation = Animation::createWithSpriteFrames(attack, 0.1f);
	attackAnimation->setRestoreOriginalFrame(true);
	AnimationCache::getInstance()->addAnimation(attackAnimation, "attack");

	// 死亡动画(帧数：22帧，高：90，宽：79）
	auto texture2 = Director::getInstance()->getTextureCache()->addImage("$lucia_dead.png");
	dead.reserve(22);
	for (int i = 0; i < 22; i++) {
		auto frame = SpriteFrame::createWithTexture(texture2, CC_RECT_PIXELS_TO_POINTS(Rect(79 * i, 0, 90, 79)));
		dead.pushBack(frame);
	}
	auto deadAnimation = Animation::createWithSpriteFrames(dead, 0.1f);
	AnimationCache::getInstance()->addAnimation(deadAnimation, "dead");

	// Todo

	// 运动动画(帧数：8帧，高：101，宽：68）
	auto texture3 = Director::getInstance()->getTextureCache()->addImage("$lucia_forward.png");
	run.reserve(8);
	for (int i = 0; i < 8; i++) {
		auto frame = SpriteFrame::createWithTexture(texture3, CC_RECT_PIXELS_TO_POINTS(Rect(68 * i, 0, 68, 101)));
		run.pushBack(frame);
	}
	auto runAnimation = Animation::createWithSpriteFrames(run, 0.1f);
	AnimationCache::getInstance()->addAnimation(runAnimation, "run");
	// Todo


	//创建菜单
	auto menu = Menu::create();
	menu->setPosition(50, 50);
	addChild(menu);

	//使用lambda表达式创建menuitem
	auto createDirectionLabel = [this, &menu](std::string c) {
		int x = 0, y = 0;
		auto label = Label::create(c, "arial", 36);
		auto menuItem = MenuItemLabel::create(label, CC_CALLBACK_1(HelloWorld::moveCallback, this, c));
		if (c == "W")
			y += 1.2 * label->getContentSize().height;
		else if (c == "A")
			x -= 1.5 * label->getContentSize().width;
		else if (c == "D")
			x += 1.5 * label->getContentSize().width;
		else if (c == "S")
			x = x; 
		menuItem->setPosition(x, y);
		menu->addChild(menuItem);
	};

	createDirectionLabel("W");
	createDirectionLabel("S");
	createDirectionLabel("A");
	createDirectionLabel("D");

	//创建攻击按钮
	auto label = Label::create("Y", "arial", 36);
	auto menuItem = MenuItemLabel::create(label, CC_CALLBACK_1(HelloWorld::attackCallback, this));
	menuItem->setPosition(origin.x + visibleSize.width - 120, -10);
	menu->addChild(menuItem);

	//创建死亡按钮
	label = Label::create("X", "arial", 36);
	menuItem = MenuItemLabel::create(label, CC_CALLBACK_1(HelloWorld::deadCallback, this));
	menuItem->setPosition(origin.x + visibleSize.width - 90, 15);
	menu->addChild(menuItem);

    return true;
}

void HelloWorld::moveCallback(Ref* pSender, std::string direction) {
	if (isDead) return;
	auto curPosition = player->getPosition();
	int minimum = 50;
	int x = 0, y = 0;

	if (direction == "A" && curPosition.x > minimum) {
		x = -20;
		if (lastDir != "A") {
			player->setFlipX(true);
			lastDir = "A";
		}
	}
	else if (direction == "S" && curPosition.y > minimum) {
		y = -35;
	}
	else if (direction == "W" && curPosition.y < visibleSize.height - minimum) {
		y = 35;
	}
	else if (direction == "D" && curPosition.x < visibleSize.width - minimum) {
		x = 20;
		if (lastDir != "D") {
			player->setFlipX(false);
			lastDir = "D";
		}
	}
	//创建序列动作移动和移动动画：同时发生
	auto spawn = Spawn::createWithTwoActions(Animate::create(AnimationCache::getInstance()->getAnimation("run")), MoveBy::create(0.2f, Vec2(x, y)));
	player->runAction(spawn);
	
}

void HelloWorld::attackCallback(Ref* pSender) {
	if (isDead) return;
	if (isAnimating == false) {
		//防止同时播放动画
		isAnimating = true;
		isAttacking = true;

		//创建序列：执行动画后再将isAnimating置为false
		auto sequence = Sequence::create(Animate::create(AnimationCache::getInstance()->getAnimation("attack")),
			CCCallFunc::create(([this]() {
				isAnimating = false;
				isAttacking = false;
			})), nullptr);
		player->runAction(sequence);
	}
}

void HelloWorld::deadCallback(Ref* pSender) {
	if (time->getString() == "GAME OVER!")return;

	if (isDead)
	{
		if (isAnimating == false) {
			//防止同时播放动画
			isAnimating = true;

			//创建序列：执行动画后再将isAnimating置为false
			auto sequence = Sequence::create(Animate::create(AnimationCache::getInstance()->getAnimation("dead")),
				CCCallFunc::create(([this]() {
				isAnimating = false;
				time->setString("GAME OVER!");
				player->stopAllActions();
				unschedule(schedule_selector(HelloWorld::update));
				unschedule(schedule_selector(HelloWorld::createMonster));
				unschedule(schedule_selector(HelloWorld::hitByMonster));
			})), nullptr);
			player->runAction(sequence);
		}
	}
}

void HelloWorld::update(float dt) {
	string str = time->getString();
	int num = atoi(str.c_str());
	if (num > 0) {
		num--;
		char nextStr[5] = { 0 };
		sprintf(nextStr, "%d", num);
		time->setString(nextStr);
	}
	else {
		time->setString("GAME OVER!");
		player->stopAllActions();
		unschedule(schedule_selector(HelloWorld::update));
		unschedule(schedule_selector(HelloWorld::createMonster));
		unschedule(schedule_selector(HelloWorld::hitByMonster));
	}
}


void HelloWorld::createMonster(float dt) {
	auto fac = Factory::getInstance();
	auto mons = fac->createMonster();
	float x = random(origin.x, visibleSize.width);
	float y = random(origin.y, visibleSize.height);
	mons->setPosition(x, y);
	addChild(mons, 3);

	fac->moveMonster(player->getPosition(), 0.5f);
}

void HelloWorld::hitByMonster(float dt) {
	auto fac = Factory::getInstance();
	Rect playerRect = player->boundingBox();
	Rect attackRect = Rect(playerRect.getMinX() - 40, playerRect.getMinY(),
		playerRect.getMaxX() - playerRect.getMinX() + 80,
		playerRect.getMaxY() - playerRect.getMinY());

	Sprite* collision = fac->collider(attackRect);
	if (collision != NULL) {
		if (isAttacking) {
			fac->removeMonster(collision);

			//加分写进数据库
			score++;
			char Str[5] = { 0 };
			sprintf(Str, "%d", score);
			sNum->setString(Str);
			database->setIntegerForKey("score", score);
			
			float percentage = pT->getPercentage();
			if (percentage < 100) {
				auto increase = ProgressFromTo::create(1.0f, percentage, percentage + 20);
				pT->runAction(increase);
			}
			
			
		}
		else {
			fac->removeMonster(collision);
			//减少血条
			float percentage = pT->getPercentage();
			if (percentage > 0) {
				auto decrease = ProgressFromTo::create(1.0f, percentage, percentage - 20);
				pT->runAction(decrease);
			}
			if (percentage - 20 <= 0) {
				isDead = true;
				deadCallback(NULL);
			}
		}
	}
}