/**=============================================================================
	@file		SceneTitle.cpp
	@brief		タイトルシーンクラス 実装
	@author		Masato Yamamoto
	@date		2015-02-12

	@par		[説明]
				タイトルシーンクラス
				画面タッチでモードセレクトシーンへ遷移

	@note

		Copyright (C) 2015  saitsuyo
=============================================================================*/

#include "SceneTitle.h"

//#include "SceneAtelierMain.h"
//#include "SceneModeSelect.h"

#include "ScenePreLoad.h"


#include "easing.h"
#include "SimpleAudioEngine.h"	// 音関係

#include "MultiResolution.h"

#include "Common.h"

#pragma execution_character_set("utf-8")
USING_NS_CC;

Scene* SceneTitle::createScene()
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = SceneTitle::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}


// on "init" you need to initialize your instance
bool SceneTitle::init()
{
	//////////////////////////////
	// 1. super init first
	if (!Layer::init())
	{
		return false;
	}

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Point origin = Director::getInstance()->getVisibleOrigin();

	/////////////////////////////
	// 2. add a menu item with "X" image, which is clicked to quit the program
	//    you may modify it.

	// add a "close" icon to exit the progress. it's an autorelease object
	auto closeItem = MenuItemImage::create(
		"CloseNormal.png",
		"CloseSelected.png",
		CC_CALLBACK_1(SceneTitle::menuCloseCallback, this));

	closeItem->setPosition(origin.x + visibleSize.width - closeItem->getContentSize().width / 2,
		origin.y + closeItem->getContentSize().height / 2);

	// create menu, it's an autorelease object
	auto menu = Menu::create(closeItem, nullptr);
	menu->setPosition(Vec2::ZERO);
	this->addChild(menu, 1);

	/////////////////////////////
	// 3. add your codes below...


	auto sprite = Sprite::create("background.png");
	if (sprite)
	{
		float scaleX = visibleSize.width / sprite->getContentSize().width;
		float scaleY = visibleSize.height / sprite->getContentSize().height;
		sprite->setScale(scaleX, scaleY);

		// position the sprite on the center of the screen
		sprite->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));

		// add the sprite as a child to this layer
		this->addChild(sprite, 0);
	}


	auto spriteTitleLogo = Sprite::create("Atelier.png");
	if (spriteTitleLogo)
	{
		spriteTitleLogo->setPosition(	visibleSize.width / 2.f,
										visibleSize.height - spriteTitleLogo->getContentSize().height*spriteTitleLogo->getAnchorPoint().y - 10.f);
		this->addChild(spriteTitleLogo, 0, "TitleLogo");
	}



	//シングルタッチ機能追加
	//this->setTouchMode(Touch::DispatchMode::ONE_BY_ONE);
	//this->setTouchEnabled(true);
	auto listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);
	listener->onTouchBegan = CC_CALLBACK_2(SceneTitle::onTouchBegan, this);
	listener->onTouchEnded = CC_CALLBACK_2(SceneTitle::onTouchEnded, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

	//------------------------------------
	// label

	// label情報
	m_pLabel = Label::createWithSystemFont("Touch Screen", "GenShinGothic-Regular.ttf", 120.f*MultiResolution::getFontScale());

	m_pLabel->setPosition(visibleSize.width / 2 + origin.x, origin.y + m_pLabel->getContentSize().height * 3);

	// FadeIn･Outは"Touch Graphics"だけなので
	m_pLabel->runAction(
		EaseIn::create(
			MoveTo::create(1.f, 
				Vec2(visibleSize.width / 2.f, 
					m_pLabel->getContentSize().height + 10.f)), 2.f));
	
	this->addChild(m_pLabel, 1);


	//初期値代入 ※共通
	m_labelScale = 1.0f;
	m_isSizeUp = false;
	m_time = 0.f;		// 0固定
	// 初期値代入 ※labels[0]のみに適応中→updateで処理
	m_fadeTime = 1.f;	// フェードインアウト処理で使う時間
	m_fadeState = FadeState::FADE_IN;	// フェードインorアウトどっちから開始か

	//------------------------------------
	// BGM

	// BGMのロード(ファイルはResourcesフォルダに)

	//------------------------------------
	// BGM・SEのロード
	CocosDenshion::SimpleAudioEngine::getInstance()->preloadBackgroundMusic("TitleBGM.mp3");

	//------------------------------------
	// BGM
	// データの保存に使用
	UserDefault *_userDef = UserDefault::getInstance();

	// float型の保存データ取り出し
	float bgmVolume = 0.5f;
	if (_userDef->getFloatForKey("BGM", bgmVolume) >= 0)
	{
		bgmVolume = _userDef->getFloatForKey("BGM");
	}
	float seVolume = 0.5f;
	if (_userDef->getFloatForKey("SE", seVolume) >= 0)
	{
		seVolume = _userDef->getFloatForKey("SE");
	}

	// BGMの音量の設定(最大値1.0、0.0は消音)
	CocosDenshion::SimpleAudioEngine::getInstance()->setBackgroundMusicVolume(bgmVolume);
	// SEの音量設定
	CocosDenshion::SimpleAudioEngine::getInstance()->setEffectsVolume(seVolume);

	// BGMの再生(第二引数：ループさせるか)
	CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic("TitleBGM.mp3", true);

	// Step処理呼び出し
	this->scheduleUpdate();




	return true;
}



/**
	@brief		更新
	@attention	init内で	this->scheduleUpdate();を呼ぶこと
	@param[in]	delta		1フレームの間隔
	@return		なし
*/
void SceneTitle::update(float delta)
{
	// 以下 α値を変更　フェードイン・アウト処理
	m_time += delta;
	if (m_fadeState == FadeState::FADE_IN)
	{
		// 透明度
		m_pLabel->setOpacity((int)(Easing::Linear(m_time, 0.f, 255.f - 0.f, m_fadeTime)));
		if (m_time > m_fadeTime)
		{
			m_fadeState = FadeState::FADE_OUT;
			m_time = 0.f;
		}
	}
	else
	{
		// 透明度
		m_pLabel->setOpacity((int)(Easing::Linear(m_time, 255.f, 0.f - 255.f, m_fadeTime)));
		if (m_time > m_fadeTime)
		{
			m_fadeState = FadeState::FADE_IN;
			m_time = 0.f;
		}
	}
	// 以下 スケール値を変更
	if (m_isSizeUp)
	{
		m_labelScale += 0.1f;
	}
	m_pLabel->setScale(m_labelScale);
}


void SceneTitle::menuCloseCallback(Ref* pSender)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WP8) || (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
	MessageBox("You pressed the close button. Windows Store Apps do not implement a close button.", "Alert");
	return;
#endif

	Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	exit(0);
#endif
}



/**
	@brief		タッチしたイベント
	@param[in]	touch		タッチ情報
	@param[in]	event		イベントを起こしたオブジェクト
	@return		タッチを有効にするか
	@retval		true		タッチを有効とする(onTouchEndedが呼び出される)
	@retval		false		タッチを無効とする(onTouchEndedが呼び出されない)
*/
bool SceneTitle::onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event)
{
	if (m_isSizeUp)
		return false;
	return true;
}




/**
	@brief		タッチし終わったイベント
	@param[in]	touch		タッチ情報
	@param[in]	event		イベントを起こしたオブジェクト
	@return		なし
*/
void SceneTitle::onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event)
{
	//auto target = static_cast<Sprite*>(event->getCurrentTarget());

	Director::getInstance()->replaceScene(
		TransitionFade::create(1.0f,
			//SceneAtelierMain::createScene())
			ScenePreLoad::createScene())
		);


	m_isSizeUp = true;
	m_fadeTime = 0.2f;


}
