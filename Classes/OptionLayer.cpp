/**=============================================================================
	@file		OptionLayer.cpp
	@brief		オプションレイヤークラス 実装
	@author		Masato Yamamoto
	@date		2015-02-08
			
	@par		[説明]
				音量調整、フラグ管理オプションレイヤー
	@note

		Copyright (C) 2015  saitsuyo
=============================================================================*/

// インクルード
#include "OptionLayer.h"

#include "SimpleAudioEngine.h"	// 音関係
USING_NS_CC;

#include "MultiResolution.h"

#include "SceneModeSelect.h"

#include "Gauge.h"

#pragma region OptionLayer methods


/**
	@brief		デストラクタ
	@param[in]	なし
	@return		なし
	@par		メモリの解放等
*/
OptionLayer::~OptionLayer()
{
}
static Gauge* m_pGauge;
// on "init" you need to initialize your instance
bool OptionLayer::init()
{
	//////////////////////////////
	// 1. super init first
	if (!GridLayer::init())
	{
		return false;
	}

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Point origin = Director::getInstance()->getVisibleOrigin();


	m_pGauge = Gauge::create("OptionIllust.png");
	m_pGauge->setPosition(visibleSize / 2.f);
	m_pGauge->setDirection(Gauge::Direction::VERTICAL);
	m_pGauge->setTurn(true);
	m_pGauge->setAnchorPoint(Vec2(0.8f,0.2f));
	this->addChild(m_pGauge, 100000);
	_SetDefaultValues();


	/////////////////////////////
	// 3. add your codes below...

	
	// add a label shows "Hello World"
	// create and initialize a label

	//auto label = Label::createWithSystemFont("Hello Option World", "GenShinGothic-Regular.ttf", 24.f*MultiResolution::getFontScale());
	//
	//// position the label on the center of the screen
	//label->setPosition(Vec2(origin.x + visibleSize.width / 2,
	//	origin.y + visibleSize.height - label->getContentSize().height));
	//
	//// add the label as a child to this layer
	//this->addChild(label, 1);
	
	// 以下追加

	////------------------------------------
	//// BGM

	//// BGMのロード(ファイルはResourcesフォルダに)
	//CocosDenshion::SimpleAudioEngine::getInstance()->preloadBackgroundMusic("popbgm_title2_normalize.wav");
	//// BGMの音量の設定(最大値1.0、0.0は消音)
	//CocosDenshion::SimpleAudioEngine::getInstance()->setBackgroundMusicVolume(1.0f);
	//// BGMの再生(第二引数：ループさせるか)
	//CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic("popbgm_title2_normalize.wav", true);

	{
		// 背景
		auto backGra = Sprite::create("BlackBoard.png");
		if (backGra)
		{
			backGra->setPosition(visibleSize / 2.f);
			this->addChild(backGra, 0, "Back");
		}

		// ブラシプレビュー
		m_pDrawNode = DrawNode::create();
		this->addChild(m_pDrawNode, 100);

		auto brushBox = Sprite::create("BrushCheck2.png");
		brushBox->setPosition(m_previewPos);
		this->addChild(brushBox);

		// ブラシサイズサークル描画
		m_pDrawNode->drawDot(m_previewPos, m_brushSize, Color4F::RED);
	}


	
	//------------------------------------
	// ぷすらいと

	// BGM調整用Sprite ＋　リターンボタン　てへぺろ
	std::vector<cocos2d::Sprite*> spritesSetting =
	{
		cocos2d::Sprite::create("LineBack.png"),	// BGM線
		cocos2d::Sprite::create("bar.png"),			// BGMバー
		cocos2d::Sprite::create("LineBack.png"),	// SE線
		cocos2d::Sprite::create("bar.png"),			// SEバー
		cocos2d::Sprite::create("LineBack.png"),	// Brush線
		cocos2d::Sprite::create("bar.png"),			// Brushバー
	//	cocos2d::Sprite::create("HelloWorld.png"),	// リターンボタン（backFontに変更する事
	};

	std::vector<std::string> spriteNames = {
		"BgmLine",
		"BgmBar",
		"SeLine",
		"SeBar",
		"BrushLine",
		"BrushBar",
	//	"Return",
	};

	std::vector<cocos2d::Vec2> spritePositions = {
		// BGM
		cocos2d::Vec2(	origin.x + visibleSize.width / 2.f,
						origin.y + visibleSize.height / 10.f * 6.f),
		cocos2d::Vec2(	origin.x + visibleSize.width / 2.f,
						origin.y + visibleSize.height / 10.f * 6.f), // あとで音量の大きさから位置を決める
		// SE
		cocos2d::Vec2(	origin.x + visibleSize.width / 2.f,
						origin.y + visibleSize.height / 10.f * 4.f),
		cocos2d::Vec2(	origin.x + visibleSize.width / 2.f,
						origin.y + visibleSize.height / 10.f * 4.f), // あとで音量の大きさから位置を決める
		// Brush
		cocos2d::Vec2(	origin.x + visibleSize.width / 2.f,
						origin.y + visibleSize.height / 10.f * 2.f),
		cocos2d::Vec2(	origin.x + visibleSize.width / 2.f,
						origin.y + visibleSize.height / 10.f * 2.f), // ブラシサイズ変更用
		//// 戻るボタン
		//cocos2d::Vec2(	origin.x + visibleSize.width / 10.f * 2.f,
		//				origin.y + visibleSize.height / 10.f * 9.f),
	};
	{
		int i = 0;
		for (auto& sprite : spritesSetting)
		{
			// 座標
			sprite->setPosition(spritePositions[i]);
			// Tag
			sprite->setTag(i);
			// 子授かり
			int z = spriteNames[i].find("Bar") != std::string::npos ? 10 : 1;
			this->addChild(sprite, z, spriteNames[i]);

			if (spriteNames[i].find("Line") != std::string::npos)
			{
				// 線

				// 中身 左寄せ
				auto content = Sprite::create("Line.png");
				Vec2 pos = sprite->getPosition();
				pos.x += -sprite->getContentSize().width*sprite->getAnchorPoint().x;
				content->setPosition(pos);
				content->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
				this->addChild(content, 2, spriteNames[i] + "Content");
			}

			i++;
		}
	}

	// 戻るボタン
	{
		auto spriteCharaReturn = Sprite::create();
		const float w = 256.f, h = 150.f;
		Vector<SpriteFrame*> spritesReturn;
		for (int i = 0; i < 4; ++i)
		{
			auto rect = Rect(i*w, 0.f, w, h);
			spritesReturn.pushBack(SpriteFrame::create("CharaBack.png", rect));
		}
		auto animationReturn = Animation::createWithSpriteFrames(spritesReturn);
		animationReturn->setDelayPerUnit(0.5f);
		// アニメーション
		spriteCharaReturn->setPosition(origin.x + visibleSize.width / 2.f,
			origin.y + visibleSize.height / 10.f * 8.f);
		spriteCharaReturn->runAction(RepeatForever::create(Animate::create(animationReturn)));
		spriteCharaReturn->setName("Return");

		this->addChild(spriteCharaReturn);

	}




	// 音量の位置にスプライト移動
	this->_CalcValueStartPosition();

	{
		// 値表示ラベル
		std::vector<std::string> values = {
			std::to_string(static_cast<int>(m_bgmVolume*100.f)),
			std::to_string(static_cast<int>(m_seVolume*100.f)),
			std::to_string(static_cast<int>(m_brushSize* 2.f)),
		};// 直径Size

		const char* const lines[3] = { "BgmLine", "SeLine", "BrushLine" };
		const char* const valueNames[3] = { "BgmVolume", "SeVolume", "BrushSize" };
		int i = 0;
		for (auto value : values)
		{
			auto label = Label::createWithSystemFont(value, "GenShinGothic-Medium.ttf", calcFontSize(40.f));
			auto line = this->getChildByName(lines[i]);
			auto sz = label->getContentSize();
			auto pos = line->getPosition();
			auto size = line->getContentSize();
			auto anchor = line->getAnchorPoint();
			label->setPosition(
				origin.x + pos.x + size.width*anchor.x + sz.width / 2.f + 10.f,
				origin.y + pos.y);

			label->setColor(Color3B(250, 250, 250));

			this->addChild(label, 1, valueNames[i++]);
		}

		// 種別ラベル
		const char* const labelNames[3] = { "BGM", "SE", "BRUSH" };
		i = 0;
		for (auto& name : labelNames)
		{
			auto label = Label::createWithSystemFont(name, "GenShinGothic-Medium.ttf", calcFontSize(20.f));
			auto line = this->getChildByName(lines[i]);
			auto sz = label->getContentSize();
			auto pos = line->getPosition();
			auto size = line->getContentSize();
			auto anchor = line->getAnchorPoint();

			label->setPosition(
				origin.x + pos.x + size.width * -anchor.x - sz.width / 2.f - 10.f,
				origin.y + pos.y);

			label->setColor(Color3B(250, 250, 250));

			this->addChild(label, 1, i++);
		}
	}



	return true;
}


/**
	@brief		オプションレイヤー開始
	@param[in]	from	元レイヤー
	@return		なし
*/
void OptionLayer::restart(const std::string& from)
{
	m_fromLayerName = from;

	m_state = SceneAtelierMain::LayerState::INPUT;

	this->setVisible(true);

	this->_CalcValueStartPosition();

	_SetTouchListener();
}




/**
	@brief		メンバ変数初期化
	@param[in]	なし
	@return		なし
*/
void OptionLayer::_SetDefaultValues()
{
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	m_state = SceneAtelierMain::LayerState::INPUT;


	m_previewPos = Vec2(origin.x + visibleSize.width / 10.f * 8.f,
						origin.y + visibleSize.height / 10.f * 2.f);

	m_minBrushSize = 0.5f;
	m_maxBrushSize = 20.f - m_minBrushSize;

	
	// データの保存に使用
	UserDefault *_userDef = UserDefault::getInstance();

	m_bgmVolume = CocosDenshion::SimpleAudioEngine::getInstance()->getBackgroundMusicVolume();
	//float型の保存データ取り出し
	if (_userDef->getFloatForKey("BGM", m_bgmVolume) >= 0)
	{
		m_bgmVolume = _userDef->getFloatForKey("BGM");
	}
	m_seVolume = CocosDenshion::SimpleAudioEngine::getInstance()->getEffectsVolume();
	if (_userDef->getFloatForKey("SE", m_seVolume) >= 0)
	{
		m_seVolume = _userDef->getFloatForKey("SE");
	}
	m_brushSize = 5.f;
	if ((m_brushSize= _userDef->getFloatForKey("BrushSize", m_brushSize)) >= 0){
		
	}



}


/**
	@brief		タッチリスナーの設定
	@param[in]	なし
	@return		なし
*/
void OptionLayer::_SetTouchListener()
{

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Point origin = Director::getInstance()->getVisibleOrigin();

	// シングルタッチイベントリスナー作成
	auto listener = EventListenerTouchOneByOne::create();

	std::vector<std::string> names =
	{
		"BgmBar",
		"SeBar",
		"BrushBar",
		"BgmLine",
		"SeLine",
		"BrushLine",
		"Return",
		"Back"
	};
	
	listener->setSwallowTouches(true);

	// タッチした場合
	listener->onTouchBegan = [this](Touch* touch, Event* event)
	{
		auto target = static_cast<Sprite*>(event->getCurrentTarget());
		Point locationInNode = target->convertToNodeSpace(touch->getLocation());
		Size s = target->getContentSize();
		Rect rect = Rect(0, 0, s.width, s.height);

		
		// クリックエリアをチェック
		if (rect.containsPoint(locationInNode))
		{
			// 不透明度変更
			if (target->getName() != "Back")
			{
				//target->setOpacity(180);
			}
			else
			{
				m_touchPoints.push_back(touch->getLocation());
				m_pDrawNode->drawDot(touch->getLocation(), m_brushSize, Color4F::WHITE);
			}
			return true;
		}

		return false;
	};

	// タッチが移動した場合
	listener->onTouchMoved = [&](Touch* touch, Event* event)
	{
		auto currentTarget = static_cast<Sprite*>(event->getCurrentTarget());
		const auto name = currentTarget->getName();
		// バーとラインに対して
		if (name.find("Bgm") != std::string::npos ||
			name.find("Se") != std::string::npos ||
			name.find("Brush") != std::string::npos)
		{
			//タッチ中座標
			cocos2d::Vec2 pos = touch->getLocation();
			// 音量調節線幅
			auto line = this->getChildByName<Sprite*>("BgmLine");
			auto lineSize = line->getContentSize();
			// 線始点X
			auto lineLeft = line->getPositionX() - lineSize.width*line->getAnchorPoint().x;

			// 線の絶対座標領域内にタッチ座標を入れる

			pos.x = clampf(pos.x, lineLeft, lineLeft + lineSize.width);
			
			Node* bar;			// 移動バー
			float val = 0.f;	// 0.0~1.0
			Sprite* content;	// 中身
			if (name.find("Bgm") != std::string::npos)
			{
				// BGM
				bar = this->getChildByName("BgmBar");
				content = this->getChildByName<Sprite*>("BgmLineContent");
				// スプライト位置から音量計算
				m_bgmVolume = (pos.x - lineLeft) / lineSize.width;
				val = m_bgmVolume;
				// 音量ラベルに音量反映
				this->getChildByName<Label*>("BgmVolume")->setString(std::to_string(static_cast<int>(m_bgmVolume*100.f)));
				// BGMに音量値反映
				CocosDenshion::SimpleAudioEngine::getInstance()->setBackgroundMusicVolume(m_bgmVolume);

			}
			else if (name.find("Se") != std::string::npos)
			{
				// SE
				bar = this->getChildByName("SeBar");
				content = this->getChildByName<Sprite*>("SeLineContent");
				// スプライト位置から音量計算
				m_seVolume = (pos.x - lineLeft) / lineSize.width;
				val = m_seVolume;
				// 音量ラベルに音量反映
				this->getChildByName<Label*>("SeVolume")->setString(std::to_string(static_cast<int>(m_seVolume*100.f)));
				// SEに音量値反映
				CocosDenshion::SimpleAudioEngine::getInstance()->setEffectsVolume(m_seVolume);



			}
			else if (name.find("Brush") != std::string::npos)
			{
				// BRUSH
				bar = this->getChildByName("BrushBar");
				content = this->getChildByName<Sprite*>("BrushLineContent");

				// ブラシサイズラベルに反映
				this->getChildByName<Label*>("BrushSize")->setString(std::to_string(static_cast<int>(m_brushSize*2.f)));

				// Brushサイズを変更
				m_brushSize = m_minBrushSize + (((pos.x - lineLeft) / lineSize.width) * m_maxBrushSize);
				val = ((pos.x - lineLeft) / lineSize.width);
				// 描いた線の太さを消して
				m_pDrawNode->clear();
				// 線の太さを描画
				m_pDrawNode->drawDot(m_previewPos, m_brushSize, Color4F::RED);
			}
			// バーの座標反映
			bar->setPositionX(pos.x);
			// 中身
			Rect rect(line->getTextureRect());
			Rect r(rect);
			r.size.width = rect.size.width * val;
			content->setTextureRect(r);

			m_pGauge->setValue(val);
		}
		else if (name == "Return")
		{
		}
		else
		{
			// 以外
			// 描いた線の太さを消して
			m_pDrawNode->clear();
			// プレビュー描画
			m_pDrawNode->drawDot(m_previewPos, m_brushSize, Color4F::RED);

			// 線描画
			m_touchPoints.push_back(touch->getLocation());

			for (int i = 0; i < static_cast<int>(m_touchPoints.size()) - 1; ++i)
			{
				m_pDrawNode->drawSegment(m_touchPoints[i], m_touchPoints[i + 1], m_brushSize, Color4F::WHITE);
			}
		}
	};

	// タッチイベントが終了した場合
	listener->onTouchEnded = [this, names](Touch* touch, Event* event)
	{
		// 終了した場合の処理
		auto target = static_cast<Sprite*>(event->getCurrentTarget());
		const auto name = target->getName();
		// データの保存
		UserDefault *_userDef = UserDefault::getInstance();
		// 外部にBGM音量を保存
		if (name == "BgmBar")
		{
			_userDef->setFloatForKey("BGM", m_bgmVolume);
		}
		// 外部にSE音量を保存
		else if (name == "SeBar")
		{
			_userDef->setFloatForKey("SE", m_seVolume);
			// 確認音の再生
			CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("ButtonPushed.mp3");
		}
		// 外部にBrushサイズを保存
		else if (name == "BrushBar")
		{
			_userDef->setFloatForKey("BrushSize", m_brushSize);
		}
		else if (name == "Return")
		{
			// ここにシーン切り替え処理
			// 元レイヤーへ
			// 入力中以外受け付けない
			if (m_state != SceneAtelierMain::LayerState::INPUT) return;
			// 状態変更
			m_state = SceneAtelierMain::LayerState::BACK;

			// アニメーション開始
			if (m_fromLayerName == "SceneModeSelect")
			{
				dynamic_cast<SceneOption*>(this->getParent())->func();
			}
			else
			{
				for (const auto& name : names)
				{
					auto child = this->getChildByName(name);
					if (child)
					{
						_eventDispatcher->removeEventListenersForTarget(child);
					}
				}
				dynamic_cast<SceneAtelierMain*>(this->getParent()->getParent())->changeLayer(this->getName(), m_fromLayerName);
			}
		}
		else
		{
			// 描いた線の太さを消して
			m_pDrawNode->clear();
			// プレビュー描画
			m_pDrawNode->drawDot(m_previewPos, m_brushSize, Color4F::RED);

		}
		m_touchPoints.clear();

		// 不透明度を元に
		target->setOpacity(255);
	};

	for (const auto& name : names)
	{
		auto child = this->getChildByName(name);
		if (child)
		{
			auto listener_clone = listener->clone();
			_eventDispatcher->addEventListenerWithSceneGraphPriority(listener_clone, child);
		}
	}

}


/**
	@brief		各値のスプライト位置を計算
	@param[in]	なし
	@return		ない
*/
void OptionLayer::_CalcValueStartPosition()
{
	// 音量線の始点座標と幅からスプライト座標計算
	const auto* line = this->getChildByName<const Sprite*>("BgmLine");
	const auto& rect = line->getTextureRect();
	auto w = line->getContentSize().width;
	auto x = line->getPositionX() - w*line->getAnchorPoint().x;
	//---------------------------------
	// BGM
	{
		this->getChildByName("BgmBar")->setPositionX(x + w*m_bgmVolume);
		// 中身
		Rect r(rect);
		r.size.width = rect.size.width * m_bgmVolume;
		this->getChildByName<Sprite*>("BgmLineContent")->setTextureRect(r);
	}
	//---------------------------------
	// SE
	{
		this->getChildByName("SeBar")->setPositionX(x + w*m_seVolume);
		// 中身
		Rect r(rect);
		r.size.width = rect.size.width * m_seVolume;
		this->getChildByName<Sprite*>("SeLineContent")->setTextureRect(r);
	}
	//---------------------------------
	// ブラシサイズ
	{
		// 0.0~1.0
		float val = ((m_brushSize - m_minBrushSize) / m_maxBrushSize);
		this->getChildByName("BrushBar")->setPositionX(x + w*val);
		// 中身
		Rect r(rect);
		r.size.width = rect.size.width * val;
		this->getChildByName<Sprite*>("BrushLineContent")->setTextureRect(r);
	}
}
#pragma endregion	// OptionLayer methods




#pragma region	SceneOption methods
Scene* SceneOption::createScene()
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = SceneOption::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

bool SceneOption::init()
{
	//////////////////////////////
	// 1. super init first
	if (!Layer::init())
	{
		return false;
	}

	auto visibleSize = Director::getInstance()->getVisibleSize();
	Point origin = Director::getInstance()->getVisibleOrigin();


	auto m_pOptionLayer = OptionLayer::create();
	m_pOptionLayer->restart("SceneModeSelect");
	this->addChild(m_pOptionLayer, 0, "OptionLayer");


	return true;

}

void SceneOption::func()
{
	Director::getInstance()->replaceScene(TransitionFade::create(1.0f, SceneModeSelect::createScene(SceneModeSelect::Select::OPTION)));
}

#pragma endregion	// SceneOption methods
