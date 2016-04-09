/**=============================================================================
	@file		CountUpGameLayer.cpp
	@brief		タイムアタックモードレイヤークラス 実装
	@author		Masato Yamamoto
	@date		2015-02-02

	@par		[説明]
				タイムアタックモード用レイヤークラス
				テーマ当てるまでの時間を競う

	@note

		Copyright (C) 2015  saitsuyo
=============================================================================*/

#include "MultiResolution.h"
#include "CountUpGameLayer.h"
#include "SceneAtelierMain.h"
#include "LayerCanvas.hpp"
//#include "OptionLayer.h"


#include "Random.h"
#include "Common.h"


#include <fstream>
#include <sstream>

#pragma execution_character_set("utf-8")

USING_NS_CC_EXT;
USING_NS_CC;




const float CountUpGameLayer::TOUCH_APPLY_DELAY = 0.11666667f;	// 0.12秒(7フレーム)
const float CountUpGameLayer::ONE_DRAW_TIME = 3.f;	// 3秒


#pragma region CountUpGameLayer methods

/**
	@brief		デストラクタ
	@param[in]	なし
	@return		なし
	@par		メモリの解放等
*/
CountUpGameLayer::~CountUpGameLayer()
{
	m_colors.clear();
}

// on "init" you need to initialize your instance
bool CountUpGameLayer::init()
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


	//-----------------------------------------------------
	// 背景
	auto spriteBack = Sprite::create("MainBackAll.png");
	if(spriteBack)
	{
		// add "canvas" splash screen"

		spriteBack->setScale(
			this->getContentSize().width / spriteBack->getContentSize().width,
			this->getContentSize().height / spriteBack->getContentSize().height);
		// position the sprite on the center of the screen
		spriteBack->setPosition(
			origin.x + visibleSize.width / 2.f + origin.x,
			origin.y + visibleSize.height / 2.f + origin.y);

		spriteBack->setName("Back");

		// add the sprite as a child to this layer
		this->addChild(spriteBack, ZOrder::BACK);
	}

	//-----------------------------------------------------
	// キャンバス
	auto layerCanvas = LayerCanvas::create();
	if (layerCanvas && layerCanvas->createSprite("canvas.png"))
	{
		this->addChild(layerCanvas, ZOrder::CANVAS, "Canvas");

		layerCanvas->setPosition(
			origin + visibleSize / 2.f
			);
		layerCanvas->setVisible(false);



		//layerCanvas->setAnchorPoint(Vec2(0.f, 1.f));		// 左上

		//layerCanvas->setPosition(
		//	origin.x + 0.f,
		//	origin.y + visibleSize.height);


	}
	//-----------------------------------------------------
	// テーマ発表用
	auto spriteBoard = Sprite::create("BigBoard.png");
	if (spriteBoard)
	{
		spriteBoard->setPosition(
			origin + visibleSize/ 2.f
			);
		spriteBack->setVisible(true);		// 非表示
		this->addChild(spriteBoard, ZOrder::BIGBOARD, "BigBoard");

		auto size = spriteBoard->getContentSize();
		// 表示ラベル設定 余白 x:90*y:60
		// 上
		auto labelTop = Label::createWithSystemFont("周りに人がいないことを確認してから", "GenShinGothic-Regular.ttf", calcFontSize(40.f));
		if (labelTop)
		{
			// 中央揃え上揃え
			labelTop->setAlignment(TextHAlignment::CENTER, TextVAlignment::TOP);

			labelTop->setPosition(
				size.width / 2.f,
				size.height - labelTop->getContentSize().height/2.f - 60.f
				);

			spriteBoard->addChild(labelTop, 0, "TopLabel");
		}
		// 中央
		auto labelMiddle = Label::createWithSystemFont("", "GenShinGothic-Medium.ttf", calcFontSize(80.f));
		if (labelMiddle)
		{
			// 中央揃え中央揃え
			labelMiddle->setAlignment(TextHAlignment::CENTER, TextVAlignment::BOTTOM);

			labelMiddle->setPosition(
				size / 2.f
				);
			labelMiddle->setColor(Color3B(250, 210, 10));


			labelMiddle->setDimensions(size.width - 90, 0);
			//labelMiddle

			spriteBoard->addChild(labelMiddle, 0, "MiddleLabel");
		}
		// 下
		auto labelBottom = Label::createWithSystemFont("画面をタッチしてください", "GenShinGothic-Regular.ttf", calcFontSize(37.f));
		if (labelBottom)
		{
			// 中央揃え下揃え
			labelBottom->setAlignment(TextHAlignment::CENTER, TextVAlignment::BOTTOM);

			labelBottom->setAnchorPoint(Vec2(0.5f, 0.0f));

			labelBottom->setPosition(
				size.width / 2.f,
				labelBottom->getContentSize().height / 2.f + 60.f
				);

			spriteBoard->addChild(labelBottom, 0, "BottomLabel");
		}
	}

	//-----------------------------------------------------
	// ラベル用
	auto spriteSmallBoard = Sprite::create("board.png");
	if (spriteSmallBoard)
	{
		spriteSmallBoard->setPosition(
			origin.x + visibleSize.width / 2.f,
			origin.y + visibleSize.height - spriteSmallBoard->getContentSize().height / 2.f
			);
		this->addChild(spriteSmallBoard, ZOrder::TOPBOARD, "SmallBoard");
	}



	//-----------------------------------------------------
	// ラベル設定
	// アナウンス
	auto label = Label::createWithSystemFont(" ", "GenShinGothic-Medium.ttf", calcFontSize(30.f));
	{
		label->setColor(Color3B(250, 250, 250));	// 白文字
		label->enableOutline(Color4B::BLACK, 3);	// 黒枠線

		// position the label on the center of the screen
		//label->setPosition(
		//	origin.x + visibleSize.width / 2.f,
		//	origin.y + visibleSize.height - label->getContentSize().height);

		// add the label as a child to this layer
		//this->addChild(label, 10);
		label->setHorizontalAlignment(TextHAlignment::CENTER);
		spriteSmallBoard->addChild(label, 10, "Label");
		label->setPosition(spriteSmallBoard->getAnchorPointInPoints());
		m_pLabel = label;			// キャッシュ
	}
	//-----------------------------------------------------


	//-----------------------------------------------------
	// ボタン
	auto nextButton = MenuItemImage::create("button.png",
											"button2.png",
											[&](Ref* pSender)
	{
		//dynamic_cast<SceneAtelierMain*>(this->getParent()->getParent())->toTitle();

		if (FlowPhase::ANSWERED == m_flowPhase)
		{
			// タイトルへ
			dynamic_cast<SceneAtelierMain*>(this->getParent()->getParent())->toTitle();
		}
		else
		{
			// 全消去
			m_touchPoints.clear();
			m_pDrawNode->clear();
		}
	});
	if (nextButton)
	{
		m_pNextButton = nextButton;
		nextButton->setName("NextButton");
		// ボタン非表示
		//nextButton->setVisible(false);
		auto size = nextButton->getContentSize();
		nextButton->setPosition(
			origin.x + visibleSize.width - size.width / 2.f,
			origin.y + size.height / 2.f);
		auto label = Label::createWithSystemFont("全消去", "GenShinGothic-Medium.ttf", calcFontSize(20.f));
		if (label)
		{
			label->setPosition(nextButton->getContentSize() / 2.f);
			label->setHorizontalAlignment(TextHAlignment::CENTER);
			// ラベル非表示
			//label->setVisible(false);
			label->setColor(Color3B(5, 5, 5));
			nextButton->addChild(label, ZOrder::NEXTBUTTONLABEL, "ButtonLabel");
	
		}
	}
	// ヒントボタン
	auto hintButton = MenuItemImage::create("CheckHintButton.png",
											"CheckHintButton2.png",
											"CheckHintButton3.png",
											[&](Ref* pSender){
		// ヒント確認
		this->_ShowHint();
	});
	if (hintButton)
	{
		hintButton->setName("HintButton");
		//hintButton->setVisible(false);
		auto size = hintButton->getContentSize();
		hintButton->setPosition(
			origin + visibleSize - size / 2.f
			);

	}

	auto menuButtons = Menu::create(nextButton, hintButton, nullptr);
	if (menuButtons)
	{
		menuButtons->setPosition(Vec2::ZERO);
		menuButtons->setVisible(false);
		this->addChild(menuButtons, ZOrder::MENUBUTTONS, "MenuButtons");
	}
	//-----------------------------------------------------
	// 描く人表示
	// 描く人ノード
	m_pDrawNodeDrawer = DrawNode::create();
	{
		m_drawerDispRadius = 20.f;
		m_drawerDispMarginY = m_drawerDispRadius*2.f;
		m_drawerDispStaPos.x = m_drawerDispRadius + m_drawerDispMarginY;
		m_drawerDispStaPos.y = (spriteSmallBoard->getPositionY() - spriteSmallBoard->getContentSize().height*spriteSmallBoard->getAnchorPoint().y) - m_drawerDispMarginY;
		m_pDrawNodeDrawer->setPosition(Vec2::ZERO);
		this->addChild(m_pDrawNodeDrawer, ZOrder::DRAWER_NODE, "DrawNodeDrawer");
	}
	// nowパーティクル
	auto particleBatch = ParticleBatchNode::create("particle_stars.png");
	{
		particleBatch->setVisible(false);

		this->addChild(particleBatch, ZOrder::PARTICLE, "ParticleBatch_Stars");
	}
	// 描かない人×
	auto spriteCross = Sprite::create("batsu.png");
	{
		spriteCross->setVisible(false);
		this->addChild(spriteCross, ZOrder::CANT_DRAWER, "Cross");
	}
	//-----------------------------------------------------
	m_pDrawNode = DrawNode::create();
	{
		m_pDrawNode->setAnchorPoint(Vec2::ZERO);
		auto canvas = layerCanvas->getSprite();
		//layerCanvas->addChild(m_pDrawNode, 1);
		m_pDrawNode->setPosition(
			-(layerCanvas->getPositionX() - layerCanvas->getAnchorPoint().x*layerCanvas->getContentSize().width ),
			-(layerCanvas->getPositionY() - layerCanvas->getAnchorPoint().y*layerCanvas->getContentSize().height)
			);
		canvas->addChild(m_pDrawNode, 1, "DrawNode");
		//m_pDrawNode->setPosition(
		//	-(canvas->getPositionX() - canvas->getAnchorPoint().x*canvas->getContentSize().width),
		//	-(canvas->getPositionY() - canvas->getAnchorPoint().y*canvas->getContentSize().height)
		//	);

	}


	//-----------------------------------------------------
	// 描きすぎレイヤー
	auto layerTouch = Layer::create();
	{
		layerTouch->setVisible(false);
		
		auto sprite = Sprite::create();	// 背景用
		sprite->setTextureRect(Rect(0, 0, visibleSize.width, visibleSize.height));
		sprite->setPosition(visibleSize / 2.f);
		sprite->setColor(Color3B::BLACK);
		sprite->setOpacity(192);
		layerTouch->addChild(sprite, 0);

		this->addChild(layerTouch, ZOrder::MOST_FRONT, "OverTouchLayer");
		auto label = Label::createWithSystemFont("一人の持ち時間を使い果たしました\n"
												 "\n次の人に回して新たに描き始めてください\n"
												 "\n※　タッチした瞬間からカウントが再開します", "GenShinGothic-Medium.ttf", calcFontSize(40.f));
		{
			label->setPosition(layerTouch->getContentSize() / 2.f);
			label->setAlignment(TextHAlignment::CENTER, TextVAlignment::CENTER);
			label->setColor(Color3B(250, 250, 15));
			layerTouch->addChild(label, 10, "Label");
		}

		//auto listener = EventListenerTouchOneByOne::create();
		//listener->setSwallowTouches(false);
		//listener->onTouchBegan = [this](Touch* touch, Event* event){
		//	if (!m_isTouching)
		//	{
		//		return false;
		//	}
		//	return true;
		//};
		//listener->onTouchEnded = [layerTouch](Touch* touch, Event* event){
		//	layerTouch->setVisible(false);
		//};
		//this->_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, layerTouch);
	}


	//-----------------------------------------------------
	// 解答入力ボックス
	m_pEditBox = EditBox::create(spriteSmallBoard->getContentSize(), Scale9Sprite::create("board.png"));
	{
		// box
		auto size = m_pEditBox->getContentSize();
		m_pEditBox->setPreferredSize(spriteSmallBoard->getContentSize());
		m_pEditBox->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
		m_pEditBox->setPosition(Vec2(
			origin.x + visibleSize.width + size.width / 2.f,
			origin.y + size.height / 2.f));
		//m_pEditBox->setPosition(Vec2(
		//	origin.x + visibleSize.width / 2.f,
		//	origin.y + size.height / 2.f));
		// 内部ラベル
		m_pEditBox->setFontName("GenShinGothic-Regular.ttf");	// ファミリー
		m_pEditBox->setFontSize(static_cast<int>(calcFontSize(25.f)));		// サイズ
		m_pEditBox->setFontColor(Color3B(250, 250, 250));				// 色
		m_pEditBox->getLabel()->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
		m_pEditBox->getLabel()->setPosition(spriteSmallBoard->getContentSize() / 2.f);
		// 初期ラベル
		m_pEditBox->setPlaceHolder("ここをタッチして解答 (全てひらがなで、数字は半角で入力)");	// 例等初期値(入力時に消える)
		m_pEditBox->setPlaceholderFontColor(Color3B(5, 5, 5));
		m_pEditBox->getPlaceHolderLabel()->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
		m_pEditBox->getPlaceHolderLabel()->setPosition(spriteSmallBoard->getContentSize() / 2.f);

		// Box自体の設定
		m_pEditBox->setMaxLength(20);	// 文字数
		m_pEditBox->setReturnType(EditBox::KeyboardReturnType::DEFAULT);	// Returnキーが押された時
		m_pEditBox->setInputMode(EditBox::InputMode::SINGLE_LINE);			// 1行,自由文字列

		// メソッド
		m_pEditBox->setDelegate(this);

		this->addChild(m_pEditBox, ZOrder::BOX, "EditBox");
	}
	
	//-----------------------------------------------------
	// 間違え
	auto spriteCrossCanvas = Sprite::create("CrossCanvas.png");
	{
		spriteCrossCanvas->setPosition(visibleSize / 2.f);
		spriteCrossCanvas->setVisible(false);
		this->addChild(spriteCrossCanvas, ZOrder::MOST_FRONT, "CrossCanvas");
	}
	//-----------------------------------------------------
	// 正解
	auto spriteCircleCanvas = Sprite::create("CircleCanvas.png");
	{
		spriteCircleCanvas->setPosition(visibleSize / 2.f);
		spriteCircleCanvas->setVisible(false);
		this->addChild(spriteCircleCanvas, ZOrder::MOST_FRONT, "CircleCanvas");

	}
	


	//-----------------------------------------------------
	// Curtain
	//auto layerCurtain = Layer::create();
	//if (layerCurtain)
	//{
	//	auto pos = origin + visibleSize / 2.f;
	//	layerCurtain->setPosition(Vec2::ZERO);
	//	//layerCurtain->setScale(0.5f);
	//	this->addChild(layerCurtain, ZOrder::CURTAIN, "CurtainLayer");
	//	// 左
	//	auto spriteCurtainL = Sprite::create("CurtainL.png");
	//	if (spriteCurtainL)
	//	{
	//		spriteCurtainL->setPosition(pos);
	//		layerCurtain->addChild(spriteCurtainL, 0, "Left");
	//	}
	//	// 右
	//	auto spriteCurtainR = Sprite::create("CurtainR.png");
	//	if (spriteCurtainR)
	//	{
	//		spriteCurtainR->setPosition(pos);
	//		layerCurtain->addChild(spriteCurtainR, 0, "Right");
	//	}
	//	// 上
	//	auto spriteCurtainT = Sprite::create("CurtainT.png");
	//	if (spriteCurtainT)
	//	{
	//		spriteCurtainT->setPosition(pos);
	//		layerCurtain->addChild(spriteCurtainT, 10, "Top");
	//	}
	//
	//	// カーテン非表示
	//	layerCurtain->setVisible(false);
	//}


	
	
	//-----------------------------------------------------
	// タッチリスナー設定 非表示時の誤作動を防ぐためstart()で呼ぶ
	//_SetTouchListener();



	// オプション
	//auto spriteOption = Sprite::create("settingButton.png");
	//if (spriteOption)
	//{
	//	auto buttonOption = ControlButton::create(Scale9Sprite::create("settingButton.png"));
	//	buttonOption->setPreferredSize(spriteOption->getContentSize());
	//	ADD_BUTTON_PUSHED(buttonOption, CountUpGameLayer::_ControlButtonPushed);
	//
	//	// ボタン位置情報
	//	buttonOption->setPosition(
	//		origin.x + visibleSize.width / 2.f + 250,
	//		origin.y + visibleSize.height * 1.f / 4.f);
	//
	//	this->addChild(buttonOption, 10, "Option");
	//	//m_settingNodes.emplace_back(spriteOption);
	//}


	

	return true;
}


/**
	@brief		お絵かきメイン
	@param[in]	delta		1フレームの間隔
	@return		なし
*/
void CountUpGameLayer::update(float delta)
{
	if (m_isPenalty)
	{
		m_drawTime += delta;		// 時間加算
		this->_UpdateTimeLabel();
	}

	switch (m_flowPhase)
	{
	case CountUpGameLayer::FlowPhase::NEXT:
		break;
	case CountUpGameLayer::FlowPhase::CHECK:
		break;
	case CountUpGameLayer::FlowPhase::PLAY:
		switch (m_phase)
		{
		case CountUpGameLayer::Phase::ANNOUNCE:
			// 誰々さんの番ですー
		{
			// アナウンス表示後
			if (true)
			{
				m_phase = Phase::DRAWING;
				m_nonTouchTime = 0.f;
			}
		}
			break;
		case CountUpGameLayer::Phase::DRAWING:
			// お絵かき中
		{
			m_nonTouchTime += delta;		// 時間加算
			// タッチ中
			if (m_isTouching)
			{
				if (ONE_DRAW_TIME > m_oneDrawTime)
				{
					//this->getChildByName("OverTouchLayer")->setVisible(false);
					m_drawTime += delta;		// 時間加算
					m_oneDrawTime += delta;
					this->_UpdateTimeLabel();
				}
				else
				{
					this->_StopDrawing();
				}
			}
			m_pDrawNode->clear();

			for (size_t layer = 0; layer < m_touchPoints.size(); ++layer)
			{
				if (m_touchPoints[layer].size() == 1)
				{
					m_pDrawNode->drawDot(m_touchPoints[layer][0], m_brushSize, m_colors[layer % m_playerNum]);
				}
				// スプライン補間

				//*
				for (int i = 0; i < static_cast<int>(m_touchPoints[layer].size()) - 1; ++i)
				{
					m_pDrawNode->drawSegment(m_touchPoints[layer][i], m_touchPoints[layer][i + 1], m_brushSize, m_colors[layer % m_playerNum]);
					//m_pDrawNode->drawDot(m_touchPoints[i], PENSIZE, Color4F(1.f, 1.f, 1.f, 1.f));
				}
				//*/
		   }
		}
			break;
		case CountUpGameLayer::Phase::CONFIRM:
			// ok or cancel
			break;
		default:
			break;
		}

		// わかった！
		//if (false)
		//{
		//	// 確認
		//	m_flowPhase = FlowPhase::CONFIRM;
		//	
		//	// ラベルの設定
		//	m_pLabel->setString("全員描き終わりました これでよろしいですか？");

		//	// ボタンとラベルの設定
		//	m_pNextButton->setVisible(true);
		//	m_pButtonLabel->setVisible(true);
		//	m_pButtonLabel->setString("投票へ");
		//	
		//	// 小テーマ確認ボタン非表示
		//	this->getChildByName("MenuButtons")->getChildByName("HintButton")->setVisible(false);
		//
		//	// パーティクル
		//	this->getChildByName("ParticleBatch_Stars")->setVisible(false);
		//	


		//	//m_pLabel->setString("画伯だと思う人を指さしてください。");
		//	//m_pLabel->setVisible(true);


		//}

		break;
	//case CountUpGameLayer::FlowPhase::VOTE:
	//	break;
	//case CountUpGameLayer::FlowPhase::JUDGE:
	//	break;
	default:
		break;
	}
}

/**
	@brief		ゲームメイン開始
	@param[in]	from	元レイヤー
	@return		なし
*/
void CountUpGameLayer::restart(const std::string& from)
{
	if (from == "OptionLayer")
	{
		// 音量調整程度しかしていないのでスルー
	}
	else
	{
		Size visibleSize = Director::getInstance()->getVisibleSize();
		Vec2 origin = Director::getInstance()->getVisibleOrigin();

		auto scene = dynamic_cast<SceneAtelierMain*>(this->getParent()->getParent());
		// プレイ人数
		m_playerNum = scene->getPlayerNum();

		// 筆太さ
		auto def = UserDefault::getInstance();
		m_brushSize = 5.f;
		m_brushSize = def->getFloatForKey("BrushSize", m_brushSize);

		//-----------------------------------------------------
		// テーマ設定
		if (scene->isUseGMAgent())
		{
			// 大テーマ選定
			int bTheme = (2 <= m_themes.size()) ? ns_Random::Random::getInstance()->getRandomInt(0, m_themes.size() - 1) : 0;
			auto it = m_themes.begin();
			for (int i = 0; i < bTheme; ++i)
			{
				// イテレータを進める
				++it;
			}
			m_hint = it->first;


			// 小テーマ選定
			auto size = m_themes[m_hint].size();
			auto theme = m_themes[m_hint][ns_Random::Random::getInstance()->getRandomInt(0, size - 1)];
			m_smallTheme = theme.theme;
			m_kanaTheme = theme.kana;

			//m_themes.clear();

		}
		else
		{
			// GM入力
			m_hint = scene->getBigTheme();
			m_smallTheme = scene->getSmallTheme();
			m_kanaTheme = scene->getKanaTheme();
		}

		//-----------------------------------------------------
		// ラベル設定
		// 大テーマ
		//std::stringstream ss;
		//ss << "大テーマ[ " << m_hint << " ] ";
		//auto hintLabel = this->getChildByName<Label*>("HintLabel");
		//hintLabel->setString(ss.str());
		//hintLabel->setPosition(
		//	origin.x + visibleSize.width / 2.f,
		//	origin.y + visibleSize.height - hintLabel->getContentSize().height / 1.5f);

		// 小テーマ
		//this->getChildByName<Label*>("Label")->setString("小テーマ発表です。周りに人がいないことを確認してから画面をタッチしてください。");

		m_pLabel->setString("お題発表");

		//-----------------------------------------------------
		// 弟子選出
		m_pupilPlayer = ns_Random::Random::getInstance()->getRandomInt(0, m_playerNum - 1);

	
		
		//-----------------------------------------------------
		// タッチリスナー設定
		m_touchNum = 0;		// タッチ数初期化
		_SetTouchListener();


		//-----------------------------------------------------
		// 毎フレームupdate呼び出し設定
		this->scheduleUpdate();
	}

	// アニメーションフラグ折る
	m_isAnimation = false;

	m_isDrew = false;
	//-----------------------------------------------------
	// 表示
	this->setVisible(true);

}


//---------------------------------------------------------------------------------------------------
// ボックス表示時に呼ばれる
void CountUpGameLayer::editBoxEditingDidBegin(EditBox* editBox)
{
	log("editBox %p DidBegin !", editBox);
	// 描きすぎレイヤー非表示
	this->getChildByName("OverTouchLayer")->setVisible(false);

}
// OKボタン押したら呼ばれる
void CountUpGameLayer::editBoxTextChanged(EditBox* editBox, const std::string& text)
{
	log("editBox %p TextChanged, text: %s ", editBox, text.c_str());

	bool isPenalty = false;
	// 判定
	if (text == m_kanaTheme)
	{
		this->_StartAnsweredAnimation();
	}
	else if (text.empty())
	{
		// 入力が空
	}
	else
	{
		// 不正解
		isPenalty = true;
		auto cross = this->getChildByName("CrossCanvas");
		cross->runAction(
			Sequence::create(
				Show::create(),
				Blink::create(0.3f, 2),
				Hide::create(),
				nullptr
			)
		);
		// 解答ボックス出現し直し
		auto visibleSize = Director::getInstance()->getVisibleSize();
		m_pEditBox->runAction(
			Sequence::create(
				CallFunc::create([this, isPenalty]{ m_isPenalty = isPenalty; m_pEditBox->setEnabled(false); }),
				MoveTo::create(0.5f,
					Vec2(
						visibleSize.width + m_pEditBox->getContentSize().width*m_pEditBox->getAnchorPoint().x,
						m_pEditBox->getPositionY()
					)	
				),
				//CallFunc::create([this]{ m_pEditBox->setText(""); }),
				DelayTime::create(0.5f),
				EaseBackOut::create(
					MoveTo::create(0.5f,
						Vec2(visibleSize.width / 2.f,
							m_pEditBox->getPositionY()
						)	// 画面中央まで
					)
				),
				CallFunc::create([this]{ m_isPenalty = false; m_pEditBox->setEnabled(true); }),
				nullptr
			)
		);
	}
}

// ボックス終了時に呼ばれる
void CountUpGameLayer::editBoxEditingDidEnd(EditBox* editBox)
{
	log("editBox %p DidEnd !", editBox);
	
	std::string text = editBox->getText();
	
}

// ボックス終了後に呼ばれる
void CountUpGameLayer::editBoxReturn(EditBox* editBox)
{
	log("editBox %p was returned !", editBox);

}
//---------------------------------------------------------------------------------------------------




/**
	@brief		メンバ変数初期化
	@param[in]	なし
	@return		なし
*/
void CountUpGameLayer::_SetDefaultValues()
{

	// アニメーションフラグ折る
	m_isAnimation = false;

	// 回してね表示へ
	m_flowPhase = FlowPhase::NEXT;

	// 経過時間初期化
	m_drawTime = 0.f;

	// タッチしていない
	m_isTouching = false;
	m_isPenalty = false;

	// 描きはじめ
	m_nowDrawer = 0;
	
	//-----------------------------------------------------
	// ペン色
	m_colors.push_back(Color4F(0xff / 255.f, 0x0f / 255.f, 0x0f / 255.f, 1.f)); // ff0f0f
	m_colors.push_back(Color4F(0x0f / 255.f, 0xa3 / 255.f, 0xff / 255.f, 1.f));	// 0fa3ff
	m_colors.push_back(Color4F(0x00 / 255.f, 0x74 / 255.f, 0x16 / 255.f, 1.f));	// 007416
	m_colors.push_back(Color4F(0xf5 / 255.f, 0xb8 / 255.f, 0x00 / 255.f, 1.f));	// f5b800
	m_colors.push_back(Color4F(0x20 / 255.f, 0x0f / 255.f, 0xff / 255.f, 1.f));	// 200fff
	m_colors.push_back(Color4F(0x9c / 255.f, 0x0f / 255.f, 0xff / 255.f, 1.f));	// 9c0fff
	m_colors.push_back(Color4F(0x0f / 255.f, 0xff / 255.f, 0x36 / 255.f, 1.f));	// 0fff36
	m_colors.push_back(Color4F(0xff / 255.f, 0x30 / 255.f, 0xe6 / 255.f, 1.f));	// ff30e6
	m_colors.push_back(Color4F(0xff / 255.f, 0xff / 255.f, 0x2d / 255.f, 1.f));	// ffff2d


	// 条件がfalse:assert(色数が人数より少ない場合)
	//CCASSERT(m_colors.size() > m_playerNum, "m_colorsが足りないですぅー。");


	//-----------------------------------------------------
	// テーマ読み込み
#pragma region テーマ読み込み
	//*
	// テーマファイル読み込み
	//std::string fullPath = FileUtils::getInstance()->fullPathForFilename("theme.csv");
	//std::ifstream csvFile(fullPath);
	//CCASSERT(FileUtils::getInstance()->isFileExist(fullPath), "テーマファイル[theme.csv]開けませんでしたぁー。");

	//std::string csvName("theme_numeric.csv");
	std::string csvName("ThemeCountUp.csv");
	std::string fullpath = FileUtils::getInstance()->fullPathForFilename(csvName);
	ssize_t fs = 0;

#if CC_PLATFORM_WIN32 == CC_TARGET_PLATFORM
	//auto data = FileUtils::getInstance()->getDataFromFile("theme.csv");
	//fs = data.getSize();
	//unsigned char *strData = FileUtils::getInstance()->getFileData(fullpath, "r", &fs);
	//str = (char*)strData;
	//delete[] strData;
	std::ifstream csvFile(fullpath);
#else
	std::string str = FileUtils::getInstance()->getStringFromFile(csvName);
	std::istringstream csvFile(str);
#endif	


	//
	//CCASSERT(csvFile.is_open(), "テーマファイル[theme.csv]開けませんでしたぁー。");

	//std::string fullpath = FileUtils::getInstance()->fullPathForFilename("theme.csv");
	//ssize_t fs = 0;
	//unsigned char *data = FileUtils::getInstance()->getFileData(fullpath, "r", &fs);
	//std::string str = (char*)data;
	//std::stringstream sstream(str);
	//picojson::value v;
	//picojson::parse(v, sstream);

	//if(!csvFile) return;

	std::string buf;

	while (std::getline(csvFile, buf))
	{
		// インプットチェック(空白の場合終了)
		if (buf.empty())
			break;

		// カンマ区切りで読み込む
		std::istringstream ss(buf);

		// 書式は "ヒント,テーマ,よみがな"
		std::string hint;
		std::getline(ss, hint, ',');
		Theme theme;
		std::getline(ss, theme.theme, ',');
		ss >> theme.kana;

		// UTF-8のEOF検知用
		if (theme.kana.empty())
			break;

		// 配列に追加
		m_themes[hint].push_back(theme);


	}

	//*/
#pragma endregion	// テーマ読み込み

	
}


/**
	@brief		タッチリスナーの設定
	@param[in]	なし
	@return		なし
*/
void CountUpGameLayer::_SetTouchListener()
{
	// シングルタッチイベントリスナー作成
	//auto listener1 = EventListenerTouchOneByOne::create();
	// マルチタッチリスナー
	auto multiListener1 = EventListenerTouchAllAtOnce::create();
	auto dispatcher = this->getEventDispatcher();

	//listener1->setSwallowTouches(true);
	multiListener1->setEnabled(true);
	
	multiListener1->onTouchesBegan = ([&, this](const std::vector<Touch*>& touches, Event* event)->void
	{
		m_touchNum++;
		log("Began m_touchNum:%d", m_touchNum);
		if (m_isAnimation) return;// false;
		// 最初のタッチのみ
		if (m_touchNum > 1) return;

		auto touch = touches.front();

		switch (m_flowPhase)
		{
		case CountUpGameLayer::FlowPhase::NEXT:
			// 次の人へ回してね☆
		{
			// 役割表示へ
			m_flowPhase = FlowPhase::CHECK;
			// 表示文字の変更
			auto board = this->getChildByName("BigBoard");
			auto top = board->getChildByName<Label*>("TopLabel");
			auto middle = board->getChildByName<Label*>("MiddleLabel");
			auto bottom = board->getChildByName<Label*>("BottomLabel");

			//std::stringstream ss;
			if (this->_CheckPupil(m_nowDrawer))
			{
				// 画伯の人
				top->setString("あなたが");
				middle->setString("見習いさん");
			}
			else
			{
				// 芸術家
				top->setString("テーマ");
				middle->setString(m_smallTheme);
			}
			if (m_nowDrawer == 0)
			{
				// 最初の人がタッチした時
				// ラベルに大テーマを表示
				//std::stringstream ssHint;
				//ssHint << "ヒント[ " << m_hint << " ] ";
				//m_pLabel->setString(ssHint.str());
			}
			if (m_nowDrawer + 1 == m_playerNum)
			{
				// 最後の人
				//ss << "確認したらタッチしてテーマを見た順に、描き始めてください";
				bottom->setString("確認したらタッチしてテーマを見た順に\n描き始めてください");
			}
			else
			{
				// まだ他に見る人がいる
				// ss << "確認したらタッチして次の人に渡してください";
				bottom->setString("確認したらタッチして次の人に渡してください");
			}

			//m_pLabel->setString(ss.str());
		}
			//return false;
			break;
		case CountUpGameLayer::FlowPhase::CHECK:
			// 自分の役割を表示
		{

			// 人数分見たら
			if (++m_nowDrawer == m_playerNum)
			{
				// ゲームスタート
				_StartDrawing();
			}
			else
			{
				// まだ見る人がいるので
				m_flowPhase = FlowPhase::NEXT;		// 次の人へ回して画面へ
				// 表示文字の変更
				//m_pLabel->setString("周りに人がいないことを確認してから画面をタッチしてください。");
				auto board = this->getChildByName("BigBoard");
				auto top = board->getChildByName<Label*>("TopLabel");
				auto middle = board->getChildByName<Label*>("MiddleLabel");
				auto bottom = board->getChildByName<Label*>("BottomLabel");
				top->setString("周りに人がいないことを確認してから");
				middle->setString("");
				bottom->setString("画面をタッチしてください");
			}
		}
			//return false;
			break;
		case CountUpGameLayer::FlowPhase::PLAY:
			// お絵かき中
		{
			this->getChildByName("OverTouchLayer")->setVisible(false);
			// メインフェーズ
			switch (m_phase)
			{
			case CountUpGameLayer::Phase::ANNOUNCE:
				//return false;
				break;
			case CountUpGameLayer::Phase::DRAWING:
			{
				auto canvas = getChildByName<LayerCanvas*>("Canvas");
				auto target = event->getCurrentTarget();
				Point locationInNode = target->convertToNodeSpace(touch->getLocation());

				//*

				//Size s = target->getContentSize();
				//Rect rect = Rect(0, 0, s.width, s.height);

				Size s = canvas->getContentSize();
				s.width *= canvas->getScaleX();
				s.height *= canvas->getScaleY();
	
				Rect rect(canvas->getPositionX() - canvas->getAnchorPoint().x*s.width,
					canvas->getPositionY() - canvas->getAnchorPoint().y*s.height,
					s.width, s.height);
				// キャンバス内なら	
				if (rect.containsPoint(locationInNode))
					//*/
				{

					// 誤タッチ防ぎ用(指が離されてからの時間が一定以上)
					if (!m_touchPoints.empty() && TOUCH_APPLY_DELAY >= m_nonTouchTime)
					{
						// 誤タッチと認識。描いてる人を1人前に
						if (this->_CheckPupil(--m_nowDrawer))
						{
							// 弟子スキップ
							--m_nowDrawer;
						}						
						this->_UpdateDrawerDisp();
					}
					else
					{
						// 誤タッチでない
						// 時間初期化
						m_oneDrawTime = 0.f;

						// 点打ち始め
						size_t i = m_touchPoints.size();
						for (; i <= m_nowDrawer; ++i)
						{
							m_touchPoints.push_back(std::vector<Vec2>());
						}
						m_touchPoints[m_nowDrawer].push_back(locationInNode);
					}
					m_isDrew = true;
					m_isTouching = true;
				}
				else
				{
				//	return false;
					m_isDrew = false;
					break;
				}
			}
				break;
			case CountUpGameLayer::Phase::CONFIRM:
				//return false;
				break;
			default:
				break;
			}


		}
			break;
		//case CountUpGameLayer::FlowPhase::VOTE:
		//	// 画伯投票
		//{

		//}
		//	break;
		//case CountUpGameLayer::FlowPhase::JUDGE:
		//	// 画伯発表と正誤発表
		//{

		//}
		//	break;
		case CountUpGameLayer::FlowPhase::REPEAT:
			// 再プレイ？
		{
		}
			break;
		default:
			break;
		}
		//return true;
	});
	// タッチが移動した場合
	multiListener1->onTouchesMoved = [&](const std::vector<Touch*>& touches, Event* event)
	{
		log("Move m_touchNum:%d", m_touchNum);
		// 最初のタッチのみ
		if (m_touchNum > 1) return;

		// おっきなふぇーず
		if (m_flowPhase != FlowPhase::PLAY)
			return;
		// メインフェーズ
		if (m_phase != Phase::DRAWING)
			return;

		// 描きすぎ
		if (ONE_DRAW_TIME <= m_oneDrawTime)
			return;

		auto touch = touches.front();
		auto target = event->getCurrentTarget();
		Point locationInNode = target->convertToNodeSpace(touch->getLocation());




		auto canvas = getChildByName("Canvas");
		Size s = canvas->getContentSize();
		s.width *= canvas->getScaleX();
		s.height *= canvas->getScaleY();
		Rect rect(canvas->getPositionX() - canvas->getAnchorPoint().x*s.width,
			canvas->getPositionY() - canvas->getAnchorPoint().y*s.height,
			s.width, s.height);
		if (rect.containsPoint(locationInNode))
		{
			if (m_nowDrawer < m_touchPoints.size())
			{
				m_touchPoints[m_nowDrawer].push_back(locationInNode);
			}
		}

	};

	multiListener1->onTouchesEnded = [&](const std::vector<Touch*>& touches, Event* event)
	{
		m_touchNum--;
		log("Ended m_touchNum:%d", m_touchNum);
		// おっきなふぇーず
		if (m_flowPhase != FlowPhase::PLAY)
			return;
		// メインフェーズ
		if (m_phase != Phase::DRAWING)
			return;

		if (!m_isDrew) return;

		// 最初のタッチのみ
		if (m_touchNum > 0) return;

		// タッチしてない
		m_isTouching = false;

		// タッチしてない時間初期化
		m_nonTouchTime = 0.f;

		// 次描く人へ
		if (this->_CheckPupil(++m_nowDrawer))
		{
			// 弟子スキップ
			++m_nowDrawer;
		}
		
		this->_UpdateDrawerDisp();
	};

	
	//dispatcher->addEventListenerWithSceneGraphPriority(listener1, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(multiListener1, this);



	//_eventDispatcher->addEventListenerWithFixedPriority(listener1, 1);
}



/**
	@brief		お絵かき開始
	@param[in]	なし
	@return		なし
*/
void CountUpGameLayer::_StartDrawing()
{
	// フェーズ移行
	//m_flowPhase = FlowPhase::PLAY;
	// ラベル非表示
	//m_pLabel->setString("");
	//m_pLabel->setVisible(false);
	// 矢印非表示
	//this->removeChildByName("Arrow");

	m_isAnimation = true;

	auto canvas = this->getChildByName("Canvas");
	
	auto endFunc = CallFunc::create([&, this, canvas]()
	{
		// フェーズ移行
		m_flowPhase = FlowPhase::PLAY;
		m_isAnimation = false;

		// ボード非表示
		this->getChildByName("BigBoard")->setVisible(false);
		// ボタン表示
		this->getChildByName("MenuButtons")->setVisible(true);

		// 描く人情報更新
		auto spriteCross = this->getChildByName("Cross");
		spriteCross->setVisible(true);
		Vec2 pos(m_drawerDispStaPos);
		pos.y -= (m_pupilPlayer % m_playerNum)*(m_drawerDispRadius + m_drawerDispMarginY);
		spriteCross->setPosition(pos);
		this->_UpdateDrawerDisp();
		
		// 上ラベル
		std::stringstream ss;
		ss << "累計" << common::getFloatString(m_drawTime, 1, 8) << "秒";
		m_pLabel->setString(ss.str());

		// パーティクル
		auto particleBatch = this->getChildByName<ParticleBatchNode*>("ParticleBatch_Stars");
		particleBatch->setVisible(true);
		auto createFunc = CallFunc::create([&, this, particleBatch](){
			auto particle = common::createParticle("particle_sparkle_stars.plist");
			Vec2 pos(m_drawerDispStaPos);
			pos.y -= (m_nowDrawer % m_playerNum)*(m_drawerDispRadius + m_drawerDispMarginY);
			particle->setPosition(pos);
			particleBatch->addChild(particle);
		});
		auto particleAction = RepeatForever::create(
				Sequence::create(
					createFunc,
					DelayTime::create(0.5f),
					createFunc,
					DelayTime::create(3.f),
					nullptr
				)
			);
		particleAction->setTag(Tag::PARTICLE_ACTION);
		this->runAction(particleAction);
		

		canvas->setLocalZOrder(ZOrder::CANVAS);



		// 解答ボックス出現
		auto visibleSize = Director::getInstance()->getVisibleSize();
		m_pEditBox->runAction(
			Sequence::create(
				CallFunc::create([this]{ m_pEditBox->setEnabled(false); }),
				EaseBackOut::create(
					MoveTo::create(0.5f,
						Vec2(visibleSize.width / 2.f,
						m_pEditBox->getPositionY()
						)	// 画面中央まで
					)
				),
				CallFunc::create([this]{ m_pEditBox->setEnabled(true); }),
				nullptr
			)
		);

	});
	canvas->setLocalZOrder(this->getChildByName("SmallBoard")->getLocalZOrder() + 1);

	canvas->runAction(
		Sequence::create(
			Show::create(),
			ReverseTime::create(PageTurn3D::create(1.f, Size(24, 32))),
			endFunc,
			nullptr
		)
	);



	// 戻るボタン表示
	//this->getChildByName("MenuButtons")->getChildByName("BackButton")->setVisible(true);
	//-------------------------------------
	// 変数初期化
	m_phase = Phase::ANNOUNCE;		// アニメーションフェーズ
	m_nowDrawer = (m_pupilPlayer == 0) ? 1 : 0;				// 描く人
	m_drawTime = 0.f;				// タッチ時間初期化
	m_isTouching = false;			// 
	m_nonTouchTime = 0.f;			// タッチしていない時間初期化
}


// わかったボタン
void CountUpGameLayer::_ButtonCallback(cocos2d::Ref* pSender)
{
	
	// 解答ボックス出現
	//auto visibleSize = Director::getInstance()->getVisibleSize();
	//m_pEditBox->runAction(
	//	Sequence::create(
	//		CallFunc::create([this]{ m_pNextButton->setEnabled(false); }),
	//		EaseBackOut::create(
	//			MoveTo::create(0.5f,
	//				Vec2(visibleSize.width / 2.f,
	//				m_pEditBox->getPositionY()
	//				)	// 画面中央まで
	//			)
	//		),
	//		CallFunc::create([this]{ m_pNextButton->setEnabled(true); }),
	//		nullptr
	//	)
	//);

	//switch (m_flowPhase)
	//{
	////case CountUpGameLayer::FlowPhase::NEXT:
	////	break;
	////case CountUpGameLayer::FlowPhase::CHECK:
	////	break;
	////case CountUpGameLayer::FlowPhase::PLAY:
	////	break;
	//case CountUpGameLayer::FlowPhase::CONFIRM:
	//	this->_StartVote();
	//	break;
	//case CountUpGameLayer::FlowPhase::VOTE:
	//	// 開票
	//	this->_StartCounting();
	//	break;
	//case CountUpGameLayer::FlowPhase::COUNTING:
	//	// 開票終了
	//	this->_StartedCounting();
	//	break;
	//case CountUpGameLayer::FlowPhase::JUDGE:
	//	this->_StartShowCanvas();
	//	break;
	//case CountUpGameLayer::FlowPhase::SHOW_CANVAS:
	//	this->_StartOpenTheme();
	//	break;
	//case CountUpGameLayer::FlowPhase::OPEN_THEME:
	//	dynamic_cast<SceneAtelierMain*>(this->getParent()->getParent())->toTitle();
	//	//restart(this->getName());
	//	break;
	//case CountUpGameLayer::FlowPhase::REPEAT:
	//	break;
	//case CountUpGameLayer::FlowPhase::END:
	//	break;
	//default:
	//	break;
	//}
	


}



///**
//	@brief		ボタンタッチ
//	@param[in]	pSender		ボタン
//	@param[in]	eventType	イベントタイプ
//	@return		なし
//*/
//void CountUpGameLayer::_ControlButtonPushed(Ref* pSender, Control::EventType eventType)
//{
//	auto button = dynamic_cast<ControlButton*>(pSender);
//	if (!button || m_isAnimation)
//		return;
//
//	std::string name = button->getName();
//	if ("Option" == name)
//	{
//		// オプションレイヤーへ
//		m_isAnimation = true;
//		dynamic_cast<SceneAtelierMain*>(this->getParent()->getParent())->changeLayer(this->getName(), "OptionLayer");
//
//		//auto canvas = this->getChildByName<Sprite*>("Canvas");
//		//canvas->runAction(ScaleTo::create(2.f, 0.75f));
//	}
//}

/**
	@brief		描画一時終了
	@param[in]	なし
	@return		なし
*/
void CountUpGameLayer::_StopDrawing()
{
	this->getChildByName("OverTouchLayer")->setVisible(true);
}

/**
	@brief		描画者情報更新
	@param[in]	なし
	@return		なし
*/
void CountUpGameLayer::_UpdateDrawerDisp()
{
	// 描画者
	m_pDrawNodeDrawer->clear();
	const auto winSize = Director::getInstance()->getWinSize();
	for (unsigned int i = 0; i < m_playerNum; ++i)
	{
		Vec2 pos(m_drawerDispStaPos);
		pos.y -= i*(m_drawerDispRadius + m_drawerDispMarginY);
		auto color = m_colors[i];
		if (i != m_nowDrawer%m_playerNum)
		{
			color.a = 0.5f;
		}
		m_pDrawNodeDrawer->drawDot(pos, m_drawerDispRadius, color);
	}
}


/**
	@brief		累計時間ラベル更新
	@param[in]	なし
	@return		なし
*/
void CountUpGameLayer::_UpdateTimeLabel()
{
	std::stringstream ss;
	ss << "累計" << common::getFloatString(m_drawTime, 1, 8) << "秒";
	m_pLabel->setString(ss.str());
}

/**
	@brief		ヒントレイヤーの表示
	@param[in]	なし
	@return		なし
*/
void CountUpGameLayer::_ShowHint()
{
	// ペナルティ
	m_drawTime += 5.0f;
	this->_UpdateTimeLabel();

	auto winSize = Director::getInstance()->getWinSize();
	auto layer = Layer::create();
	auto sprite = Sprite::create();	// 背景用
	layer->addChild(sprite, 0);
	sprite->setTextureRect(Rect(0,0,winSize.width,winSize.height));
	sprite->setPosition(winSize / 2.f);
	sprite->setColor(Color3B::BLACK);
	sprite->setOpacity(128);
	auto board = this->getChildByName<Sprite*>("BigBoard");
	
	// ボード表示
	board->setVisible(true);
	
	// タッチスクリーン用レイヤー
	// z: ボード > レイヤー > キャンバス(その他)
	this->addChild(layer, ZOrder::MOST_FRONT);
	board->setLocalZOrder(layer->getLocalZOrder() + 1);

	auto listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);
	listener->onTouchBegan = [](Touch* touch, Event* event){
		return true;
	};
	listener->onTouchEnded = [layer, board](Touch* touch, Event* event){
		board->setVisible(false);
		layer->runAction(RemoveSelf::create());
	};
	this->_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, layer);
	
	auto top = board->getChildByName<Label*>("TopLabel");
	auto middle = board->getChildByName<Label*>("MiddleLabel");
	auto bottom = board->getChildByName<Label*>("BottomLabel");
	top->setString("ヒント");
	middle->setSystemFontSize(calcFontSize(30.f));
	middle->setColor(Color3B::BLACK);
	middle->setString(m_hint);

	bottom->setString("確認したら画面をタッチ");
	
}


/**
	@brief		解答正解アニメーション開始
	@param[in]	なし
	@return		なし
*/
void CountUpGameLayer::_StartAnsweredAnimation()
{
	// 正解
	m_flowPhase = FlowPhase::ANSWERED;
	
	// ○
	auto circle = this->getChildByName("CircleCanvas");
	circle->runAction(
		Sequence::create(
			Show::create(),
			Blink::create(0.3f, 2),
			//Hide::create(),
			nullptr
		)
	);

	// 編集不可
	m_pEditBox->setEnabled(false);
	// 中身変換
	m_pEditBox->setText(m_smallTheme.c_str());

	// ヒントボタン非表示
	this->getChildByName("MenuButtons")->getChildByName("HintButton")->setVisible(false);

	// ボタンラベルを「タイトルへ」
	m_pNextButton->getChildByName<Label*>("ButtonLabel")->setString("タイトルへ");

	// パーティクル発生
	// 加算合成なので見やすくなるように少し暗く
	auto visibleSize = Director::getInstance()->getVisibleSize();
	auto layer = Layer::create();
	auto sprite = Sprite::create();	// 背景用
	layer->addChild(sprite, 0);
	sprite->setTextureRect(Rect(0, 0, visibleSize.width, visibleSize.height));
	sprite->setPosition(visibleSize / 2.f);
	sprite->setColor(Color3B::BLACK);
	sprite->setOpacity(192);

	auto particleBatch = ParticleBatchNode::create("particle_texture.png");
	this->addChild(particleBatch, ZOrder::MOST_FRONT, "ParticleBatch_Texture");

	this->addChild(layer, ZOrder::PARTICLE, "ParticleLayer");

	// パーティクル定期発生
	auto paticleFunc = CallFunc::create([&, visibleSize, particleBatch](){

		auto particleFw = common::createParticle("particle_fireworks_texture.plist");

		auto size = particleFw->getContentSize();

		// 座標決め
		float posX = ns_Random::Random::getInstance()->getRandom<float>(size.width / 2.f, visibleSize.width - size.width / 2.f);
		float posY = ns_Random::Random::getInstance()->getRandom<float>(size.height / 2.f, visibleSize.height - size.height / 2.f);
		particleFw->setPosition(posX, posY);


		particleBatch->addChild(particleFw, 0, "Fireworks");

	});

	this->runAction(
		Sequence::create(
			Repeat::create(
				Sequence::create(
					DelayTime::create(0.75f),
					paticleFunc,			// パーティクル発生
					nullptr
				), 5
			),
			DelayTime::create(1.5f),	// 数秒後
			Spawn::create(
				TargetedAction::create(sprite, FadeOut::create(1.f)),
				TargetedAction::create(circle, FadeOut::create(1.f)),
				nullptr
			),
			TargetedAction::create(layer, RemoveSelf::create()),	// パーティクルレイヤー削除
			TargetedAction::create(circle, RemoveSelf::create()),	// ○削除
			TargetedAction::create(m_pNextButton, Show::create()),	// タイトルへボタン表示
			//------------------
			nullptr
		)
	);

}

#pragma endregion CountUpGameLayer methods

