/**=============================================================================
	@file		SceneAtelierMain.cpp
	@brief		ゲーム本編メインシーンクラス 実装
	@author		Masato Yamamoto
	@date		2015-02-01
			
	@par		[説明]
				ゲーム本編メインシーン
				モードセレクトからの引数で読み込みゲームレイヤーを変える
	@note

		Copyright (C) 2015  saitsuyo
=============================================================================*/

#include "SceneAtelierMain.h"

#include "OptionLayer.h"
#include "EseThemeInputLayer.h"
#include "EseMainLayer.h"
#include "CountUpThemeInputLayer.h"
#include "CountUpGameLayer.h"
#include "CountDownThemeInputLayer.h"
#include "CountDownGameLayer.h"
#include "RelayThemeInputLayer.h"
#include "RelayGameLayer.h"


#include "SceneTitle.h"			// 次のシーン
#include "SceneModeSelect.h"

#include "Common.h"

#include <fstream>
#include <sstream>

#include "MultiResolution.h"

#pragma execution_character_set("utf-8")

USING_NS_CC_EXT;
USING_NS_CC;










#pragma region SceneAtelierMain methods

//Scene* SceneAtelierMain::createScene()
//{
//    // 'scene' is an autorelease object
//    auto scene = Scene::create();
//    
//    // 'layer' is an autorelease object
//    auto layer = SceneAtelierMain::create();
//	
//    // add layer as a child to scene
//	scene->addChild(layer, 0, "SceneAtelierMain");
//
//    // return the scene
//    return scene;
//}

/**
	@brief		各種レイヤー読み込み処理
	@param[in]	mainName		ゲームメインのレイヤー名
	@return		なし
*/
Scene* SceneAtelierMain::createScene(const std::string& mainName)
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = SceneAtelierMain::create();
	layer->addLayer(mainName);
	// add layer as a child to scene
	scene->addChild(layer, 0, "SceneAtelierMain");
	// return the scene
	return scene;
}




/**
	@brief		コンストラクタ
	@param[in]	なし
	@return		なし
	@par		ロード処理用スレッドの立ち上げ
*/
SceneAtelierMain::SceneAtelierMain()
#if USE_MULTITHREAD
: m_isLoaded(false)
#endif
{
}
/**
	@brief		デストラクタ
	@param[in]	なし
	@return		なし
	@par		メモリの解放等
*/
SceneAtelierMain::~SceneAtelierMain()
{
#if USE_MULTITHREAD
	if (m_loadThread.joinable())
		m_loadThread.detach();		// スレッド終了
#endif
}


// on "init" you need to initialize your instance
bool SceneAtelierMain::init()
{
	//////////////////////////////
	// 1. super init first
	if ( !Layer::init() )
	{
		return false;
	}

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	/////////////////////////////

	//m_loadThread = std::thread(&SceneAtelierMain::_AddLayer, this);
	//m_loadThread.detach();

	//-----------------------------------------------------
	// デフォルト値
	log("%-4d----------- _SetDefaultValues Start -----------", __LINE__);
	_SetDefaultValues();
	log("%-4d----------- _SetDefaultValues End -----------\n", __LINE__);


#if USE_MULTITHREAD
	//-----------------------------------------------------
	// ロード中用のレイヤー作成
	auto layer = Layer::create();
	//layer->setPosition(visibleSize / 2.f);
	this->addChild(layer, 100, "LoadLayer");

	auto needle = Sprite::create();
	needle->setContentSize(Size(visibleSize.height / 2.f - 30.f, 30.f));
	needle->setTextureRect(Rect(0, 0, needle->getContentSize().width, needle->getContentSize().height));
	needle->setColor(Color3B::WHITE);
	needle->setPosition(layer->getContentSize().width / 2.f + needle->getContentSize().width*needle->getAnchorPoint().x, layer->getContentSize().height / 2.f);
	layer->addChild(needle, 0, 0);
	//-----------------------------------------------------
	// 毎フレームupdate呼び出し設定
	this->scheduleUpdate();
#endif

	return true;
}





/**
	@brief		更新
	@attention	init内で	this->scheduleUpdate();を呼ぶこと
	@param[in]	delta		1フレームの間隔
	@return		なし
*/
void SceneAtelierMain::update(float delta)
{
	delta = delta;
	//auto sprite = this->getChildByName("LoadLayer")->getChildByTag(0);
	//sprite->runAction(RotateBy::create(0.0f, delta));

#if USE_MULTITHREAD
	if (!m_isLoaded)
	{
		log("loading///");

		//this->getChildByName("LoadLayer")->runAction(RotateBy::create(0.f, 10.f));
		auto layer = this->getChildByName("LoadLayer");
		layer->setRotation(layer->getRotation() + 180.f*delta);
	}
#endif
}

/**
	@brief		レイヤー切り替え
	@param[in]	from	元レイヤー
	@param[in]	to		次レイヤー
	@return		なし
*/
void SceneAtelierMain::changeLayer(const std::string& from, const std::string& to)
{
	LayerState state = LayerState::INPUT;	// レイヤー状態
	bool isChange = false;
	GridLayer* layer;						// めくるレイヤー

	std::string visibleLayerName;			// 切り替えレイヤー名
	Phase nextPhase;						// 次フェーズ(レイヤー)

	ActionInterval* changeAction = nullptr;	// ページめくりアニメーション

	std::string stopBGMName;				// 止めるBGM
	std::string playBGMName;				// 再生するBGM

	// 設定→
	if (Phase::SETTING == m_phase)
	{
		layer = m_pLayers->getChildByName<SettingLayer*>(from);
		state = dynamic_cast<SettingLayer*>(layer)->getState();
		// 決定
		if (LayerState::NEXT == state)
		{
			m_phase = Phase::LAYER_CHANGE;	// レイヤー切り替えフェーズへ

			// GM代行時テーマ入力をスキップ
			if (m_isUseGMAgent)
			{
				// →メインレイヤー
				visibleLayerName = "GameMainLayer";
				nextPhase = Phase::MAIN;
				stopBGMName = "ModeSelectBGM.mp3";
				playBGMName = "GameMainBGM.mp3";
			}
			else
			{
				// →テーマ入力レイヤー
				visibleLayerName = "ThemeInputLayer";
				nextPhase = Phase::THEME_INPUT;
			}
		}
		else if (LayerState::BACK == state)
		{
			state = LayerState::NEXT;	// めくりアニメーションに戻す
			m_phase = Phase::LAYER_CHANGE;	// レイヤー切り替えフェーズへ
			// →オプションレイヤー
			visibleLayerName = "OptionLayer";
			nextPhase = Phase::OPTION;
		}

	}
	// テーマ入力→
	else if (Phase::THEME_INPUT == m_phase)
	{
		layer = m_pLayers->getChildByName<ThemeInputLayer*>(from);
		state = dynamic_cast<ThemeInputLayer*>(layer)->getState();
		// 決定
		if (LayerState::NEXT == state)
		{
			// レイヤー切り替え
			m_phase = Phase::LAYER_CHANGE;
			// →メインレイヤー
			visibleLayerName = "GameMainLayer";
			nextPhase = Phase::MAIN;
			changeAction = MoveBy::create(0.9f, Vec2(0.f, layer->getContentSize().height));

			stopBGMName = "ModeSelectBGM.mp3";
			playBGMName = "GameMainBGM.mp3";
		}
		// 戻る
		else if (LayerState::BACK == state)
		{
			// レイヤー切り替え
			m_phase = Phase::LAYER_CHANGE;
			// →設定レイヤー
			visibleLayerName = "SettingLayer";
			nextPhase = Phase::SETTING;
		}
	}	
	// オプション→
	else if (Phase::OPTION == m_phase)
	{
		layer = m_pLayers->getChildByName<OptionLayer*>(from);
		state = dynamic_cast<OptionLayer*>(layer)->getState();

		if (to == "GameMainLayer")
		{
			// 画伯メインへ
			// レイヤー切り替え
			m_phase = Phase::LAYER_CHANGE;
			// →メインレイヤー
			visibleLayerName = to;
			nextPhase = Phase::MAIN;
			state = LayerState::BACK;

		}
		// 決定
		else if (LayerState::NEXT == state)
		{
			// レイヤー切り替え
			m_phase = Phase::LAYER_CHANGE;
			// →設定レイヤー
			visibleLayerName = "SettingLayer";
			nextPhase = Phase::SETTING;

		}
		// 戻る
		else if (LayerState::BACK == state)
		{
			// レイヤー切り替え
			m_phase = Phase::LAYER_CHANGE;
			// →設定レイヤー
			visibleLayerName = "SettingLayer";
			nextPhase = Phase::SETTING;
		}
	}
	// 画伯メイン→
	else if (from == "GameMainLayer")
	{
		layer = m_pLayers->getChildByName<EseMainLayer*>(from);
		// →オプション
		if (to == "OptionLayer")
		{
			state = LayerState::NEXT;
			// レイヤー切り替え
			m_phase = Phase::LAYER_CHANGE;
			visibleLayerName = to;
			nextPhase = Phase::OPTION;
		}
		// →リピート
		else if (to == "RepeatLayer")
		{
			visibleLayerName = to;
			nextPhase = Phase::REPEAT;
			//------------------------------------------
			// 該当レイヤー以外非表示
			auto& layers = m_pLayers->getChildren();
			for (auto& l : layers)
			{
				const auto& name = l->getName();
				if (name != to && name != from)
					l->setVisible(false);
			}
			//------------------------------------------
			// 表示アクション
			auto visibleAction = CallFunc::create([&, layer, visibleLayerName]{
				auto visibleLayer = m_pLayers->getChildByName(visibleLayerName);
				// レイヤーとの重なり補正
				// 元レイヤーのz
				auto fromBuffer = layer->getLocalZOrder();
				// 予定レイヤーのz
				auto toBuffer = visibleLayer->getLocalZOrder();
				// toBufferが大きくあるように
				if (fromBuffer >= toBuffer)
				{
					if (fromBuffer == toBuffer)
					{
						toBuffer = fromBuffer + 1;
					}
					layer->setLocalZOrder(fromBuffer);
					visibleLayer->setLocalZOrder(toBuffer);
				}

				visibleLayer->setVisible(true);
			});
			// 切り替え

			// 表示後
			auto endAction = CallFunc::create([&, visibleLayerName, nextPhase, from]{
				m_phase = nextPhase;
				m_pLayers->getChildByName<GridLayer*>(visibleLayerName)->restart(from);
			});

			// テーマ入力レイヤーを非表示にし、次のレイヤーを表示
			layer->runAction(Sequence::create(
				visibleAction,
				endAction,
				nullptr));

			return;
		}
	}

	//if (LayerState::INPUT != state)
	{
		//------------------------------------------
		// 該当レイヤー以外非表示
		auto& layers = m_pLayers->getChildren();
		for (auto& l : layers)
		{
			const auto& name = l->getName();
			if (name != to && name != from)
				l->setVisible(false);
		}
		//------------------------------------------

		CallFunc* visibleAction;		// レイヤー表示
		CallFunc* endAction;			// レイヤー切り替え後
		CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("ButtonPushed.mp3");

		if (LayerState::NEXT == state)
		{

			// 表示アクション
			visibleAction = CallFunc::create([&, layer, visibleLayerName]{
				auto visibleLayer = m_pLayers->getChildByName(visibleLayerName);
				// レイヤーとの重なり補正
				// 元レイヤーのz
				auto fromBuffer = layer->getLocalZOrder();
				// 予定レイヤーのz
				auto toBuffer = visibleLayer->getLocalZOrder();
				// fromBufferが大きくあるように
				if (fromBuffer <= toBuffer)
				{
					if (fromBuffer == toBuffer)
					{
						fromBuffer = toBuffer + 1;
					}
					layer->setLocalZOrder(fromBuffer);
					visibleLayer->setLocalZOrder(toBuffer);
				}

				visibleLayer->setVisible(true);
			});

			// ページめくり
			if (!changeAction)
				changeAction = EaseCubicActionOut::create(PageTurn3D::create(1.5f, { 24, 32 }));

		}
		// 戻る
		else if (LayerState::BACK == state)
		{
			
			// 表示アクション
			visibleAction = CallFunc::create([&, layer, visibleLayerName]{
				auto visibleLayer = m_pLayers->getChildByName(visibleLayerName);
				// レイヤーとの重なり補正
				//visibleLayer->setLocalZOrder(layer->getLocalZOrder() + 1);
				// 元レイヤーのz
				auto fromBuffer = layer->getLocalZOrder();
				// 予定レイヤーのz
				auto toBuffer = visibleLayer->getLocalZOrder();
				// toBufferが大きくあるように
				if (fromBuffer >= toBuffer)
				{
					if (fromBuffer == toBuffer)
					{
						toBuffer = fromBuffer + 1;
					}
					layer->setLocalZOrder(fromBuffer);
					visibleLayer->setLocalZOrder(toBuffer);
				}

				visibleLayer->setVisible(true);
			});
			

			// ページもどし
			if (!changeAction)
				changeAction = TargetedAction::create(m_pLayers->getChildByName(to),
					ReverseTime::create(EaseCubicActionIn::create(PageTurn3D::create(1.5f, { 24, 32 }))));

		}

		// 表示後
		endAction = CallFunc::create([&, visibleLayerName, nextPhase, from, to, stopBGMName, playBGMName]{
			m_phase = nextPhase;
			m_pLayers->getChildByName<GridLayer*>(visibleLayerName)->restart(from);

			//if (to == "GameMainLayer")
			//{
				// 戻らないレイヤーの削除
				//m_pLayers->removeChildByName("SettingLayer");

			//}

			// BGMストップ
			if (!stopBGMName.empty())
			{
				//CocosDenshion::SimpleAudioEngine::getInstance()->stopBackgroundMusic();
			}
			// BGM再生
			if (!playBGMName.empty())
			{
				CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic(playBGMName.c_str(), true);
			}
		});

		// テーマ入力レイヤーを非表示にし、次のレイヤーを表示
		layer->runAction(Sequence::create(
			visibleAction,
			changeAction,
			endAction,
			Hide::create(),
			//RemoveSelf::create(true),
			nullptr));
	}
}


void SceneAtelierMain::toTitle()
{
	m_phase = Phase::SCENE_CHANGE;
	// シーン移行 
	Director::getInstance()->replaceScene(
		// ページめくり(第3引数trueだと被せる)
		TransitionPageTurn::create(2.0f, SceneTitle::createScene(), false));
}



/**
	@brief		メンバ変数初期化
	@param[in]	なし
	@return		なし
*/
void SceneAtelierMain::_SetDefaultValues()
{
	// ふえぇず
	m_phase = Phase::SETTING;

#if USE_MULTITHREAD
	m_isLoaded = false;
#endif
}

/**
	@brief		各種レイヤー読み込み処理
	@param[in]	mainName		ゲームメインのレイヤー名
	@return		なし
*/
void SceneAtelierMain::addLayer(const std::string& mainName)
{
#if USE_MULTITHREAD
	std::lock_guard<std::mutex> lock(m_mutex);
#endif
	//-----------------------------------------------------
	// レイヤーラッパー生成
	m_pLayers = Layer::create();
	m_pLayers->setVisible(false);
	this->addChild(m_pLayers, 0, "Layers");

	//// メインスレッドに戻す
	//{
	//	auto scheduler = Director::getInstance()->getScheduler();
	//	scheduler->performFunctionInCocosThread(CC_CALLBACK_0(SceneAtelierMain::_StartEseMain, this));
	//	return;
	//}
	//-----------------------------------------------------
	// 設定レイヤー初期化
	log("%-4d----------- SettingLayer::create -----------", __LINE__);
	auto settingLayer = SettingLayer::create();
	if (settingLayer)
	{
		settingLayer->setVisible(false);
		m_pLayers->addChild(settingLayer, 5, "SettingLayer");
		log("%-4d----------- SettingLayer::created -----------\n", __LINE__);
	}

	//-----------------------------------------------------
	// オプションレイヤー初期化
	log("%-4d----------- OptionLayer::create -----------", __LINE__);
	auto optionLayer = OptionLayer::create();
	if (optionLayer)
	{
		optionLayer->setVisible(false);
		m_pLayers->addChild(optionLayer, 5, "OptionLayer");
		log("%-4d----------- OptionLayer::created -----------\n", __LINE__);
	}

	//-----------------------------------------------------
	// レイヤー読み込み切り替え
	if (mainName == "Ese")
	{
		settingLayer->setMode(SceneModeSelect::Select::ESE);
		//-----------------------------------------------------
		// テーマ入力レイヤー初期化
		log("%-4d----------- EseThemeInputLayer::create -----------", __LINE__);
		auto themeInputLayer = EseThemeInputLayer::create();
		if (themeInputLayer)
		{
			themeInputLayer->setVisible(false);
			m_pLayers->addChild(themeInputLayer, 5, "ThemeInputLayer");
			log("%-4d----------- EseThemeInputLayer::created -----------\n", __LINE__);
		}

		//-----------------------------------------------------
		// メインレイヤー初期化
		log("%-4d----------- EseMainLayer::create -----------", __LINE__);
		auto eseMainLayer = EseMainLayer::create();
		if (eseMainLayer)
		{
			// 非表示
			eseMainLayer->setVisible(false);
			m_pLayers->addChild(eseMainLayer, 5, "GameMainLayer");
			log("%-4d----------- EseMainLayer::created -----------\n", __LINE__);
		}
	}
	else if (mainName == "CountUp")
	{
		settingLayer->setMode(SceneModeSelect::Select::COUNTUP);
		//-----------------------------------------------------
		// テーマ入力レイヤー初期化
		log("%-4d----------- CountUpThemeInputLayer::create -----------", __LINE__);
		auto themeInputLayer = CountUpThemeInputLayer::create();
		if (themeInputLayer)
		{
			themeInputLayer->setVisible(false);
			m_pLayers->addChild(themeInputLayer, 5, "ThemeInputLayer");
			log("%-4d----------- CountUpThemeInputLayer::created -----------\n", __LINE__);
		}

		//-----------------------------------------------------
		// メインレイヤー初期化
		log("%-4d----------- CountUpGameLayer::create -----------", __LINE__);
		auto gameMainLayer = CountUpGameLayer::create();
		if (gameMainLayer)
		{
			// 非表示
			gameMainLayer->setVisible(false);
			m_pLayers->addChild(gameMainLayer, 5, "GameMainLayer");
			log("%-4d----------- CountUpGameLayer::created -----------\n", __LINE__);
		}
	}
	else if (mainName == "CountDown")
	{
		settingLayer->setMode(SceneModeSelect::Select::COUNTDOWN);
		//-----------------------------------------------------
		// テーマ入力レイヤー初期化
		//log("%-4d----------- CountDownThemeInputLayer::create -----------", __LINE__);
		//auto themeInputLayer = CountDownThemeInputLayer::create();
		//if (themeInputLayer)
		//{
		//	themeInputLayer->setVisible(false);
		//	m_pLayers->addChild(themeInputLayer, 5, "ThemeInputLayer");
		//	log("%-4d----------- CountDownThemeInputLayer::created -----------\n", __LINE__);
		//}

		//-----------------------------------------------------
		// メインレイヤー初期化
		logline("----------- CountDownGameLayer::create -----------%s", "aaa");
		auto gameMainLayer = CountDownGameLayer::create();
		if (gameMainLayer)
		{
			// 非表示
			gameMainLayer->setVisible(false);
			m_pLayers->addChild(gameMainLayer, 5, "GameMainLayer");
			logline("----------- CountDownGameLayer::created -----------\n");
		}
	}
	else if (mainName == "Relay")
	{
		settingLayer->setMode(SceneModeSelect::Select::RELAY);
		//-----------------------------------------------------
		// テーマ入力レイヤー初期化
		logline("----------- RelayThemeInputLayer::create -----------");
		auto themeInputLayer = RelayThemeInputLayer::create();
		if (themeInputLayer)
		{
			themeInputLayer->setVisible(false);
			m_pLayers->addChild(themeInputLayer, 5, "ThemeInputLayer");
			logline("----------- RelayThemeInputLayer::created -----------\n");
		}

		//-----------------------------------------------------
		// メインレイヤー初期化
		logline("----------- RelayGameLayer::create -----------");
		auto gameMainLayer = RelayGameLayer::create();
		if (gameMainLayer)
		{
			// 非表示
			gameMainLayer->setVisible(false);
			m_pLayers->addChild(gameMainLayer, 5, "GameMainLayer");
			logline("----------- RelayGameLayer::created -----------\n");
		}
	}



	//-----------------------------------------------------
	// 繰り返しレイヤー初期化
	//log("%-4d----------- RepeatLayer::create -----------", __LINE__);
	//auto repeatLayer = RepeatLayer::create();
	//if (repeatLayer)
	//{
	//	// 非表示
	//	repeatLayer->setVisible(false);
	//	m_pLayers->addChild(repeatLayer, 5, "RepeatLayer");
	//	log("%-4d----------- RepeatLayer::created -----------\n", __LINE__);
	//}

#if USE_MULTITHREAD
	// メインスレッドに戻す
	auto scheduler = Director::getInstance()->getScheduler();
	scheduler->performFunctionInCocosThread(CC_CALLBACK_0(SceneAtelierMain::_StartEseMain, this));
#else

	// 開始
	this->_StartEseMain();
#endif
}


/**
	@brief		処理開始
	@param[in]	なし
	@return		なし
*/
void SceneAtelierMain::_StartEseMain()
{
#if USE_MULTITHREAD
	this->removeChildByName("LoadLayer");

	m_isLoaded = true;
#endif
	//-----------------------------------------------------
	// レイヤー表示
	m_pLayers->setVisible(true);

	//-----------------------------------------------------
	// タッチリスナー設定
	_SetTouchListener();



	//-----------------------------------------------------
	// 設定レイヤー表示
	m_pLayers->getChildByName<SettingLayer*>("SettingLayer")->restart("SceneAtelierMain");

}

/**
	@brief		タッチリスナーの設定
	@param[in]	なし
	@return		なし
*/
void SceneAtelierMain::_SetTouchListener()
{
	/*
	auto listener1 = EventListenerTouchOneByOne::create();
	auto dispatcher = Director::getInstance()->getEventDispatcher();

	//listener1->setSwallowTouches(true);

	// タッチした場合
	listener1->onTouchBegan = [&](Touch* touch, Event* event)
	{
		switch (m_phase)
		{
		case SceneAtelierMain::Phase::SETTING:
			// 設定レイヤー
		{
			//auto settingLayer = this->getChildByName<SettingLayer*>("SettingLayer");
			//if (settingLayer->isEnded())
			//{
			//	m_phase = Phase::LAYER_CHANGE;	// レイヤー切り替えフェーズへ

			//	std::string visibleLayerName;
			//	CallFunc* call_func;
			//	// GM代行時テーマ入力をスキップ
			//	if (m_isUseGMAgent)
			//	{		
			//		visibleLayerName = "GameMainLayer";
			//		call_func = CallFunc::create([&, visibleLayerName]{
			//			m_phase = Phase::MAIN;	// 直接メインへ
			//			this->getChildByName<EseMainLayer*>(visibleLayerName)->restart();
			//		});
			//	}
			//	else
			//	{
			//		visibleLayerName = "ThemeInputLayer";
			//		call_func = CallFunc::create([&, visibleLayerName]{
			//			m_phase = Phase::THEME_INPUT;	// テーマ入力へ
			//			this->getChildByName<ThemeInputLayer*>(visibleLayerName)->restart();
			//		});
			//	}
			//	
			//	// 設定レイヤーを非表示にし、次のレイヤーを表示
			//	settingLayer->runAction(Sequence::create(
			//		CallFunc::create([&, settingLayer, visibleLayerName]{
			//			auto layer = this->getChildByName(visibleLayerName);
			//			layer->setLocalZOrder(MAX(settingLayer->getLocalZOrder() - 1, 0));
			//			layer->setVisible(true);
			//		}),
			//		PageTurn3D::create(1.f, { 24, 32 }),
			//		call_func,
			//		Hide::create(),
			//		//RemoveSelf::create(true),
			//		NULL));
			//}
		}
			break;
		case SceneAtelierMain::Phase::OPTION:
			// オプション
			break;
		case SceneAtelierMain::Phase::THEME_INPUT:
			// テーマ入力
			break;
		case SceneAtelierMain::Phase::MAIN:
			// 画伯メイン
		{
			auto esemainLayer = m_pLayers->getChildByName<EseMainLayer*>("GameMainLayer");
			if (esemainLayer->isEnded())
			{
				m_phase = Phase::REPEAT;
				// メイン非表示
				esemainLayer->setVisible(false);
			}
		}
			break;
		case SceneAtelierMain::Phase::REPEAT:
			break;
		case SceneAtelierMain::Phase::SCENE_CHANGE:
			break;
		default:
			break;
		}


		return true;
	};

	// タッチが移動した場合
	listener1->onTouchMoved = [&](Touch* touch, Event* event)
	{
	};

	// タッチイベントが終了した場合
	listener1->onTouchEnded = [=](Touch* touch, Event* event)
	{
		// エフェクト作成
		// 作成したパーティクルのプロパティリストを読み込み
		auto particle1 = ParticleSystemQuad::create("particle_circle_stars.plist");
		
		auto particle2 = common::createParticle("particle_fireworks_stars.plist");
		
		// パーティクルを開始
		particle1->resetSystem();
		
		// パーティクルの再生が終了したら破棄するように
		particle1->setAutoRemoveOnFinish(true);
		
		particle1->setScale(0.5f);
		particle2->setScale(0.5f);
		
		// パーティクルを表示する場所の設定
		particle1->setPosition(touch->getLocation());
		particle2->setPosition(touch->getLocation());
		
		// パーティクルを配置
		this->addChild(particle1, 100);
		this->addChild(particle2, 100);
	};

	//_eventDispatcher->addEventListenerWithSceneGraphPriority(listener1, this);
	//*/
}



/**
	@brief		レイヤー切り替え後コールバック関数
	@param[in]	なし
	@return		なし
	@par		設定レイヤー非表示後に呼ばれる(メインレイヤーの表示)
*/
//void SceneAtelierMain::_CallBackChangeLayerToMain()
//{
//	m_phase = Phase::MAIN;
//	this->getChildByName<EseMainLayer*>("GameMainLayer")->start();
//}


#pragma endregion	// SceneAtelierMain methods









#pragma region SceneAtelierMain::SettingLayer methods



/**
	@brief		デストラクタ
	@param[in]	なし
	@return		なし
	@par		メモリの解放等
*/
SceneAtelierMain::SettingLayer::~SettingLayer()
{
	//m_GashoOnOff.clear();
}


// on "init" you need to initialize your instance
bool SceneAtelierMain::SettingLayer::init()
{
	//////////////////////////////
	// 1. super init first
	if (!GridLayer::init())
	{
		return false;
	}

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	_SetDefaultValues();


	//*
	// slect_back_
	// 黒　白　橙　黄
	auto spriteBack = Sprite::create("select_back_black.png");
	{
		float scaleX = visibleSize.width / spriteBack->getContentSize().width;
		float scaleY = visibleSize.height / spriteBack->getContentSize().height;
		spriteBack->setScale(scaleX, scaleY);

		// position the sprite on the center of the screen
		spriteBack->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));


		this->addChild(spriteBack, 0, "Back");
	}
	//*/

	//-----------------------------------------------------
	// プレイ人数
	auto nodePlayerNum = Node::create();
	{
		nodePlayerNum->setPosition(Vec2::ZERO);
		this->addChild(nodePlayerNum, 5, "PlayerNumNode");
	}

	auto spriteHumanIcon = Sprite::create("HumanIcon.png");
	//if (spriteHumanIcon)
	{
		spriteHumanIcon->setPosition(
			origin.x + visibleSize.width / 2.f,
			origin.y + visibleSize.height * 7.f / 9.f);
		nodePlayerNum->addChild(spriteHumanIcon, 5, "HumanIcon");
	}


	//-----------------------------------------------------
	// ラベル
	// ラベル, 座標, 名前
	//std::vector<std::tuple<Label*, Vec2, std::string>> labels = {
	//	std::make_tuple(
	//		Label::createWithSystemFont(std::to_string(m_playerNum), "GenShinGothic-Regular.ttf", 40),
	//		Vec2(visibleSize.width / 2.f + origin.x, visibleSize.height / 2.f + origin.y + 130),
	//		"PlayerNum"
	//		),
	//	std::make_tuple(
	//		Label::createWithSystemFont("プレイヤー数", "GenShinGothic-Regular.ttf", 40),
	//		Vec2(visibleSize.width / 2.f + origin.x, visibleSize.height / 2.f + origin.y + 250),
	//		"LabelPlayerNum"
	//		),
	//	std::make_tuple(
	//		Label::createWithSystemFont("ゲームマスター", "GenShinGothic-Regular.ttf", 40),
	//		Vec2(visibleSize.width / 2.f + origin.x, visibleSize.height / 2.f + origin.y),
	//		"LabelGameMaster"
	//		),
	//	std::make_tuple(
	//		Label::createWithSystemFont("役職：画商", "GenShinGothic-Regular.ttf", 40),
	//		Vec2(visibleSize.width / 2.f + origin.x, visibleSize.height / 2.f + origin.y - 150),
	//		"LabelGasho"
	//		),
	//};
	//
	//for (auto& label_tuple : labels)
	//{
	//	// ラベル
	//	auto& label = std::get<0>(label_tuple);
	//
	//	// 表示位置初期化
	//	label->setPosition(std::get<1>(label_tuple));
	//
	//	// 文字色指定
	//	//label->setColor(Color3B(5, 5, 5));
	//	label->setColor(Color3B(255, 255, 255));
	//
	//	// 透明度設定の初期化
	//	label->setOpacity(255);
	//
	//	this->addChild(label, 10, std::get<2>(label_tuple));
	//
	//	//m_settingNodes.emplace_back(label);
	//}
	//labels.clear();

	// 何かメモリリークが起こるので以下のゴリ押しで勘弁
	// プレイ人数
	auto labelPlayerNum = Label::createWithSystemFont(std::to_string(m_playerNum), "GenShinGothic-Regular.ttf", 40.f*MultiResolution::getFontScale());
	{
		labelPlayerNum->setPosition(spriteHumanIcon->getContentSize() / 2.f);
		labelPlayerNum->setAlignment(TextHAlignment::CENTER, TextVAlignment::CENTER);
		labelPlayerNum->setColor(Color3B(250, 250, 250));
		spriteHumanIcon->addChild(labelPlayerNum, 10, "PlayerNum");
		m_pPlayerLabel = labelPlayerNum;

		//--------------------------------
		// ボタン
		{
			auto spriteArrowLeft = ControlButton::create(Scale9Sprite::create("ArrowLeft.png"));
			auto spriteArrowRight = ControlButton::create(Scale9Sprite::create("ArrowRight.png"));
			auto sprite = Sprite::create("ArrowLeft.png");
			spriteArrowLeft->setPreferredSize(sprite->getContentSize());
			spriteArrowRight->setPreferredSize(sprite->getContentSize());

			Vec2 pos(spriteHumanIcon->getPosition());
			float margin = sprite->getContentSize().width * 2.f;

			spriteArrowLeft->setPosition(pos.x - margin,
										 pos.y);
			spriteArrowRight->setPosition(pos.x + margin,
										  pos.y);

			ADD_BUTTON_PUSHED(spriteArrowLeft, SceneAtelierMain::SettingLayer::_ControlButtonPushed);
			ADD_BUTTON_PUSHED(spriteArrowRight, SceneAtelierMain::SettingLayer::_ControlButtonPushed);

			// 子授り
			nodePlayerNum->addChild(spriteArrowLeft, 10, "ArrowLeft");
			nodePlayerNum->addChild(spriteArrowRight, 10, "ArrowRight");

		}
	}
	// プレイ人数ラベル
	{
		auto label = Label::createWithSystemFont("プレイヤー数", "GenShinGothic-Regular.ttf", 40.f*MultiResolution::getFontScale());
		label->setPosition(
			spriteHumanIcon->getPositionX(),
			spriteHumanIcon->getPositionY() + spriteHumanIcon->getContentSize().height*spriteHumanIcon->getAnchorPoint().y + label->getContentSize().height);
		label->setColor(Color3B(250, 250, 250));
		nodePlayerNum->addChild(label, 10, "LabelPlayerNum");
	}
	// ゲームマスター代行ラベル
	auto labelGM = Label::createWithSystemFont("ゲームマスター補助", "GenShinGothic-Regular.ttf", 40.f*MultiResolution::getFontScale());
	{
		auto anchor = labelGM->getAnchorPoint();
		labelGM->setPosition(
			origin.x + visibleSize.width / 2.f,
			origin.y + visibleSize.height * 5.f / 8.f);
		labelGM->setColor(Color3B(250, 250, 250));
		this->addChild(labelGM, 10, "LabelGameMaster");
	}
	// 描ける時間
	auto labelDrawableTime = Label::createWithSystemFont("000秒", "GenShinGothic-Medium.ttf", calcFontSize(40.f));
	{
		m_pDrawableTimeLabel = labelDrawableTime;
		auto node = Node::create();
		this->addChild(node, 10, "DrawableNode");
		node->setPosition(Vec2::ZERO);
		// 基本非表示
		node->setVisible(false);


		labelDrawableTime->setPosition(
			origin.x + visibleSize.width / 2.f,
			origin.y + visibleSize.height * 3.f / 8.f);

		labelDrawableTime->setAlignment(TextHAlignment::CENTER, TextVAlignment::CENTER);
		node->addChild(labelDrawableTime);
		//--------------------------------
		// ラベル
		auto label = Label::createWithSystemFont("プレイ時間", "GenShinGothic-Regular.ttf", calcFontSize(40.f));
		label->setPosition(
			labelDrawableTime->getPositionX(),
			labelDrawableTime->getPositionY() + label->getContentSize().height*1.f);
		label->setColor(Color3B(250, 250, 250));
		node->addChild(label, 10, "LabelPlayTime");

		//--------------------------------
		// ボタン
		{
			float posY(labelDrawableTime->getPositionY());

			auto spriteArrowLeft = ControlButton::create(Scale9Sprite::create("ArrowLeft.png"));
			auto spriteArrowRight = ControlButton::create(Scale9Sprite::create("ArrowRight.png"));
			auto sprite = Sprite::create("ArrowLeft.png");
			spriteArrowLeft->setPreferredSize(sprite->getContentSize());
			spriteArrowRight->setPreferredSize(sprite->getContentSize());

			spriteArrowLeft->setPosition(nodePlayerNum->getChildByName("ArrowLeft")->getPositionX(),
										 posY);
			spriteArrowRight->setPosition(nodePlayerNum->getChildByName("ArrowRight")->getPositionX(),
										  posY);

			ADD_BUTTON_PUSHED(spriteArrowLeft, SceneAtelierMain::SettingLayer::_ControlButtonPushed);
			ADD_BUTTON_PUSHED(spriteArrowRight, SceneAtelierMain::SettingLayer::_ControlButtonPushed);

			// 子授り
			node->addChild(spriteArrowLeft, 10, "TimeDown");
			node->addChild(spriteArrowRight, 10, "TimeUp");
		}

		labelDrawableTime->setString(std::to_string(m_drawableTime)+"秒");
	}
	// 画商使用ラベル
	//{
	//	auto label = Label::createWithSystemFont("役職：画商", "GenShinGothic-Regular.ttf", 40.f*MultiResolution::getFontScale());
	//	label->setPosition(
	//		visibleSize.width / 2.f + origin.x, visibleSize.height / 2.f + origin.y - 150);
	//	label->setColor(Color3B(255, 255, 255));
	//	label->setOpacity(255);
	//	this->addChild(label, 10, "LabelGasho");
	//}



	// 画商設定画像
	//std::vector<SpriteOnOff> sprites_gasho = {
	//	{
	//		Sprite::create("on.png"),	// ○true状態	(反転させる
	//		Sprite::create("maru_f.png"),	// ○false状態	(常に描画
	//	},
	//	{
	//		Sprite::create("off.png"),	// ×true状態	(反転させる
	//		Sprite::create("batu_f.png"),	// ×false状態	(常に描画
	//	},
	//};



	
	auto spriteOnOff = Sprite::create("on.png");
	//spriteOnOff->autorelease();
	float enablePosX;		// ○×ボタンの位置X座標
	//float disablePosX;		// ×ボタンの位置X座標
	//if (spriteOnOff)
	{
		float x = labelGM->getPositionX() + labelGM->getContentSize().width / 2.f;
		float w = spriteOnOff->getContentSize().width;
		enablePosX = x + w;
		//disablePosX = enablePosX + w*1.5f;
	}
	
	//----------------------------------------
	// GM代行オンオフボタン
	{
	
		auto spriteGMUse = ControlButton::create(Scale9Sprite::create("on.png"));
		auto spriteGMNoUse = ControlButton::create(Scale9Sprite::create("off.png"));
		
		spriteGMUse->setPreferredSize(spriteOnOff->getContentSize());
		spriteGMNoUse->setPreferredSize(spriteOnOff->getContentSize());
		
		Vec2 pos(enablePosX,
				 labelGM->getPositionY());
		spriteGMUse->setPosition(pos);
		spriteGMNoUse->setPosition(pos);
	
		ADD_BUTTON_PUSHED(spriteGMUse, SceneAtelierMain::SettingLayer::_ControlButtonPushed);
		ADD_BUTTON_PUSHED(spriteGMNoUse, SceneAtelierMain::SettingLayer::_ControlButtonPushed);
	
		// 子授かり
		this->addChild(spriteGMUse, 10, "UseGMAgent");
		this->addChild(spriteGMNoUse, 10, "NoUseGMAgent");
	
	}

	//----------------------------------------
	// 画商設定オンオフボタン
	//
	//i = 0;
	//for (auto& spriteOnOff : sprites_gasho)
	//{
	//	spriteOnOff.first->setPosition((i % 2) ? disablePosX : enablePosX, visibleSize.height / 2.f + origin.y - 160);
	//	spriteOnOff.second->setPosition((i % 2) ? disablePosX : enablePosX, visibleSize.height / 2.f + origin.y - 160);
	//
	//
	//	spriteOnOff.first->setName("Gasho");
	//	spriteOnOff.second->setName("Gasho");
	//
	//
	//
	//
	//	spriteOnOff.first->setVisible(m_isUseGasho);
	//	spriteOnOff.second->setVisible(true);
	//
	//
	//	// 子授り
	//	this->addChild(spriteOnOff.first, 10);
	//	this->addChild(spriteOnOff.second, 9);
	//
	//	//m_settingNodes.emplace_back(spriteOnOff.first);
	//	//m_settingNodes.emplace_back(spriteOnOff.second);
	//	++i;
	//}
	//m_GashoOnOff = sprites_gasho;
	//sprites_gasho.clear();


	
	// 決定ボタン
	auto itemDecide = MenuItemImage::create("decide.png", "decide2.png", "decide3.png",
											[&](Ref* pSender)
	{
		auto target = dynamic_cast<MenuItemImage*>(pSender);
		if (!target || m_state != LayerState::INPUT)
			return;
		else
		{
			// シーン終了
			// メインレイヤーへ
			m_state = LayerState::NEXT;
	
	
			// 人数、フラグの設定
			auto scene = dynamic_cast<SceneAtelierMain*>(this->getParent()->getParent());
			scene->setPlayerNum(m_playerNum);		// プレイ人数
			scene->setUseGMAgent(m_isUseGMAgent);	// GM代行
			scene->setUseGasho(m_isUseGasho);		// 画商

			if (SceneModeSelect::Select::COUNTDOWN == m_mode)
			{
				// カウントダウンゲーム時、ラベル適用
				this->getParent()->getChildByName<CountDownGameLayer*>("GameMainLayer")->setDrawableTime(static_cast<float>(m_drawableTime));
			}
	
			scene->changeLayer(this->getName(), "GameMainLayer");
	
		}
	});
	if(itemDecide)
	{
		// ボタン位置情報
		itemDecide->setPosition(
			origin.x + visibleSize.width / 2.f,
			+itemDecide->getContentSize().height);
		itemDecide->setName("Decide");
	}
	
	
	// オプション
	auto itemOption = MenuItemImage::create("SettingButton.png",
											"SettingButton2.png",
											[this](Ref* pSender)
	{
		auto target = dynamic_cast<MenuItemImage*>(pSender);
		if (!target || m_state != LayerState::INPUT)
			return;
		// オプションレイヤーへ
		m_state = LayerState::BACK;
		dynamic_cast<SceneAtelierMain*>(this->getParent()->getParent())->changeLayer(this->getName(), "OptionLayer");
	});
	if (itemOption)
	{
		//auto buttonOption = ControlButton::create(Scale9Sprite::create("settingButton.png"));
		//buttonOption->setPreferredSize(itemOption->getContentSize());
		//ADD_BUTTON_PUSHED(buttonOption, SceneAtelierMain::SettingLayer::_ControlButtonPushed);
	
		//// ボタン位置情報
		//buttonOption->setPosition(
		//	itemDecide->getPositionX()
		//		+ itemDecide->getContentSize().width*itemDecide->getAnchorPoint().x
		//		+ buttonOption->getContentSize().width,
		//	itemDecide->getPositionY());
		//
		//this->addChild(buttonOption, 10, "Option");
		itemOption->setPosition(
			itemDecide->getPositionX()
			+ itemDecide->getContentSize().width*itemDecide->getAnchorPoint().x
			+ itemOption->getContentSize().width,
			itemDecide->getPositionY());
		itemOption->setName("Option");
	}

	// 戻る
	auto itemSceneBack = MenuItemImage::create("BackButton.png",
											   "BackButton2.png",
											   [this](Ref* pSender)
	{
		auto target = dynamic_cast<MenuItemImage*>(pSender);
		if (!target || m_state != LayerState::INPUT)
			return;
		// SE
		CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("ButtonPushed.mp3");
		// モードセレクトへ
		m_state = LayerState::BACK;
		Director::getInstance()->replaceScene(
			TransitionFade::create(1.0f, SceneModeSelect::createScene(m_mode)));
	});
	//if (itemSceneBack)
	{
		itemSceneBack->setPosition(
			itemDecide->getPositionX()
			- itemDecide->getContentSize().width*itemDecide->getAnchorPoint().x
			- itemSceneBack->getContentSize().width,
			itemDecide->getPositionY());
		itemSceneBack->setName("SceneBack");
	}

	auto menu = Menu::create(itemSceneBack, itemDecide, itemOption, nullptr);
	{
		menu->setPosition(Vec2::ZERO);
		this->addChild(menu, 10, "Menu");
	}
	


	//-----------------------------------------------------
	// タッチリスナー設定
	_SetTouchListener();


	//-----------------------------------------------------
	// 毎フレームupdate呼び出し設定
	//	this->scheduleUpdate();

	return true;
}

void SceneAtelierMain::SettingLayer::setMode(SceneModeSelect::Select mode)
{
	auto centerY = Director::getInstance()->getVisibleSize().height / 2.f;
	m_mode = mode;
	switch (m_mode)
	{
	case SceneModeSelect::Select::ESE:
		m_minPlayerNum = 3;
		m_maxPlayerNum = 9;
		this->getChildByName("LabelGameMaster")->setPositionY(centerY);
		this->getChildByName("UseGMAgent")->setPositionY(centerY);
		this->getChildByName("NoUseGMAgent")->setPositionY(centerY);
		break;
	case SceneModeSelect::Select::COUNTUP:
		m_minPlayerNum = 2;
		m_maxPlayerNum = 9;
		this->getChildByName("LabelGameMaster")->setPositionY(centerY);
		this->getChildByName("UseGMAgent")->setPositionY(centerY);
		this->getChildByName("NoUseGMAgent")->setPositionY(centerY);
		break;
	case SceneModeSelect::Select::COUNTDOWN:
	{
		// 人数選択消す
		this->removeChildByName("PlayerNumNode");

		// GM代行強制設定
		m_isUseGMAgent = true;
		// ボタンを無効化して以降の切り替えを無くす
		auto button = this->getChildByName<ControlButton*>("UseGMAgent"); 
		button->setEnabled(false);
		button->setBackgroundSpriteForState(Scale9Sprite::create("OnDisabled.png"), Control::State::DISABLED);

		// 時間変更ボタン等表示
		this->getChildByName("DrawableNode")->setVisible(true);
	}
		break;

	case SceneModeSelect::Select::RELAY:
		// お絵かきリレーモード
	{
		m_minPlayerNum = 2;
		m_maxPlayerNum = 9;

		this->getChildByName("LabelGameMaster")->setPositionY(centerY);
		this->getChildByName("UseGMAgent")->setPositionY(centerY);
		this->getChildByName("NoUseGMAgent")->setPositionY(centerY);
	}
		break;
	default:
		break;
	}
}

/**
	@brief		設定レイヤー開始
	param[in]	from	元レイヤー
	@return		なし
*/
void SceneAtelierMain::SettingLayer::restart(const std::string& from)
{
	// 設定中
	m_state = LayerState::INPUT;
	
	//-----------------------------------------------------
	// 表示
	this->setVisible(true);

	//------------------------------
	// GMボタンのオンオフ
	this->getChildByName("UseGMAgent")->setVisible(m_isUseGMAgent);
	this->getChildByName("NoUseGMAgent")->setVisible(!m_isUseGMAgent);

}


/**
	@brief		メンバ変数初期化
	@param[in]	なし
	@return		なし
*/
void SceneAtelierMain::SettingLayer::_SetDefaultValues()
{
	// 設定中
	m_state = LayerState::INPUT;

	// プレイ人数(デフォルト:3)
	m_playerNum = 3;
	m_minPlayerNum = 3;
	m_maxPlayerNum = 9;

	// ゲームマスター代行
	m_isUseGMAgent = true;		// 使用
	// 画商
	m_isUseGasho = false;		// 使わない

	// お絵かき時間
	m_drawableTime = 60;		// 秒

}


/**
	@brief		タッチリスナーの設定
	@param[in]	なし
	@return		なし
*/
void SceneAtelierMain::SettingLayer::_SetTouchListener()
{
	// シングルタッチイベントリスナー作成
	auto listener1 = EventListenerTouchOneByOne::create();
	auto dispatcher = Director::getInstance()->getEventDispatcher();

	listener1->setSwallowTouches(false);	// シーンまでイベント透過

	// タッチした場合
	listener1->onTouchBegan = [&](Touch* touch, Event* event)
	{
		if (LayerState::INPUT != m_state)
			return false;
		auto target = event->getCurrentTarget();
		Point locationInNode = target->convertToNodeSpace(touch->getLocation());
		Size s = target->getContentSize();
		Rect rect = Rect(0, 0, s.width, s.height);

		// クリックエリアをチェック
		if (!rect.containsPoint(locationInNode))
		{
			return false;
		}

		const std::string& name = target->getName();

		//if (name == "Gasho")
		//{
		//	for (auto& sprite : m_GashoOnOff)
		//	{
		//		sprite.first->setVisible(!sprite.first->isVisible());
		//	}
		//}

		return true;
	};

	// タッチが移動した場合
	listener1->onTouchMoved = [&](Touch* touch, Event* event)
	{
	};

	// タッチイベントが終了した場合
	listener1->onTouchEnded = [=](Touch* touch, Event* event)
	{
	};

	//	sprite->onEnter




	auto children = this->getChildren();

	const char* names[] = {
		//"UseGMAgent",
		//"NoUseGMAgent",
		"Gasho",
		//"ArrowLeft",
		//"ArrowRight",
		//"Decide",
	};


	for (auto& name : names)
	{
		auto listener = listener1->clone();
		auto child = this->getChildByName(name);
		if (child)
		{
			dispatcher->addEventListenerWithSceneGraphPriority(listener, child);
		}
	}
}


/**
	@brief		ボタンタッチ
	@param[in]	pSender		ボタン
	@param[in]	eventType	イベントタイプ
	@return		なし
*/
void SceneAtelierMain::SettingLayer::_ControlButtonPushed(Ref* pSender, Control::EventType eventType)
{
	auto button = dynamic_cast<ControlButton*>(pSender);
	if (!button || m_state != LayerState::INPUT)
		return;
	// SE
	CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("ButtonPushed.mp3");

	const std::string name = button->getName();
	if ("ArrowLeft" == name)
	{
		// 人数減算
		if (m_playerNum > m_minPlayerNum)
		{
			m_playerNum--;
			m_pPlayerLabel->setString(std::to_string(m_playerNum));
			//if (4 > m_playerNum && !m_isUseGMAgent)
			//{
			//	this->getChildByName("MenuDecide")->getChildByName<MenuItemImage*>("Decide")->setEnabled(false);
			//}
		}
	}
	else if ("ArrowRight" == name)
	{
		// 人数加算
		if (m_playerNum < m_maxPlayerNum)
		{
			m_playerNum++;
			m_pPlayerLabel->setString(std::to_string(m_playerNum));
			//this->getChildByName("MenuDecide")->getChildByName<MenuItemImage*>("Decide")->setEnabled(true);
		}
	}
	else if ("TimeDown" == name)
	{
		// 時間減少
		const int d = 5;
		m_drawableTime = MAX(m_drawableTime - d, 10);
		m_pDrawableTimeLabel->setString(std::to_string(m_drawableTime) + "秒");
	}
	else if ("TimeUp" == name)
	{
		// 時間増加
		const int d = 5;
		m_drawableTime = MIN(m_drawableTime + d, 120);
		m_pDrawableTimeLabel->setString(std::to_string(m_drawableTime) + "秒");
	}
	else if ("UseGMAgent" == name)
	{
		// GM代行しない
		m_isUseGMAgent = false;
		button->setVisible(false);	//
		this->getChildByName("NoUseGMAgent")->setVisible(true);
		//if (4 > m_playerNum)
		//{
		//	this->getChildByName("MenuDecide")->getChildByName<MenuItemImage*>("Decide")->setEnabled(false);
		//}
	}
	else if ("NoUseGMAgent" == name)
	{
		// GM代行
		m_isUseGMAgent = true;
		button->setVisible(false);	//
		this->getChildByName("UseGMAgent")->setVisible(true);
		//this->getChildByName("MenuDecide")->getChildByName<MenuItemImage*>("Decide")->setEnabled(true);
	}

}





#pragma endregion	// SceneAtelierMain::SettingLayer methods




#pragma region SceneAtelierMain::ThemeInputLayer methods



/**
	@brief		初期化
	@param[in]	なし
	@return		初期化成功か
	@retval		true	初期化成功
	@retval		false	初期化失敗
*/
bool SceneAtelierMain::ThemeInputLayer::init()
{
	//////////////////////////////
	// 1. super init first
	if (!GridLayer::init())
	{
		return false;
	}

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();


	//*
	// slect_back_
	// 黒　白　橙　黄
	// 背景
	//auto spriteBack = Sprite::create("select_back_yellow.png");
	auto spriteBack = Sprite::create("BlackBoard.png");
	Vec2 scale(1.f, 1.f);
	if(spriteBack)
	{
		scale.x = visibleSize.width / spriteBack->getContentSize().width;
		scale.y = visibleSize.height / spriteBack->getContentSize().height;
		spriteBack->setScale(scale.x, scale.y);

		// position the sprite on the center of the screen
		spriteBack->setPosition(
			origin + visibleSize / 2);


		this->addChild(spriteBack, 0, "Back");
	}
	//*/

	//===============================================-
	// 文字アニメーション
	//if(false)
	auto spriteCharaTsuki = Sprite::create();
	auto spriteCharaNichi = Sprite::create();
	{
		const float w = 60.f, h = 70.f;
		//-------------------------------------
		// 月
		Vector<SpriteFrame*> sprites;
		for (int i = 0; i < 4; ++i)
		{
			auto rect = Rect(i*w, 0.f, w, h);
			sprites.pushBack(SpriteFrame::create("chara_tsuki.png", rect));
		}
		auto animationTsuki = Animation::createWithSpriteFrames(sprites);
		animationTsuki->setDelayPerUnit(0.5f);
		// アニメーション
		spriteCharaTsuki->setPosition(visibleSize.width - (w + 10.f),
									  visibleSize.height * 2.f / 3.f);
		spriteCharaTsuki->runAction(RepeatForever::create(Animate::create(animationTsuki)));
		this->addChild(spriteCharaTsuki);

		//-------------------------------------
		// 日
		sprites.clear();
		for (int i = 0; i < 4; ++i)
		{
			auto rect = Rect(i*w, 0.f, w, h);
			sprites.pushBack(SpriteFrame::create("chara_nichi.png", rect));
		}
		auto animationNichi = Animation::createWithSpriteFrames(sprites);
		animationNichi->setDelayPerUnit(0.5f);
		// アニメーション
		spriteCharaNichi->setPosition(visibleSize.width - (w + 10.f),
									  visibleSize.height * 1.f / 3.f);
		spriteCharaNichi->runAction(RepeatForever::create(Animate::create(animationNichi)));
		this->addChild(spriteCharaNichi);

	}
	//===============================================
	// 月日
	{
		const float w = 60.f, h = 70.f;
		// 月
		auto month = common::time::GetMonth();
		if (month <= 10)
		{
			// 1桁
			Rect rect((month - 1)*w, 0, w, h);
			auto sprite = Sprite::create("number.png", rect);

			Vec2 pos(spriteCharaTsuki->getPosition());
			pos.y += spriteCharaTsuki->getContentSize().height*spriteCharaTsuki->getAnchorPoint().y;
			pos.y += h;
			sprite->setPosition(pos);

			this->addChild(sprite, 100, "Month");
		}
		else
		{
			// 2桁
			{
				// 十
				Rect rect(10*w, 0, w, h);
				auto sprite = Sprite::create("number.png", rect);

				Vec2 pos(spriteCharaTsuki->getPosition());
				pos.y += spriteCharaTsuki->getContentSize().height*spriteCharaTsuki->getAnchorPoint().y;
				pos.y += h / 2.f;
				pos.y += (h)*2.f / 2.f;
				sprite->setPosition(pos);

				this->addChild(sprite, 100, "Month1");
			}

			{
				// 1の位
				Rect rect((month) * w, 0, w, h);
				auto sprite = Sprite::create("number.png", rect);

				Vec2 pos(spriteCharaTsuki->getPosition());
				pos.y += spriteCharaTsuki->getContentSize().height*spriteCharaTsuki->getAnchorPoint().y;
				pos.y += h / 2.f;
				pos.y += (h)*1.f / 2.f;
				sprite->setPosition(pos);

				this->addChild(sprite, 100, "Month2");
			}
		}

		//---------------------------------
		// 日
		auto day = common::time::GetDay();
		if (day)
		{
			if (day <= 10)
			{
				// 1桁
				Rect rect((day - 1)*w, 0, w, h);
				auto sprite = Sprite::create("number.png", rect);

				Vec2 pos(spriteCharaNichi->getPosition());
				pos.y += spriteCharaNichi->getContentSize().height*spriteCharaNichi->getAnchorPoint().y;
				pos.y += h;
				sprite->setPosition(pos);

				this->addChild(sprite, 100, "Day");
			}
			else
			{
				// 10の位
				auto d1 = day / 10;
				// 1の位
				auto d2 = day % 10;

				if (d2 == 0)
				{
					// 2桁 十で打ち切る
					{
						// 10の位
						Rect rect((10 + d1)* w, 0, w, h);
						auto sprite = Sprite::create("number.png", rect);

						Vec2 pos(spriteCharaNichi->getPosition());
						pos.y += spriteCharaNichi->getContentSize().height*spriteCharaNichi->getAnchorPoint().y;
						pos.y += h / 2.f;
						pos.y += (h)*2.f / 2.f;
						sprite->setPosition(pos);

						this->addChild(sprite, 100, "Day1");
					}
					{
						// 十
						Rect rect(10 * w, 0, w, h);
						auto sprite = Sprite::create("number.png", rect);

						Vec2 pos(spriteCharaNichi->getPosition());
						pos.y += spriteCharaNichi->getContentSize().height*spriteCharaNichi->getAnchorPoint().y;
						pos.y += h / 2.f;
						pos.y += (h)*1.f / 2.f;
						sprite->setPosition(pos);

						this->addChild(sprite, 100, "Day2");
					}
				}
				else
				{
					// 3桁
					{
						// 10の位
						Rect rect((10 + d1)* w, 0, w, h);
						auto sprite = Sprite::create("number.png", rect);

						Vec2 pos(spriteCharaNichi->getPosition());
						pos.y += spriteCharaNichi->getContentSize().height*spriteCharaNichi->getAnchorPoint().y;
						pos.y += h / 2.f;
						pos.y += (h)*3.f / 2.f;
						sprite->setPosition(pos);

						this->addChild(sprite, 100, "Day1");
					}
					{
						// 十
						Rect rect(10 * w, 0, w, h);
						auto sprite = Sprite::create("number.png", rect);

						Vec2 pos(spriteCharaNichi->getPosition());
						pos.y += spriteCharaNichi->getContentSize().height*spriteCharaNichi->getAnchorPoint().y;
						pos.y += h / 2.f;
						pos.y += (h)*2.f / 2.f;
						sprite->setPosition(pos);

						this->addChild(sprite, 100, "Day2");
					}
					{
						// 1の位
						Rect rect((10 + d2)* w, 0, w, h);
						auto sprite = Sprite::create("number.png", rect);

						Vec2 pos(spriteCharaNichi->getPosition());
						pos.y += spriteCharaNichi->getContentSize().height*spriteCharaNichi->getAnchorPoint().y;
						pos.y += h / 2.f;
						pos.y += (h)*1.f / 2.f;
						sprite->setPosition(pos);

						this->addChild(sprite, 100, "Day3");
					}
				}
			}
		}
	}


	//auto _editName = EditBox::create(editBoxSize, Scale9Sprite::create("EditBox.png"));
	//_editName->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height * 1 / 4));
	//_editName->setFontName("Paint Boy");
	//_editName->setFontSize(25);
	//_editName->setFontColor(Color3B::RED);
	//// 初めから入っている文字
	//_editName->setPlaceHolder("DECIMAL");
	//_editName->setPlaceholderFontColor(Color3B::BLACK);
	//// 入力可能文字数(半角、全角関係なしに、文字数)
	//_editName->setMaxLength(10);
	//// エンター押されたらどうする？(組み込んでみないとわかんね(´・ω・`))
	//_editName->setReturnType(EditBox::KeyboardReturnType::GO);
	//// キーボードのタイプ(実機のみ確認可能 日本語文字はSINGLE_LINE)
	//_editName->setInputMode(EditBox::InputMode::DECIMAL);
	//_editName->setDelegate(this);
	//addChild(_editName);


	//auto _editName2 = EditBox::create(editBoxSize, Scale9Sprite::create("EditBox.png"));
	//_editName2->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height * 2 / 4));
	//_editName2->setFontName("Paint Boy");
	//_editName2->setFontSize(25);
	//_editName2->setFontColor(Color3B::RED);
	//_editName2->setPlaceHolder("SINGLE_LINE");
	//_editName2->setPlaceholderFontColor(Color3B::BLACK);
	//_editName2->setMaxLength(20);
	//_editName2->setReturnType(EditBox::KeyboardReturnType::SEARCH);
	//_editName2->setInputMode(EditBox::InputMode::SINGLE_LINE);
	//_editName2->setDelegate(this);
	//_editName2->setTitleLabel(Label::createWithSystemFont("大テーマ","Meiryo",30));
	//addChild(_editName2, 0, "BigTheme");

	//auto _editName3 = EditBox::create(editBoxSize, Scale9Sprite::create("EditBox.png"));
	//_editName3->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height * 3 / 4));
	//_editName3->setFontName("Paint Boy");
	//_editName3->setFontSize(25);
	//_editName3->setFontColor(Color3B::RED);
	//_editName3->setPlaceHolder("PHONE_NUMBER");
	//_editName3->setPlaceholderFontColor(Color3B::BLACK);
	//_editName3->setMaxLength(20);
	//_editName3->setReturnType(EditBox::KeyboardReturnType::SEND);
	//_editName3->setInputMode(EditBox::InputMode::PHONE_NUMBER);
	//_editName3->setDelegate(this);
	//addChild(_editName3);


	//---------------------------------------------------------------------------------------------------

	auto item = MenuItemImage::create("BackButton.png", 
									  "BackButton2.png",
									  [&](Ref* pSender)
	{
		// 戻るボタン
		// 設定レイヤーへ
		// 入力中以外受け付けない
		if (m_state != LayerState::INPUT) return;

		// 状態変更
		m_state = LayerState::BACK;

		// アニメーション中の動作拒否
		this->_StartAnimation();

		// アニメーション開始
		dynamic_cast<SceneAtelierMain*>(this->getParent()->getParent())->changeLayer(this->getName(),"SettingLayer");
	});
	{
		item->setName("BackButton");
		item->setEnabled(false);
	}
	auto item2 = MenuItemImage::create("button.png",
									   "button2.png",
									   [&](Ref* pSender)
	{
		// 決定ボタン
		// 入力中以外受け付けない
		if (m_state != LayerState::INPUT) return;
		
		auto scene = dynamic_cast<SceneAtelierMain*>(this->getParent()->getParent());
		// テーマ設定
		bool err = false;
		for (bool e : m_errorList)
		{
			err |= e;
		}
		if (err)
		{
			// エラーあり
			m_state = LayerState::INPUT;
			
		}
		else
		{
			// エラーなし
			// 画伯メインへ
			// 状態変更
			m_state = LayerState::NEXT;

			this->_StartAnimation();
			// アニメーション開始
			scene->changeLayer(this->getName(), "GameMainLayer");

		}
		//bool err[2] = { false, false };	// エラー検出用
		//auto bigThemeBox = this->getChildByName<EditBox*>("BigThemeBox");
		//if (bigThemeBox)
		//{
		//	std::string theme(bigThemeBox->getText());
		//	if (theme.empty())
		//	{
		//		// 文字列が空
		//		err[0] = true;
		//	}
		//	else
		//	{
		//		scene->setBigTheme(theme);
		//	}
		//}
		//auto smallThemeBox = this->getChildByName<EditBox*>("SmallThemeBox");
		//if (smallThemeBox)
		//{
		//	std::string theme(smallThemeBox->getText());
		//	if (theme.empty())
		//	{
		//		// 文字列が空
		//		err[1] = true;
		//	}
		//	else
		//	{
		//		scene->setSmallTheme(theme);
		//	}
		//
		//}
		//if (!err[0] && !err[1])
		//{
		//	// エラーなし
		//	// 画伯メインへ
		//	// 状態変更
		//	m_state = LayerState::NEXT;
		//	// アニメーション中の動作拒否
		//	this->getChildByName<EditBox*>("BigThemeBox")->setEnabled(false);
		//	this->getChildByName<EditBox*>("SmallThemeBox")->setEnabled(false);
		//
		//	// アニメーション開始
		//	scene->changeLayer(this->getName(), "GameMainLayer");
		//}
		//else
		//{
		//	// エラーあり
		//	m_state = LayerState::INPUT;
		//	// エラー通知
		//	//if ()
		//}
	});
	if (item2)
	{
		item2->setAnchorPoint(Vec2(0.5f, 0.5f));
		
	}

	auto menu = Menu::create(item, item2, nullptr);
	if(menu)
	{
		// // 横一列,間隔
		menu->alignItemsHorizontallyWithPadding(150.f);

		auto h = MAX(item->getContentSize().height, 
					 item2->getContentSize().height);

		menu->setPosition(origin.x + visibleSize.width / 2.f,
						  origin.y + h / 2.f /*+ 10.f*/);

		auto label = Label::createWithSystemFont("決定", "GenShinGothic-Medium.ttf", 20.f*MultiResolution::getFontScale());
		label->setAnchorPoint(Vec2(0.5f, 0.5f));
		label->setPosition(menu->getPosition() + item2->getPosition());
		this->addChild(label,99, "Label");

		this->addChild(menu, 0, "Buttons");
	}
	return this->_Init();
}

//---------------------------------------------------------------------------------------------------
		// ボックス表示時に呼ばれる
void SceneAtelierMain::ThemeInputLayer::editBoxEditingDidBegin(EditBox* editBox)
{
	log("editBox %p DidBegin !", editBox);
}
// OKボタン押したら呼ばれる
void SceneAtelierMain::ThemeInputLayer::editBoxTextChanged(EditBox* editBox, const std::string& text)
{
	log("editBox %p TextChanged, text: %s ", editBox, text.c_str());
}

// ボックス終了時に呼ばれる
void SceneAtelierMain::ThemeInputLayer::editBoxEditingDidEnd(EditBox* editBox)
{
	log("editBox %p DidEnd !", editBox);
}

// ボックス終了後に呼ばれる
void SceneAtelierMain::ThemeInputLayer::editBoxReturn(EditBox* editBox)
{
	log("editBox %p was returned !", editBox);

}
//---------------------------------------------------------------------------------------------------

/**
	@brief		入力レイヤー開始
	@param[in]	from	元レイヤー
	@return		なし
*/
void SceneAtelierMain::ThemeInputLayer::restart(const std::string& from)
{
	m_state = LayerState::INPUT;
	
	//-----------------------------------------------------
	// 表示
	this->setVisible(true);

	this->getChildByName("Buttons")->getChildByName<MenuItemImage*>("BackButton")->setEnabled(true);

}

#pragma endregion	// SceneAtelierMain::ThemeInputLayer methods






#pragma region SceneAtelierMain::RepeatLayer methods

/**
	@brief		デストラクタ
	@param[in]	なし
	@return		なし
	@par		メモリの解放等
*/
SceneAtelierMain::RepeatLayer::~RepeatLayer()
{
}


// on "init" you need to initialize your instance
bool SceneAtelierMain::RepeatLayer::init()
{
	//////////////////////////////
	// 1. super init first
	if (!GridLayer::init())
	{
		return false;
	}

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	_SetDefaultValues();


	auto label = Label::createWithSystemFont("エンドだよー", "Meiryo", 40*MultiResolution::getFontScale());
	this->addChild(label, 5);

	
	
	//-----------------------------------------------------
	// タッチリスナー設定
	_SetTouchListener();


	//-----------------------------------------------------
	// 毎フレームupdate呼び出し設定
	//	this->scheduleUpdate();

	return true;
}



/**
	@brief		繰り返しレイヤー開始
	@param[in]	from	元レイヤー
	@return		なし
*/
void SceneAtelierMain::RepeatLayer::restart(const std::string& from)
{
	//-----------------------------------------------------
	// 表示
	this->setVisible(true);


	auto animation = Animation::create();

	auto base = Layer::create();
	this->addChild(base, 999);
	{
		const float width = 640.f;
		const float height = 480.f;
		for (int i = 0; i < 10; ++i)
		{
			Rect rect(0.f, i*height, width, height);
			animation->addSpriteFrame(SpriteFrame::create("pipo-curtain1.png", rect));
		}
	}
	animation->setDelayPerUnit(0.1f);

	auto sequence = Sequence::create(
		Animate::create(animation),
		RemoveSelf::create(),
		nullptr);
	base->runAction(sequence);
}


/**
	@brief		メンバ変数初期化
	@param[in]	なし
	@return		なし
*/
void SceneAtelierMain::RepeatLayer::_SetDefaultValues()
{
	
}


/**
	@brief		タッチリスナーの設定
	@param[in]	なし
	@return		なし
*/
void SceneAtelierMain::RepeatLayer::_SetTouchListener()
{
	// シングルタッチイベントリスナー作成
	auto listener1 = EventListenerTouchOneByOne::create();
	auto dispatcher = Director::getInstance()->getEventDispatcher();

	listener1->setSwallowTouches(false);	// シーンまでイベント透過

	// タッチした場合
	listener1->onTouchBegan = [&](Touch* touch, Event* event)
	{
		auto target = event->getCurrentTarget();
		Point locationInNode = target->convertToNodeSpace(touch->getLocation());
		Size s = target->getContentSize();
		Rect rect = Rect(0, 0, s.width, s.height);

		// クリックエリアをチェック
		if (!rect.containsPoint(locationInNode))
		{
			return false;
		}

		
		return true;
	};

	// タッチが移動した場合
	listener1->onTouchMoved = [&](Touch* touch, Event* event)
	{
	};

	// タッチイベントが終了した場合
	listener1->onTouchEnded = [=](Touch* touch, Event* event)
	{
	};

	dispatcher->addEventListenerWithSceneGraphPriority(listener1, this);
	
}


/**
	@brief		ボタンタッチ
	@param[in]	pSender		ボタン
	@param[in]	eventType	イベントタイプ
	@return		なし
*/
void SceneAtelierMain::RepeatLayer::_ControlButtonPushed(Ref* pSender, Control::EventType eventType)
{
	auto button = dynamic_cast<ControlButton*>(pSender);
	if (!button)
		return;

	std::string name = button->getName();
	
}





#pragma endregion	// SceneAtelierMain::RepeatLayer methods

