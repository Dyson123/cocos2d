#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
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

    visibleSize = Director::getInstance()->getVisibleSize();
    origin = Director::getInstance()->getVisibleOrigin();
	isAnimating = false;

	//����������
	schedule(schedule_selector(HelloWorld::update), 1.0f, 180, 0);
	//����ʱ���ǩ
	time = Label::create("180", "arial", 36);
	time->setPosition(origin.x + visibleSize.width / 2, origin.y + visibleSize.height - 50);
	this->addChild(time, 1);

	//����һ����ͼ
	auto texture1 = Director::getInstance()->getTextureCache()->addImage("$lucia_2.png");
	//����ͼ�������ص�λ�и�����ؼ�֡
	auto frame0 = SpriteFrame::createWithTexture(texture1, CC_RECT_PIXELS_TO_POINTS(Rect(0, 0, 113, 113)));
	//ʹ�õ�һ֡��������
	player = Sprite::createWithSpriteFrame(frame0);
	player->setPosition(Vec2(origin.x + visibleSize.width / 2,
		origin.y + visibleSize.height / 2));
	addChild(player, 3);

	//hp��
	Sprite* sp0 = Sprite::create("hp.png", CC_RECT_PIXELS_TO_POINTS(Rect(0, 320, 420, 47)));
	Sprite* sp = Sprite::create("hp.png", CC_RECT_PIXELS_TO_POINTS(Rect(610, 362, 4, 16)));

	//ʹ��hp������progressBar
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

	// ��̬����(֡����1)
	idle.reserve(1);
	idle.pushBack(frame0);

	// ��������(֡����17֡���ߣ�113����113��
	attack.reserve(17);
	for (int i = 0; i < 17; i++) {
		auto frame = SpriteFrame::createWithTexture(texture1, CC_RECT_PIXELS_TO_POINTS(Rect(113 * i, 0, 113, 113)));
		attack.pushBack(frame);
	}
	auto attackAnimation = Animation::createWithSpriteFrames(attack, 0.1f);
	attackAnimation->setRestoreOriginalFrame(true);
	AnimationCache::getInstance()->addAnimation(attackAnimation, "attack");

	// ��������(֡����22֡���ߣ�90����79��
	auto texture2 = Director::getInstance()->getTextureCache()->addImage("$lucia_dead.png");
	dead.reserve(22);
	for (int i = 0; i < 22; i++) {
		auto frame = SpriteFrame::createWithTexture(texture2, CC_RECT_PIXELS_TO_POINTS(Rect(79 * i, 0, 90, 79)));
		dead.pushBack(frame);
	}
	auto deadAnimation = Animation::createWithSpriteFrames(dead, 0.1f);
	deadAnimation->setRestoreOriginalFrame(true);
	AnimationCache::getInstance()->addAnimation(deadAnimation, "dead");

	// Todo

	// �˶�����(֡����8֡���ߣ�101����68��
	auto texture3 = Director::getInstance()->getTextureCache()->addImage("$lucia_forward.png");
	run.reserve(8);
	for (int i = 0; i < 8; i++) {
		auto frame = SpriteFrame::createWithTexture(texture3, CC_RECT_PIXELS_TO_POINTS(Rect(68 * i, 0, 68, 101)));
		run.pushBack(frame);
	}
	auto runAnimation = Animation::createWithSpriteFrames(run, 0.1f);
	AnimationCache::getInstance()->addAnimation(runAnimation, "run");
	// Todo


	//�����˵�
	auto menu = Menu::create();
	menu->setPosition(50, 50);
	addChild(menu);

	//ʹ��lambda���ʽ����menuitem
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

	//����������ť
	auto label = Label::create("Y", "arial", 36);
	auto menuItem = MenuItemLabel::create(label, CC_CALLBACK_1(HelloWorld::attackCallback, this));
	menuItem->setPosition(origin.x + visibleSize.width - 120, -10);
	menu->addChild(menuItem);

	//����������ť
	label = Label::create("X", "arial", 36);
	menuItem = MenuItemLabel::create(label, CC_CALLBACK_1(HelloWorld::deadCallback, this));
	menuItem->setPosition(origin.x + visibleSize.width - 90, 15);
	menu->addChild(menuItem);

    return true;
}

void HelloWorld::moveCallback(Ref* pSender, std::string direction) {
	auto curPosition = player->getPosition();
	if (!isAnimating) {
		isAnimating = true;

		//����߽����С����
		int minimum = 50;
		int x = 0, y = 0;

		if (direction == "A" && curPosition.x > minimum) {
			x = -20;
			y = 0;
		}
		else if (direction == "S" && curPosition.y > minimum) {
			x = 0;
			y = -35;
		}
		else if (direction == "W" && curPosition.y < visibleSize.height - minimum) {
			x = 0;
			y = 35;
		}
		else if (direction == "D" && curPosition.x < visibleSize.width - minimum) {
			x = 20;
			y = 0;
		}
		//�������ж����ƶ����ƶ�������ͬʱ����
		auto spawn = Spawn::createWithTwoActions(Animate::create(AnimationCache::getInstance()->getAnimation("run")), MoveBy::create(0.2f, Vec2(x, y)));
		
		auto sequence = Sequence::create(spawn,
			CCCallFunc::create(([this]() {
				isAnimating = false;
			})), nullptr);
		player->runAction(sequence);
	}
	
}

void HelloWorld::attackCallback(Ref* pSender) {
	if (isAnimating == false) {
		//��ֹͬʱ���Ŷ���
		isAnimating = true;

		//�������У�ִ�ж������ٽ�isAnimating��Ϊfalse
		auto sequence = Sequence::create(Animate::create(AnimationCache::getInstance()->getAnimation("attack")),
			CCCallFunc::create(([this]() {
				isAnimating = false;
			})), nullptr);
		player->runAction(sequence);
		
		//����Ѫ��
		float percentage = pT->getPercentage();
		if (percentage < 100) {
			auto increase = ProgressFromTo::create(2.0f, percentage, percentage + 20);
			pT->runAction(increase);
		}
	}
}

void HelloWorld::deadCallback(Ref* pSender) {
	if (isAnimating == false) {
		//��ֹͬʱ���Ŷ���
		isAnimating = true;

		//�������У�ִ�ж������ٽ�isAnimating��Ϊfalse
		auto sequence = Sequence::create(Animate::create(AnimationCache::getInstance()->getAnimation("dead")),
			CCCallFunc::create(([this]() {
				isAnimating = false;
			})), nullptr);
		player->runAction(sequence);
		
		//����Ѫ��
		float percentage = pT->getPercentage();
		if (percentage > 0) {
			auto decrease = ProgressFromTo::create(2.0f, percentage, percentage - 20);
			pT->runAction(decrease);
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
		unschedule(schedule_selector(HelloWorld::update));
	}

}
