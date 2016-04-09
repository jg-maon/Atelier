/**=============================================================================
	@file		SceneModeSelect.cpp
	@brief		モードセレクトクラス 実装
	@author		Ken Fujishima	Masato Yamamoto
	@date		2015-03-11
			
	@par		[説明]
				モードセレクト

	@note

		Copyright (C) 2015  saitsuyo
=============================================================================*/

// インクルード
#include "SceneModeSelect.h"
#include "SceneAtelierMain.h"
#include "OptionLayer.h"
#include "MultiResolution.h"
//#include "SceneTutorial.h"

#include "Common.h"

#pragma execution_character_set("utf-8")

USING_NS_CC;
USING_NS_CC_EXT;

#pragma region	SceneModeSelect methods

Scene* SceneModeSelect::createScene()
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = SceneModeSelect::create();

	// add layer as a child to scene
	scene->addChild(layer);
	scene->setCascadeOpacityEnabled(true);
	// return the scene
	return scene;
}

Scene* SceneModeSelect::createScene(Select fromScene)
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = SceneModeSelect::create();

	layer->initPos(fromScene);

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}


/**
	@brief		デストラクタ
	@param[in]	なし
	@return		なし
*/
SceneModeSelect::~SceneModeSelect()
{
	// ボタン画像キャッシュのクリア
	m_buttonSprites.clear();
	// 扇画像キャッシュのクリア
	m_pieSprites.clear();	
}


/**
	@brief		初期化
	@param[in]	なし
	@return		初期化成功したか
	@retval		true	成功
	@rerval		false	失敗
*/
bool SceneModeSelect::init()
{
	//////////////////////////////
	// 1. super init first
	if (!Layer::init())
	{
		return false;
	}

	auto visibleSize = Director::getInstance()->getVisibleSize();
	Point origin = Director::getInstance()->getVisibleOrigin();

	// add a "close" icon to exit the progress. it's an autorelease object
	auto closeItem = MenuItemImage::create(	"CloseNormal.png",
											"CloseSelected.png",
											[&](Ref* pSender){
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WP8) || (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
		MessageBox("You pressed the close button. Windows Store Apps do not implement a close button.", "Alert");
		return;
#endif
		Director::getInstance()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
		exit(0);
#endif
	});

	closeItem->setPosition(origin.x + visibleSize.width - closeItem->getContentSize().width / 2,
						   origin.y + closeItem->getContentSize().height / 2);

	// create menu, it's an autorelease object
	auto menu = Menu::create(closeItem, nullptr);
	menu->setPosition(Vec2::ZERO);
	this->addChild(menu, 1000000);
	
	/////////////////////////////
	// 3. add your codes below...

	this->_SetDefaultValues();

	//------------------------
	// レイヤー
	// 動かさない背景画像
	auto BackSprite = Sprite::create("ModeSelectBack.png");
	{
		BackSprite->setPosition(visibleSize / 2.f);
		this->addChild(BackSprite, 1);
	}

	//------------------------
	// スクロールビュー

	auto tmp = Sprite::create("FrameButton.png");
	auto tmpSize = tmp->getContentSize();
	const float marginX = tmpSize.width / 1.5f;	// 余白

	// スクロールビューに入れるスプライトを用意
	auto scrollSprite = Layer::create();
	scrollSprite->setContentSize(cocos2d::Size(
		visibleSize.width / 2.f + (tmpSize.width + marginX)*Select::NUM - (tmpSize.width / 2.f *float(Select::NUM - 1) / Select::NUM),
		visibleSize.height));
	// フェードアウト用親ノードの透明度を子ノードへ反映設定
	scrollSprite->setCascadeOpacityEnabled(true);
	

	// 画面サイズでスクロールビューを作る
	auto scrollView = ScrollView::create(visibleSize);
	// シーンにレイヤーを追加する
	this->addChild(scrollView, 100, "ScrollView");
	// レイヤーをデリゲートに設定
	scrollView->setDelegate(this);

	scrollView->setContainer(scrollSprite);
	scrollView->setPosition(origin);
	scrollView->setContentSize(scrollSprite->getContentSize());

	// スクロールビューを横移動のみに設定
	scrollView->setDirection(ScrollView::Direction::HORIZONTAL);
	// バウンド処理をなくすことで画面外の描画を表示させない
	scrollView->setBounceable(true);

	scrollView->setCascadeOpacityEnabled(true);

	//------------------------
	// スプライト
	{
		// 選択ボタン
		std::vector<std::string> btnSprites = {
			"CountUpIllustBack.png",
			"CountDownIllustBack.png",
			"FrameButton.png",				//!< アニメーションが上書きされる
			"OptionIllust.png",
			"FrameButton.png",			//!< 非表示にする
		};
		m_buttonSprites.assign(Select::ESE, nullptr);
		int i = 0;
		const float w = 576.f, h = 240.f;
		for(auto& frame : m_buttonSprites)
		{
			const float illustPosY = origin.y + visibleSize.height / 10.f * 7.f;
			// 額縁
			frame = Sprite::create(btnSprites[i]);
			frame->setPosition(
				origin.x + visibleSize.width / 2 + (i * (tmpSize.width + marginX)),
				illustPosY);
			frame->setCascadeOpacityEnabled(true);
			scrollSprite->addChild(frame, 10, i);

			// 説明
			auto rect = Rect(i*w, 0.f, w, h);
			auto spriteDesc = Sprite::create("ModeDescription.png", rect);
			
			spriteDesc->setPosition(
				frame->getPositionX(),
				origin.y + visibleSize.height / 20 * 5);
			scrollSprite->addChild(spriteDesc, 10, i++);

		}
	}
	{

		//////////////////////////////////////////////////////////////////////////////////////////////////////////
		// デジタル数字(カウントアップで使用)
		{
			auto spriteCountUp = m_buttonSprites[Select::COUNTUP];
			auto size = spriteCountUp->getContentSize();
			// 桁数
			const int	DIGIT_NUM = 7;
			// 更新間隔 負数は更新なし(:や.)
			const float CountTime[DIGIT_NUM] = { 60.f, -1.f, 10.f, 1.f, -1.f, 0.1f, 0.01f };
			// 分割数もしくは画像番号
			const int	DivisionNumber[DIGIT_NUM] = { 10, 10, 6, 10, 11, 10, 10 };

			// 分割サイズ
			const float w = 56.f, h = 90.f;
			for (int i = 0; i < DIGIT_NUM; i++)
			{
				Vector<SpriteFrame*> spritesReturn;
				for (int j = 0; j < DivisionNumber[i]; ++j)
				{
					// コロンもしくはピリオドなら
					if (CountTime[i] < 0)
					{
						Rect rect(DivisionNumber[i] * w, 0.f, w, h);
						spritesReturn.pushBack(SpriteFrame::create("DegitalNumber.png", rect));
						break;
					}
					Rect rect(j*w, 0.f, w, h);
					spritesReturn.pushBack(SpriteFrame::create("DegitalNumber.png", rect));

				}
				auto animationReturn = Animation::createWithSpriteFrames(spritesReturn);

				// 1.fで1s毎
				animationReturn->setDelayPerUnit(CountTime[i]);
				auto spriteCharaNo = Sprite::create();
				spriteCharaNo->setColor(Color3B(5, 5, 5));
				spriteCharaNo->setPosition(
					size.width / 2.f - (DIGIT_NUM / 2 - i) * w,
					h*1.25f);
				// アニメーション
				spriteCharaNo->runAction(RepeatForever::create(Animate::create(animationReturn)));
				//spriteCharaNo->setName("Number");

				spriteCountUp->addChild(spriteCharaNo, 10, "Number");
			}
		}

		//////////////////////////////////////////////////////////////////////////////////////////////////////////
		// 時計(カウントダウンで使用)
		{

			// ボタン画像のサイズ取得
			auto spriteCountDown = m_buttonSprites[Select::COUNTDOWN];
			spriteCountDown->setCascadeOpacityEnabled(true);
			auto buttonSize = spriteCountDown->getContentSize();

			// 針画像,名前
			std::string needleFiles[][2] = {
				{ "minutes.png", "minutes" },
				{ "hour.png", "hour" },
				{ "seconds.png", "seconds" },
			};

			// 針の中心までのオフセット(実値)
			const Vec2 offset(242.f, buttonSize.height - 197.f);

			int i = 0;
			for (const auto& fileName : needleFiles)
			{
				auto sprite = Sprite::create(fileName[0]);
				// セットポジション
				sprite->setPosition(offset);
				sprite->setTag(i++);
				// 子授かり
				spriteCountDown->addChild(sprite, 15, fileName[1]);
			}

			// カウントダウンバー(透過ゲージ)12枚
			{
				// 黒い画像初期化
				m_maskCnt = 0;
				const int pieAngle = 30; // 角度
				m_maskNum = 360 / pieAngle;
				// 一周分追加1枚30度なので360度分
				for (int j = 0; j < m_maskNum; j++)
				{
					auto pie = Sprite::create("pie30.png");
					m_pieSprites.push_back(pie);

					// 反時計周りに回転し配置
					pie->setRotation(360.f - (j * pieAngle));
					pie->setPosition(offset);
					
					// 非表示(時間経過で表示させるため)
					pie->setVisible(false);
					// 子授かり
					spriteCountDown->addChild(pie, 10, i++);

				}
			}
			
		}
		//////////////////////////////////////////////////////////////////////////////////////////////////////////
		// ダイエット
		{
			// アニメーション用スプライト
			auto spriteRelay = m_buttonSprites[Select::RELAY];
			const Size size = spriteRelay->getContentSize();
			Vector<SpriteFrame*> sprites;
			for (int i = 0; i < 4; ++i)
			{
				auto rect = Rect(i*size.width, 0.f, size.width, size.height);
				sprites.pushBack(SpriteFrame::create("RelayIllust.png", rect));
			}
			auto animationRelay = Animation::createWithSpriteFrames(sprites);
			animationRelay->setDelayPerUnit(1.f);

			// アニメーション
			spriteRelay->runAction(RepeatForever::create(Animate::create(animationRelay)));
			
		}
	}

	
	// 画伯のみ非表示
	m_buttonSprites[Select::ESE-1]->setVisible(false);
	
	// 1秒ごとに呼び出し
	this->schedule(schedule_selector(SceneModeSelect::_OneSecondTimer), 1.f);


	//====================================================
	// タッチリスナー
	this->_SetTouchListener();



	//====================================================
	// BGM
	// すでにモードセレクトのBGMが再生されていたら続行、されていなかったら新規に再生
	if (!CocosDenshion::SimpleAudioEngine::getInstance()->isBackgroundMusicPlaying())
		CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic("ModeSelectBGM.mp3", true);

	return true;
}

/**
	@brief		スクロール初期位置
	@attention	Select::NUMは指定しないこと。(動くけどバグる)
	@param[in]	fromScene	入ってきたシーン
	@return		なし
*/
void SceneModeSelect::initPos(SceneModeSelect::Select fromScene)
{
	auto scroll = this->getChildByName<ScrollView*>("ScrollView")->getContainer();
	int index = MIN(Select::NUM - 1, fromScene);	// 隠し対策
	auto tmp = Sprite::create("FrameButton.png");
	auto tmpSize = tmp->getContentSize();
	const float marginX = tmpSize.width / 1.5f;	// 余白

	scroll->setPositionX(-(tmpSize.width + marginX)*index);
}



/**
	@brief		メンバ変数初期化
	@param[in]	なし
	@return		なし
*/
void SceneModeSelect::_SetDefaultValues()
{
	m_isSceneChangeCanceled = false;
	m_isSceneChanging = false;
}



/**
	@brief		タッチリスナーの設定
	@param[in]	なし
	@return		なし
*/
void SceneModeSelect::_SetTouchListener()
{
	// タッチ
	auto listener = EventListenerTouchOneByOne::create();


	// タッチした場合
	listener->onTouchBegan = [&](Touch* touch, Event* event)
	{
		if (m_isSceneChanging)
			return false;
		auto target = static_cast<Sprite*>(event->getCurrentTarget());
		Point locationInNode = target->convertToNodeSpace(touch->getLocation());
		Size s = target->getContentSize();
		Rect rect = Rect(0, 0, s.width, s.height);

		// タッチした座標取得
		m_touchBeganPoint = touch->getLocation();


		// クリックエリアをチェック
		if (rect.containsPoint(locationInNode))
		{
			// 不透明度変更
			target->setOpacity(180);

			// キャンセルしない
			m_isSceneChangeCanceled = false;

			return true;
		}

		return false;
	};

	// タッチが移動した場合
	listener->onTouchMoved = [&](Touch* touch, Event* event)
	{
		const float distance = 50.f;
		const float distance2 = distance*distance;

		auto touchPos = touch->getLocation();

		const auto dist = m_touchBeganPoint - touchPos;
		
		// 移動してたらキャンセル
		if (dist.x*dist.x > distance2)
		{
			m_isSceneChangeCanceled = true;
		}
		else if (dist.y*dist.y > distance2)
		{
			m_isSceneChangeCanceled = true;
		}
		
		if (m_isSceneChangeCanceled)
		{
			event->getCurrentTarget()->setOpacity(255);
		}

	};

	// タッチイベントが終了した場合
	listener->onTouchEnded = [=](Touch* touch, Event* event)
	{
		if (m_isSceneChangeCanceled)
			return;
		//不透明度を元に
		event->getCurrentTarget()->setOpacity(255);


		// ボタンSE
		CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("ModeSelected.mp3");

		this->_StartChangeSceneAnimation(event->getCurrentTarget());
	};


	//for (int i = 0; i < Select::NUM; i++)
	for (auto& sprite : m_buttonSprites)
	{
		auto clone = listener->clone();
		//_eventDispatcher->addEventListenerWithSceneGraphPriority(clone, m_buttonSprites[i]);
		_eventDispatcher->addEventListenerWithSceneGraphPriority(clone, sprite);
	}
}


/**
	@brief		シーン切り替えアニメーションの開始
	@param[in]	currentTarget
	@return		なし
*/
void SceneModeSelect::_StartChangeSceneAnimation(Node* currentTarget)
{
	m_isSceneChanging = true;
	auto target = static_cast<Sprite*>(currentTarget);

	// シーン移動に使う時間
	const float duration = 1.f;


	// 押された絵に近づく
	auto zoom = CallFunc::create([this, target, duration](){
		auto view = this->getChildByName<ScrollView*>("ScrollView");

		auto con = view->getContainer();

		auto no = target->getTag();

		auto size = m_buttonSprites[0]->getContentSize();

		auto marginX = size.width / 1.5f;

		float posX = -(size.width + marginX)*no;

		con->runAction(MoveTo::create(duration - 0.3f, Vec2(posX, size.height - target->getPositionY())));
	});

	// 親ノードの透明度を子ノードへ反映設定(FadeOutのため)
	this->setCascadeOpacityEnabled(true);

	this->runAction(
		Sequence::create(
			Spawn::create(
				zoom,
				EaseIn::create(ScaleBy::create(duration - 0.3f, 3.f), 2.f),
				//FadeOut::create(duration),
			//	nullptr
			//),
			CallFunc::create([this, duration, target](){
				//this->setOpacity(0);
				//Scene* scene;
				//終了した場合の処理
				switch (target->getTag())
				{
				case Select::COUNTUP:	// カウントアップゲーム
					//Director::getInstance()->replaceScene(scene = SceneAtelierMain::createScene("CountUp"));
					Director::getInstance()->replaceScene(TransitionFade::create(duration, SceneAtelierMain::createScene("CountUp")));
					break;
				case Select::COUNTDOWN:	// カウントダウンゲーム
					//Director::getInstance()->replaceScene(scene = SceneAtelierMain::createScene("CountDown"));
					Director::getInstance()->replaceScene(TransitionFade::create(duration, SceneAtelierMain::createScene("CountDown")));
					break;
				case Select::RELAY:	// お絵かきリレーゲーム
					//Director::getInstance()->replaceScene(scene = SceneAtelierMain::createScene("Relay"));
					Director::getInstance()->replaceScene(TransitionFade::create(duration, SceneAtelierMain::createScene("Relay")));
					break;
				case Select::OPTION: // オプション
					//Director::getInstance()->replaceScene(scene = SceneOption::createScene());
					Director::getInstance()->replaceScene(TransitionFade::create(duration, SceneOption::createScene()));
					break;
				case Select::ESE - 1:	// 画伯あぶり出し
					//Director::getInstance()->replaceScene(scene = SceneAtelierMain::createScene("Ese"));
					Director::getInstance()->replaceScene(TransitionFade::create(duration, SceneAtelierMain::createScene("Ese")));
					break;
				default:
					break;
				}
			}),
				nullptr),
			nullptr
		)
	);
	
}

/**
	@brief		更新
	@param[in]	delta	更新間隔(1秒おき)
	@return		なし
*/
void SceneModeSelect::_OneSecondTimer(float delta)
{
	m_pieSprites[m_maskCnt]->setVisible(true);
	m_maskCnt++;	// マスク画像数増加
	if (m_maskCnt >= m_maskNum)
	{
		// リセット
		m_maskCnt = 0;
		for (auto& pie : m_pieSprites)
		{
			pie->setVisible(false);
		}
	}

	// 長針移動
	auto sprite = m_buttonSprites[Select::COUNTDOWN]->getChildByName("minutes");
	sprite->setRotation(360.f - (m_maskCnt * 30.f));
		
}

#pragma endregion	// SceneModeSelect methods

