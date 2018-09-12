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

    // add a "close" icon to exit the progress. it's an autorelease object
    auto closeItem = MenuItemImage::create(
                                           "CloseNormal.png",
                                           "CloseSelected.png",
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

	//add a musicItem to play music
	auto menuItem = MenuItemFont::create("Click Me", CC_CALLBACK_1(HelloWorld::menuCallback, this));
	menuItem->setTag(1);

	float x = origin.x + visibleSize.width - menuItem->getContentSize().width / 2;
	float y = origin.y + menuItem->getContentSize().height / 2 + closeItem->getContentSize().height;
	menuItem->setPosition(Vec2(x, y));
	
	// create menu, it's an autorelease object
	auto menu = Menu::create(closeItem, menuItem, NULL);
	menu->setName("menu");
	menu->setPosition(Vec2::ZERO);
	this->addChild(menu, 1);

    // add a label shows my name and id
   
	auto *chnStrings = Dictionary::createWithContentsOfFile("string.xml");
	const char* myName = ((String*)chnStrings->objectForKey("Name"))->getCString();
	const char* myId = ((String*)chnStrings->objectForKey("ID"))->getCString();
	auto nameLabel = Label::create(myName, "fonts/FZSIK.ttf", 24);
	auto idLabel = Label::create(myId, "fonts/Marker Felt.ttf", 24);
	nameLabel->setName("name");
	//handling error
	if (nameLabel == nullptr) {
		problemLoading("'fonts/FZSIK.ttf'");
	}
	else {
		// position the label on the center of the screen
		nameLabel->setPosition(Vec2(origin.x + visibleSize.width / 2,
			origin.y + visibleSize.height - nameLabel->getContentSize().height*2));

		// add the label as a child to this layer
		this->addChild(nameLabel, 1);
	}

	if (idLabel == nullptr) {
		problemLoading("'fonts/Marker Felt.ttf'");
	}
	else {		
		idLabel->setPosition(Vec2(origin.x + visibleSize.width / 2,
			origin.y + visibleSize.height -nameLabel->getContentSize().height - idLabel->getContentSize().height*2));
		this->addChild(idLabel, 1);
	}

    // add "HelloWorld" splash screen"
    auto sprite = Sprite::create("myImage.jpg");
    if (sprite == nullptr)
    {
        problemLoading("'myImage.jpg'");
    }
    else
    {
        // position the sprite on the center of the screen
        sprite->setPosition(Vec2(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));

        // add the sprite as a child to this layer
        this->addChild(sprite, 0);
    }
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

void HelloWorld::menuCallback(Ref* pSender) {
	/*
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
	auto visibleSize = Director::getInstance()->getVisibleSize();

	auto newLabel = Label::create("Menu item has beed clicked!", "fonts/Marker Felt.ttf", 24);
	newLabel->setPosition(Vec2(origin.x+ visibleSize.width / 2, visibleSize.height - newLabel->getContentSize().height));

	this->addChild(newLabel, 1);
	*/
	
	//change the color of the "Click Me" message
	auto menu = this->getChildByName("menu");
	auto clickItem = menu->getChildByTag(1);
	if (clickItem->getColor() == Color3B::WHITE) {
		clickItem->setColor(Color3B::BLUE);
	}
	else {
		clickItem->setColor(Color3B::WHITE);
	}
	
}