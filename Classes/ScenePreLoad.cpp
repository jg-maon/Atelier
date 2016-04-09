/**=============================================================================
	@file		ScenePreLoad.cpp
	@brief		ゲーム使用リソースロードシーンクラス 実装
	@author		Masato Yamamoto
	@date		2015-02-10
			
	@par		[説明]
				ゲーム使用リソースロードシーン
				NowLoading...ってやつ
	@note

		Copyright (C) 2015  saitsuyo
=============================================================================*/

#include "ScenePreLoad.h"
#include "SceneModeSelect.h"		// 次のシーン

#include "SimpleAudioEngine.h"	// 音関係
#include "MultiResolution.h"

#include "Common.h"

#pragma execution_character_set("utf-8")

USING_NS_CC;




#pragma region ScenePreLoad methods

Scene* ScenePreLoad::createScene()
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();
	
	// 'layer' is an autorelease object
	auto layer = ScenePreLoad::create();
	
	// add layer as a child to scene
	scene->addChild(layer, 0, "ScenePreLoad");

	// return the scene
	return scene;
}


// on "init" you need to initialize your instance
bool ScenePreLoad::init()
{
	//////////////////////////////
	// 1. super init first
	if (!Layer::init())
	{
		return false;
	}

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	/////////////////////////////
	// 3. add your codes below...

	m_phase = Phase::LOADING;


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

	//------------------------------------
	// loadinglabel
	auto label = Label::createWithSystemFont("データロード中", "GenShinGothic-Regular.ttf", calcFontSize(30.f));

	label->setHorizontalAlignment(TextHAlignment::CENTER);
	label->setPosition(visibleSize/ 2.f);

	this->addChild(label, 1, "NowLoading");

	// ラベル
	auto labelLoaded = Label::createWithSystemFont("0%", "GenShinGothic-Regular.ttf", calcFontSize(30.f));
	labelLoaded->setPosition(label->getPositionX() + label->getContentSize().width*label->getAnchorPoint().x,
							 label->getPositionY() - label->getContentSize().height*label->getAnchorPoint().y - labelLoaded->getContentSize().height
							);
	labelLoaded->setAnchorPoint(Vec2::ANCHOR_MIDDLE_RIGHT);
	labelLoaded->setHorizontalAlignment(TextHAlignment::RIGHT);
	this->addChild(labelLoaded, 10, "LoadedLabel");


	// リングエフェクト
	auto spriteRing = Sprite::create("ring.png");
	{
		spriteRing->setPosition(visibleSize / 2.f);
		spriteRing->setColor(Color3B(250, 250, 100));
		// 加算合成
		BlendFunc blend;
		blend.src = GL_SRC_ALPHA;
		blend.dst = GL_ONE;
		spriteRing->setBlendFunc(blend);
		spriteRing->setOpacity(0);
		this->addChild(spriteRing, 100, "Ring");
	}


	//--------------------------------------------
	// タッチリスナー
	this->_SetTouchListener();

	// 処理呼び出し
	this->scheduleUpdate();

	// リソース読み込み
	this->_LoadResource();

	return true;
}

/**
	@brief		更新
	@param[in]	delta	1フレーム間隔
	@return		なし
*/
void ScenePreLoad::update(float delta)
{
	switch (m_phase)
	{
	case ScenePreLoad::Phase::LOADING:
		if (m_loadedNum >= m_imageNum)
		{
			// 全部の画像読み込み完了
			m_phase = Phase::COMPLETED;
			//----------------------------------------
			// リングエフェクト
			const float duration = 1.5f;
			auto ring = this->getChildByName("Ring");
			ring->runAction(
				Sequence::create(
					Spawn::create(
						EaseCubicActionOut::create(ScaleBy::create(duration, 2.f)),	// 拡大
						Sequence::create(
							EaseSineOut::create(FadeIn::create(duration*0.2f)),	// 急いでフェードイン
							EaseExponentialOut::create(FadeOut::create(duration*0.8f)),	// 緩やかにフェードアウト
							nullptr
						),
						nullptr
					),
					// 終了後シーン移動
					CallFunc::create([](){
						Director::getInstance()->replaceScene(
							TransitionFade::create(1.f, SceneModeSelect::createScene())
							);
					}),
					nullptr
				)
			);

		}
		break;
	case ScenePreLoad::Phase::COMPLETED:
		break;
	default:
		break;
	}
}


/**
	@brief		タッチリスナーの設定
	@param[in]	なし
	@return		なし
*/
void ScenePreLoad::_SetTouchListener()
{
	auto pbnode = ParticleBatchNode::create("particle_stars.png");
	this->addChild(pbnode, 10, "Particle");
	auto listener = EventListenerTouchAllAtOnce::create();
	auto dispatcher = Director::getInstance()->getEventDispatcher();
	listener->onTouchesBegan = [pbnode](const std::vector<Touch*>& touches, Event* event)
	{};
	listener->onTouchesMoved = [this, pbnode](const std::vector<Touch*>& touches, Event* event)
	{
		auto target = event->getCurrentTarget();
		for (auto& touch : touches)
		{
			auto p = common::createParticle("particle_twinkle_stars.plist");
			Vec2 pos(target->convertToNodeSpace(touch->getLocation()));
			p->setPosition(pos);
			pbnode->addChild(p);
		}
	};
	listener->onTouchesEnded = [this, pbnode](const std::vector<Touch*>& touches, Event* event)
	{
		auto target = event->getCurrentTarget();
		for (auto& touch : touches)
		{
			auto p = common::createParticle("particle_sparkle_stars.plist");
			Vec2 pos(target->convertToNodeSpace(touch->getLocation()));
			p->setPosition(pos);
			pbnode->addChild(p);
		}
	};
	dispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

/**
	@brief		リソースのロード
	@param[in]	なし
	@return		なし
*/
void ScenePreLoad::_LoadResource()
{
	// BGM
	std::vector<std::string> sounds = {
		"ModeSelectBGM.mp3",
		"GameMainBGM.mp3",
	};
	// SE
	std::vector<std::string> effects = {
		"PageTurn.mp3",			// ページめくり
		"ModeSelected.mp3",		// モードセレクト各シーン押したとき
		"ButtonPushed.mp3",		// ボタン音
	};

	// Sprite
	std::vector<std::string> images = {
		"ArrowLeft.png",
		"ArrowRight.png",
		"BackButton.png",
		"BackButton2.png",
		"bar.png",
		"batsu.png",
		"BigBoard.png",
		"BlackBoard.png",
		"board.png",
		"BrushCheck2.png",
		"button.png",
		"button2.png",
		"canvas.png",
		"chara_nichi.png",
		"chara_tsuki.png",
		"CharaBack.png",
		"CharaBigTheme.png",
		"CharaDrawEnd.png",
		"CharaHint.png",
		"CharaKanaTheme.png",
		"CharaSmallTheme.png",
		"CharaTheme.png",
		"CheckHintButton.png",
		"CheckHintButton2.png",
		"CheckHintButton3.png",
		"CheckThemeButton.png",
		"CheckThemeButton2.png",
		"CheckThemeButton3.png",
		"CircleCanvas.png",
		"CountDownIllustBack.png",
		"CountUpIllustBack.png",
		"CrossCanvas.png",
		"decide.png",
		"decide2.png",
		"decide3.png",
		"DegitalNumber.png",
		"FrameButton.png",
		"HelloWorld.png",
		"hour.png",
		"HumanIcon.png",
		"Line.png",
		"LineBack.png",
		"MainBackAll.png",
		"mark.png",
		"minutes.png",
		"ModeDescription.png",
		"ModeSelectBack.png",
		"NextButton.png",
		"NextButton2.png",
		"number.png",
		"off.png",
		"OffDisabled.png",
		"on.png",
		"OnDisabled.png",
		"OptionIllust.png",
		"particle_stars.png",
		"particle_texture.png",
		"PassButton.png",
		"PassButton2.png",
		"PassButton3.png",
		"pie30.png",
		"PlayerButton_Disabled.png",
		"PlayerButton_Normal0.png",
		"PlayerButton_Normal1.png",
		"PlayerButton_Normal2.png",
		"PlayerButton_Normal3.png",
		"PlayerButton_Normal4.png",
		"PlayerButton_Normal5.png",
		"PlayerButton_Normal6.png",
		"PlayerButton_Normal7.png",
		"PlayerButton_Normal8.png",
		"PlayerButton_Selected0.png",
		"PlayerButton_Selected1.png",
		"PlayerButton_Selected2.png",
		"PlayerButton_Selected3.png",
		"PlayerButton_Selected4.png",
		"PlayerButton_Selected5.png",
		"PlayerButton_Selected6.png",
		"PlayerButton_Selected7.png",
		"PlayerButton_Selected8.png",
		"RelayIllust.png",
		"ring.png",
		"seconds.png",
		"select_back_black.png",
		"SettingButton.png",
		"SettingButton2.png",
		"ThemeInputBox.png",
	};

	//--------------------------------
	// BGM,SE読み込み
	{
		// オーディオエンジン
		auto engine = CocosDenshion::SimpleAudioEngine::getInstance();
		// BGM読み込み
		for (const auto& name : sounds)
		{
			engine->preloadBackgroundMusic(name.c_str());
		}
		// SE読み込み
		for (const auto& name : effects)
		{
			engine->preloadEffect(name.c_str());
		}
	}


	//--------------------------------
	// Sprite読み込み
	{
		m_imageNum = images.size();
		m_loadedNum = 0;
		auto label = this->getChildByName<Label*>("LoadedLabel");
		std::stringstream ss;
		ss << static_cast<int>(static_cast<float>(m_loadedNum) / m_imageNum * 100.f) << "%";
		label->setString(ss.str());
		log("%3d / %3d", m_loadedNum, m_imageNum);
		auto cache = Director::getInstance()->getTextureCache();
		for (const auto& name : images)
		{
			cache->addImageAsync(name, CC_CALLBACK_1(ScenePreLoad::_LoadedResource, this));
		}
	}

}

/**
	@brief		1つSpriteが読み込まれたら呼ばれる
	@param[in]	pTexture		読み込んだSpriteのテクスチャ
	@return		なし
*/
void ScenePreLoad::_LoadedResource(cocos2d::Texture2D* pTexture)
{
	m_loadedNum++;

	// ラベル更新
	auto label = this->getChildByName<Label*>("LoadedLabel");
	std::stringstream ss;
	ss << static_cast<int>(static_cast<float>(m_loadedNum) / m_imageNum * 100.f) << "%";
	label->setString(ss.str());
	log("%3d / %3d", m_loadedNum, m_imageNum);

}




#pragma endregion	// ScenePreLoad methods
