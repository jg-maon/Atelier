/**=============================================================================
	@file		RelayGameLayer.cpp
	@brief		お絵かきリレーモードレイヤークラス 実装
	@author		Masato Yamamoto
	@date		2015-02-11

	@par		[説明]
				お絵かきリレーモード用レイヤークラス
				テーマ当てるまでの時間を競う

	@note

		Copyright (C) 2015  saitsuyo
=============================================================================*/

#include "MultiResolution.h"
#include "RelayGameLayer.h"
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

// 1:パス押した瞬間(描いてたテーマ発表)
// 2:次のテーマ表示
// 3:次のテーマ非表示
#define NEXT_CANVAS_TIMING 3


//const float RelayGameLayer::TOUCH_APPLY_DELAY = 0.11666667f;	//!< 誤タッチ認識間隔 0.12秒(7フレーム)
const float RelayGameLayer::BONUS_TIME = 5.f;		//!< ボーナス時間(max(ボーナス-使用時間,0))
const float RelayGameLayer::ONE_DRAW_TIME = 15.f;	//!< タッチ時間 15秒

#pragma region RelayGameLayer methods

/**
	@brief		デストラクタ
	@param[in]	なし
	@return		なし
	@par		メモリの解放等
*/
RelayGameLayer::~RelayGameLayer()
{
	m_colors.clear();
}

// on "init" you need to initialize your instance
bool RelayGameLayer::init()
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
		auto labelTop = Label::createWithSystemFont("描く順番と解答者の発表です", "GenShinGothic-Regular.ttf", calcFontSize(40.f));
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
		labelMiddle->setName("MiddleLabel");
		auto itemLabel = MenuItemLabel::create(labelMiddle, [this](Ref* pSender){
			// 検索窓開く
			std::stringstream url;
			url << "https://www.google.co.jp/search?tbm=isch&q=" << m_nowTheme.theme;
			Application::getInstance()->openURL(url.str().c_str());
		});
		if (itemLabel)
		{
			m_pMiddleLabel = itemLabel;
			// 始めのうちは反応なし
			m_pMiddleLabel->setEnabled(false);
			// 中央揃え中央揃え
			labelMiddle->setAlignment(TextHAlignment::CENTER, TextVAlignment::BOTTOM);

			labelMiddle->setColor(Color3B(250, 210, 10));
			m_pMiddleLabel->setColor(Color3B(250, 210, 10));


			labelMiddle->setDimensions(size.width - 90, 0);
			m_pMiddleLabel->setName("ItemMiddleLabel");
			auto menu = Menu::create(m_pMiddleLabel, nullptr);
			menu->setPosition(
				size / 2.f
				);
			spriteBoard->addChild(menu, 0, "MenuMiddleLabel");
		}
		// 下
		auto labelBottom = Label::createWithSystemFont("1人目の人から順に\n画面をタッチしてください", "GenShinGothic-Regular.ttf", calcFontSize(37.f));
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
											[this](Ref* pSender)
	{
		if (FlowPhase::ANSWERED == m_flowPhase)
		{
			// タイトルへ
			dynamic_cast<SceneAtelierMain*>(this->getParent()->getParent())->toTitle();
		}
		else
		{
			// 全消去
			m_touchPoints[m_nowPage].clear();
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
			m_pButtonLabel = label;
			label->setPosition(nextButton->getContentSize() / 2.f);
			label->setHorizontalAlignment(TextHAlignment::CENTER);
			// ラベル非表示
			//label->setVisible(false);
			label->setColor(Color3B(5, 5, 5));
			nextButton->addChild(label, ZOrder::NEXTBUTTONLABEL, "ButtonLabel");
	
		}
	}


	auto menuButtons = Menu::create(nextButton, nullptr);
	if (menuButtons)
	{
		menuButtons->setPosition(Vec2::ZERO);
		menuButtons->setVisible(false);
		this->addChild(menuButtons, ZOrder::MENUBUTTONS, "MenuButtons");
	}


	//-----------------------------------------------------
	// 前後ボタン
	// 前
	auto itemBack = MenuItemImage::create("BackButton.png",
										  "BackButton2.png",
										  [this](Ref* pSender){

		auto button = dynamic_cast<MenuItemImage*>(pSender);
		if (0 < m_nowPage)
		{
			// 戻るよ！
			auto oldCanvas = this->getChildByName("Canvas");
			if (!oldCanvas)
				oldCanvas = this->getChildByName("Canvas" + std::to_string(m_nowPage));



			--m_nowPage;
			std::stringstream ss;
			ss << "Canvas" << m_nowPage;
			auto layerCanvas = this->getChildByName(ss.str());
			layerCanvas->setLocalZOrder(oldCanvas->getLocalZOrder() + 1);
			auto drawNode = layerCanvas->getChildByName<DrawNode*>("DrawNode");
			layerCanvas->setVisible(true);
			layerCanvas->runAction(
				Sequence::create(
					ReverseTime::create(PageTurn3D::create(0.5f, Size(24.f, 32.f))),
					CallFunc::create([this, layerCanvas, drawNode](){
						// キャンバスの入れ替え
						
						//--------------------------
						// DrawNode入れ替え
						m_pDrawNode = drawNode;
						//--------------------------
					}),
					nullptr
				)
			);

			if (0 == m_nowPage)
			{
				// もう戻れないよ！
				button->setVisible(false);
			}
			button->getParent()->getChildByName("LastNextButton")->setVisible(true);
		}
	
	});
	{
		itemBack->setName("LastBackButton");
	}
	auto itemNext = MenuItemImage::create("NextButton.png",
										  "NextButton2.png",
										  [this](Ref* pSender){
		auto button = dynamic_cast<MenuItemImage*>(pSender);
		const auto maxPage = m_touchPoints.size() - 1;
		if (maxPage > m_nowPage)
		{
			// 進むよ！
			auto oldCanvas = this->getChildByName("Canvas");
			if (!oldCanvas)
				oldCanvas = this->getChildByName("Canvas" + std::to_string(m_nowPage));


			++m_nowPage;
			std::stringstream ss;
			ss << "Canvas" << m_nowPage;
			auto layerCanvas = this->getChildByName(ss.str());
			layerCanvas->setLocalZOrder(oldCanvas->getLocalZOrder() - 1);
			auto drawNode = layerCanvas->getChildByName<DrawNode*>("DrawNode");
			layerCanvas->setVisible(true);
			
			oldCanvas->runAction(
				Sequence::create(
					PageTurn3D::create(0.5f, Size(24.f, 32.f)),
					CallFunc::create([this, layerCanvas, drawNode](){
						// キャンバスの入れ替え
						
						//--------------------------
						// DrawNode入れ替え
						m_pDrawNode = drawNode;
						//--------------------------
					}),
					nullptr
				)
			);

			if (maxPage == m_nowPage)
			{
				// もう進めないよ！
				button->setVisible(false);
			}
			button->getParent()->getChildByName("LastBackButton")->setVisible(true);
		}

	});
	{
		itemNext->setName("LastNextButton");
		itemNext->setVisible(false);
	}
	auto menuLastButtons = Menu::create(itemBack, itemNext, nullptr);
	{
		// 縦一列,間隔
		const float padding = 10.f;
		menuLastButtons->alignItemsVerticallyWithPadding(padding);

		// 非表示
		menuLastButtons->setVisible(false);

		auto canvas = layerCanvas->getSprite();
		Vec2 leftBottom = canvas->convertToWorldSpace(Vec2::ZERO);

		auto size = itemBack->getContentSize();
		menuLastButtons->setPosition(leftBottom.x / 2.f,
									 leftBottom.y + size.height + padding / 2.f);

		this->addChild(menuLastButtons, ZOrder::MENULASTBUTTONS, "MenuLastButtons");
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
												 "\n次の人に回してタッチしてください\n",
												 "GenShinGothic-Medium.ttf", calcFontSize(40.f));
		{
			label->setPosition(layerTouch->getContentSize() / 2.f);
			label->setAlignment(TextHAlignment::CENTER, TextVAlignment::CENTER);
			label->setColor(Color3B(250, 250, 15));
			layerTouch->addChild(label, 10, "Label");
		}

		auto listener = EventListenerTouchOneByOne::create();
		listener->setSwallowTouches(false);
		listener->onTouchBegan = [this, layerTouch](Touch* touch, Event* event){
			if (layerTouch->isVisible())
			{
				return true;
			}
			return false;
		};
		listener->onTouchEnded = [this, layerTouch](Touch* touch, Event* event){
			//layerTouch->setVisible(false);
			//m_pListener->setSwallowTouches(false);
			// 描きすぎレイヤー非表示
			this->_SetOverTouchLayerVisible(false);
			if (m_nowPage >= m_playerNum - 2)
			{
				// 全員描いた場合
				// キャンバスめくらない
				m_flowPhase = FlowPhase::JUDGE;

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

				// 全消去無効化
				m_pNextButton->setVisible(false);


				// ラベルの設定
				m_pLabel->setString("テーマを答えてください");

			}
			else
			{
				// 前の人の絵を見せる
				m_flowPhase = FlowPhase::SHOW;
				m_pLabel->setString("見終わったらタッチして描き始めてください");
			}

			// 全員描いた場合
			//if (m_nowPage >= m_playerNum - 1)
			//{
			//	// フェーズ
			//	m_flowPhase = FlowPhase::JUDGE;
			//
			//	// 解答ボックス出現
			//	auto visibleSize = Director::getInstance()->getVisibleSize();
			//	m_pEditBox->runAction(
			//		Sequence::create(
			//			CallFunc::create([this]{ m_pEditBox->setEnabled(false); }),
			//			EaseBackOut::create(
			//				MoveTo::create(0.5f,
			//					Vec2(visibleSize.width / 2.f,
			//					m_pEditBox->getPositionY()
			//					)	// 画面中央まで
			//				)
			//			),
			//			CallFunc::create([this]{ m_pEditBox->setEnabled(true); }),
			//			nullptr
			//		)
			//	);
			//
			//	// ラベルの設定
			//	m_pLabel->setString("テーマを答えてください");
			//
			//
			//}

		};
		this->_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, layerTouch);
		m_pListener = listener;
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


	return true;
}


/**
	@brief		お絵かきメイン
	@param[in]	delta		1フレームの間隔
	@return		なし
*/
void RelayGameLayer::update(float delta)
{
	switch (m_flowPhase)
	{
	case RelayGameLayer::FlowPhase::CHECK:
		break;
	case RelayGameLayer::FlowPhase::PLAY:
		switch (m_phase)
		{
		case RelayGameLayer::Phase::ANNOUNCE:
			// 誰々さんの番ですー
		{
			// アナウンス表示後
			//if (true)
			{
				m_phase = Phase::DRAWING;
				m_nonTouchTime = 0.f;
			}
		}
			break;
		case RelayGameLayer::Phase::DRAWING:
			// お絵かき中
		{
			if (m_isCounting)
			{
				m_oneDrawTime += delta;
				this->_UpdateTimeLabel();
				if (ONE_DRAW_TIME < m_oneDrawTime)
				{
					// 描きすぎレイヤー表示
					this->_SetOverTouchLayerVisible(true);

					// 時間設定
					m_oneDrawTime = ONE_DRAW_TIME;
					this->_UpdateTimeLabel();
					m_isCounting = false;
				}
			}

			m_nonTouchTime += delta;		// 時間加算
			// タッチ中
			//if (m_isTouching)
			//{
			//	m_drawableTime -= delta;		// 時間減算
			//	m_oneDrawTime += delta;
			//	this->_UpdateTimeLabel();				
			//}
			m_pDrawNode->clear();

			Color4F color(m_colors[m_nowPage % m_playerNum]);
			for (size_t layer = 0; layer < m_touchPoints[m_nowPage].size(); ++layer)
			{
				if (m_touchPoints[m_nowPage][layer].size() == 1)
				{
					m_pDrawNode->drawDot(m_touchPoints[m_nowPage][layer][0], m_brushSize, color);
				}
				// スプライン補間

				//*
				for (int i = 0; i < static_cast<int>(m_touchPoints[m_nowPage][layer].size()) - 1; ++i)
				{
					m_pDrawNode->drawSegment(m_touchPoints[m_nowPage][layer][i], m_touchPoints[m_nowPage][layer][i + 1], m_brushSize, color);
				}
				//*/
		   }
		}
			break;
		case RelayGameLayer::Phase::CONFIRM:
			// ok or cancel
			break;
		default:
			break;
		}

	
		break;
	//case RelayGameLayer::FlowPhase::VOTE:
	//	break;
	//case RelayGameLayer::FlowPhase::JUDGE:
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
void RelayGameLayer::restart(const std::string& from)
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
		// テーマ選定
		int theme = (2 <= m_themes.size()) ? ns_Random::Random::getInstance()->getRandomInt(0, m_themes.size() - 1) : 0;
		m_nowTheme = m_themes[theme];

		//-----------------------------------------------------
		// 描く人順決定
		m_drawOrder.assign(m_playerNum, 0);	// 0で初期化
		unsigned int n = 0;
		while (n < m_playerNum)
		{
			int player = ns_Random::Random::getInstance()->getRandomInt(0, m_playerNum - 1);
			// 未設定のプレイヤーに順序の設定
			if (m_drawOrder[player] == 0)
			{
				m_drawOrder[player] = ++n;	// n番目
			}
		}


		//-----------------------------------------------------
		// 上ラベル
		m_pLabel->setString("お題発表");
		
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
void RelayGameLayer::editBoxEditingDidBegin(EditBox* editBox)
{
	log("editBox %p DidBegin !", editBox);

}
// OKボタン押したら呼ばれる
void RelayGameLayer::editBoxTextChanged(EditBox* editBox, const std::string& text)
{
	log("editBox %p TextChanged, text: %s ", editBox, text.c_str());

	if (text.empty())
	{
		// 入力が空
	}
	else
	{
		// アニメーション
		this->_StartAnsweredAnimation(text == m_nowTheme.kana);
	}
}

// ボックス終了時に呼ばれる
void RelayGameLayer::editBoxEditingDidEnd(EditBox* editBox)
{
	log("editBox %p DidEnd !", editBox);
}

// ボックス終了後に呼ばれる
void RelayGameLayer::editBoxReturn(EditBox* editBox)
{
	log("editBox %p was returned !", editBox);

}
//---------------------------------------------------------------------------------------------------




/**
	@brief		メンバ変数初期化
	@param[in]	なし
	@return		なし
*/
void RelayGameLayer::_SetDefaultValues()
{

	// アニメーションフラグ折る
	m_isAnimation = false;

	// 順番表示へ
	m_flowPhase = FlowPhase::CHECK;

	// 時間計測停止
	m_isCounting = false;

	m_oneDrawTime = 0.f;

	// 描きはじめ
	m_nowDrawer = 0;
	m_nowPage = 0;
	m_nowLine = 0;

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

	
	//-----------------------------------------------------
	// テーマ読み込み
#pragma region テーマ読み込み
	//std::string csvName("theme_numeric.csv");
	std::string csvName("ThemeRelay.csv");
	std::string fullpath = FileUtils::getInstance()->fullPathForFilename(csvName);
	ssize_t fs = 0;

#if CC_PLATFORM_WIN32 == CC_TARGET_PLATFORM
	std::ifstream csvFile(fullpath);
	CCASSERT(csvFile.is_open(), "csvFile open error.");
#else
	std::string str = FileUtils::getInstance()->getStringFromFile(csvName);
	std::istringstream csvFile(str);
#endif	

	std::string buf;

	while (std::getline(csvFile, buf))
	{
		// インプットチェック(空白の場合終了)
		if (buf.empty())
			break;

		// カンマ区切りで読み込む
		std::istringstream ss(buf);

		// 書式は "テーマ,よみがな"
		Theme theme;
		std::getline(ss, theme.theme, ',');
		ss >> theme.kana;

		// UTF-8のEOF検知用
		if (theme.kana.empty())
			break;

		// 配列に追加
		m_themes.push_back(theme);
	}
#pragma endregion	// テーマ読み込み


	
}


/**
	@brief		タッチリスナーの設定
	@param[in]	なし
	@return		なし
*/
void RelayGameLayer::_SetTouchListener()
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
		case RelayGameLayer::FlowPhase::CHECK:
			// 自分の順番を表示
		{
			// テーマ表示
			auto board = this->getChildByName<Sprite*>("BigBoard");
			auto top = board->getChildByName<Label*>("TopLabel");
			auto bottom = board->getChildByName<Label*>("BottomLabel");

			// ボード表示
			board->setVisible(true);

			std::stringstream ss;
			if (m_drawOrder[m_nowDrawer] == m_playerNum)
			{
				// 解答者
				top->setString("あなたが");
				ss << "最後に解答してください";
			}
			else
			{
				top->setString("あなたは");
				ss << m_drawOrder[m_nowDrawer] << "番目に描いてください";
			}
			m_pMiddleLabel->setString(ss.str());

			if (++m_nowDrawer == m_playerNum)
			{
				// 最後の人
				m_flowPhase = FlowPhase::HIDE;
				//ss << "確認したらタッチしてテーマを見た順に、描き始めてください";
				bottom->setString("確認したらタッチして1番目の人に渡してください\n次はテーマ発表です");
			}
			else
			{
				// まだ他に見る人がいる
				std::stringstream next;
				next << "次(" << (m_nowDrawer + 1) << "人目)の人がタッチしてください。";
				bottom->setString(next.str());
			}

		}
			break;
		case RelayGameLayer::FlowPhase::HIDE:
		{
			m_flowPhase = FlowPhase::THEME;
			auto board = this->getChildByName("BigBoard");
			auto top = board->getChildByName<Label*>("TopLabel");
			auto bottom = board->getChildByName<Label*>("BottomLabel");
			top->setString("テーマ発表です。周りの人に見られないように");
			m_pMiddleLabel->setString("");
			bottom->setString("画面をタッチしてください");
		}
			break;

		case RelayGameLayer::FlowPhase::THEME:
		{
			m_flowPhase = FlowPhase::START;
			// テーマ表示
			this->_ShowTheme();

		}
			break;	
		case RelayGameLayer::FlowPhase::START:
		{
			// お絵かき開始
			this->_StartDrawing();
		}
			break;
		case RelayGameLayer::FlowPhase::PLAY:
			// お絵かき中
		{
			// メインフェーズ
			switch (m_phase)
			{
			case RelayGameLayer::Phase::ANNOUNCE:
				//return false;
				break;
			case RelayGameLayer::Phase::DRAWING:
			{
				auto canvas = this->getChildByName<LayerCanvas*>("Canvas");
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
					// 時間計測停止
					m_isCounting = true;


					// 点打ち始め
					size_t i = m_touchPoints[m_nowPage].size();
					for (; i <= m_nowLine; ++i)
					{
						m_touchPoints[m_nowPage].push_back(points());
					}
					m_touchPoints[m_nowPage][m_nowLine].push_back(locationInNode);
				
					// 描いた
					m_isDrew = true;
				}
				else
				{
					// 描かなかった
					m_isDrew = false;
					break;
				}
			}
				break;
			case RelayGameLayer::Phase::CONFIRM:
				//return false;
				break;
			default:
				break;
			}


		}
			break;
		case RelayGameLayer::FlowPhase::SHOW:
			// ひとり前の絵を見ている
		{
			// 次のキャンバスへ
			this->_NextCanvas();
			// フェーズ戻し
			m_flowPhase = FlowPhase::PLAY;
			// 時間初期化
			m_oneDrawTime = 0.f;
			this->_UpdateTimeLabel();
		}
			break;
		//case RelayGameLayer::FlowPhase::JUDGE:
		//	// 画伯発表と正誤発表
		//{

		//}
		//	break;
		case RelayGameLayer::FlowPhase::REPEAT:
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
		
		if (m_isAnimation) return;
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




		auto canvas = this->getChildByName("Canvas");
		Size s = canvas->getContentSize();
		s.width *= canvas->getScaleX();
		s.height *= canvas->getScaleY();
		Rect rect(canvas->getPositionX() - canvas->getAnchorPoint().x*s.width,
			canvas->getPositionY() - canvas->getAnchorPoint().y*s.height,
			s.width, s.height);
		if (rect.containsPoint(locationInNode))
		{
			if (m_touchPoints.size() >= m_nowPage && m_touchPoints[m_nowPage].size() > m_nowLine)
			{
				m_touchPoints[m_nowPage][m_nowLine].push_back(locationInNode);
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

		//this->getChildByName("OverTouchLayer")->setVisible(false);

		// タッチしてない時間初期化
		m_nonTouchTime = 0.f;

		// 次の線へ
		++m_nowLine;
		
	};

	
	//dispatcher->addEventListenerWithSceneGraphPriority(listener1, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(multiListener1, this);



	//_eventDispatcher->addEventListenerWithFixedPriority(listener1, 1);
}


/**
	@brief		テーマの表示
	@param[in]	なし
	@return		なし
*/
Sprite* RelayGameLayer::_ShowTheme()
{
	// リンク有効化
	m_pMiddleLabel->setEnabled(true);
	
	auto board = this->getChildByName<Sprite*>("BigBoard");
	auto top = board->getChildByName<Label*>("TopLabel");
	auto bottom = board->getChildByName<Label*>("BottomLabel");
	top->setString("テーマ");
	std::stringstream ss;
	ss << "[ " << m_nowTheme.theme << " ]文字をタッチすると検索できます";
	m_pMiddleLabel->setString(ss.str());
	bottom->setString("確認したらタッチして\n描き始めてください");

	return board;
}

/**
	@brief		最初の人が戻るを押してテーマを表示
	@param[in]	なし
	@return		なし
*/
void RelayGameLayer::_BackShowTheme()
{
	auto board = this->_ShowTheme();
	auto winSize = Director::getInstance()->getWinSize();
	// タッチスクリーン用レイヤー
	auto layer = Layer::create();
	// z: ボード > レイヤー > キャンバス(その他)
	this->addChild(layer, 10000/*this->getChildByName("Canvas")->getLocalZOrder() + 1*/, "");

	auto listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);
	listener->onTouchBegan = [](Touch* touch, Event* event){
		return true;
	};
	listener->onTouchEnded = [this, layer, board](Touch* touch, Event* event){
		board->setVisible(false);
		layer->runAction(RemoveSelf::create());
	};
	this->_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, layer);
}


/**
	@brief		お絵かき開始
	@param[in]	なし
	@return		なし
*/
void RelayGameLayer::_StartDrawing()
{
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
			
		// 上ラベル
		this->_UpdateTimeLabel();
		
		canvas->setLocalZOrder(ZOrder::CANVAS);
		// 戻る進む
		//auto menu = this->getChildByName("MenuLastButtons");
		//menu->setVisible(true);


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
	m_touchPoints.push_back(page_t());



	// 戻るボタン表示
	//this->getChildByName("MenuButtons")->getChildByName("BackButton")->setVisible(true);
	//-------------------------------------
	// 変数初期化
	m_phase = Phase::ANNOUNCE;		// アニメーションフェーズ
	m_nowDrawer = 0;				// 描く人
	m_isCounting = false;
	m_oneDrawTime = 0.f;			// タッチ時間初期化
	m_nonTouchTime = 0.f;			// タッチしていない時間初期化
}

/**
	@brief		キャンバス初期化
	@param[in]	なし
	@return		なし
*/
void RelayGameLayer::_NextCanvas()
{
	auto oldCanvas = this->getChildByName("Canvas");
	//-----------------------------------------------------
	// 新規キャンバスを既存キャンバスの下に
	auto layerCanvas = LayerCanvas::create();
	if (layerCanvas && layerCanvas->createSprite("canvas.png"))
	{
		this->addChild(layerCanvas, oldCanvas->getLocalZOrder()-1, "NewCanvas");

		layerCanvas->setPosition(oldCanvas->getPosition());
		//layerCanvas->setVisible(false);
	}

	//-----------------------------------------------------
	// DrawNode
	auto drawNode = DrawNode::create();
	{
		drawNode->setAnchorPoint(Vec2::ZERO);
		auto canvas = layerCanvas->getSprite();
		//layerCanvas->addChild(m_pDrawNode, 1);
		drawNode->setPosition(
			-(layerCanvas->getPositionX() - layerCanvas->getAnchorPoint().x*layerCanvas->getContentSize().width),
			-(layerCanvas->getPositionY() - layerCanvas->getAnchorPoint().y*layerCanvas->getContentSize().height)
			);
		canvas->addChild(drawNode, 1, "DrawNode");
	}

	oldCanvas->runAction(
		Sequence::create(
			PageTurn3D::create(0.5f, Size(24.f, 32.f)),
			CallFunc::create([this, oldCanvas, layerCanvas, drawNode](){
				// キャンバスの入れ替え
				//this->removeChild(oldCanvas);
				oldCanvas->setName(oldCanvas->getName() + std::to_string(m_nowPage));
				oldCanvas->setVisible(false);

				layerCanvas->setLocalZOrder(ZOrder::CANVAS);
				layerCanvas->setName("Canvas");
				//--------------------------
				// DrawNode入れ替え
				//m_pDrawNode->release();
				m_pDrawNode = drawNode;
				//--------------------------
				// ページ追加
				m_nowPage++;
				m_touchPoints.push_back(page_t());
				m_nowLine = 0;
				//--------------------------
				// 解答ボックス初期化
				m_pEditBox->setText("");
			}),
			nullptr
		)
	);

}


/**
	@brief		描きすぎレイヤー表示設定
	@param[in]	isVisible	表示するか
	@return		なし
*/
void RelayGameLayer::_SetOverTouchLayerVisible(bool isVisible)
{
	auto layer = this->getChildByName("OverTouchLayer");
	layer->setVisible(isVisible);
	// リスナー
	m_pListener->setSwallowTouches(isVisible);

}

/**
	@brief		累計時間ラベル更新
	@param[in]	なし
	@return		なし
*/
void RelayGameLayer::_UpdateTimeLabel()
{
	std::stringstream ss;
	ss << "一人が描ける時間 残り" << common::getFloatString(ONE_DRAW_TIME - m_oneDrawTime, 1, 8) << "秒";
	m_pLabel->setString(ss.str());
}



/**
	@brief		解答後アニメーション
	@param[in]	isAnswerd	正解か
	@return		なし
*/
void RelayGameLayer::_StartAnsweredAnimation(bool isAnswerd)
{


	// 時間計測停止
	m_isCounting = false;

	// 表示画像
	Node* spriteAnswer;
	// 正解時の演出
	FiniteTimeAction* answeredFunc;
	if (isAnswerd) 
	{
		// 正解
		// ○
		spriteAnswer = this->getChildByName("CircleCanvas");
		// パーティクル発生用
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

		answeredFunc = Sequence::create(
			// パーティクル発生
			Repeat::create(
				Sequence::create(
					DelayTime::create(0.75f),
					paticleFunc,
					nullptr
				), 5
			),
			DelayTime::create(1.5f),	// 数秒後
			Spawn::create(
				TargetedAction::create(sprite, FadeOut::create(1.f)),
				TargetedAction::create(spriteAnswer, FadeOut::create(1.f)),
				nullptr
			),
			TargetedAction::create(layer, RemoveSelf::create()),	// パーティクルレイヤー削除
			//------------------
			nullptr
		);
	}
	else
	{
		// 不正解
		// ×
		spriteAnswer = this->getChildByName("CrossCanvas");
		answeredFunc = TargetedAction::create(spriteAnswer, FadeOut::create(1.f));
	}
	this->runAction(
		Sequence::create(
			// 画像点滅
			TargetedAction::create(spriteAnswer,
				Sequence::create(
					Show::create(),
					Blink::create(0.3f, 2),
					nullptr
				)
			),
			answeredFunc,
			TargetedAction::create(spriteAnswer, RemoveSelf::create()),	// 画像削除
			TargetedAction::create(m_pNextButton, Show::create()),	// タイトルへボタン表示
			CallFunc::create([this](){
				this->_StartEnd();
			}),
			nullptr
		)
	);

	// 編集不可
	m_pEditBox->setEnabled(false);
	// 中身変換
	m_pEditBox->setText(m_nowTheme.theme.c_str());

}



/**
	@brief		最後一覧表示等メニュー処理
	@param[in]	なし
	@return		なし
*/
void RelayGameLayer::_StartEnd()
{
	m_flowPhase = FlowPhase::ANSWERED;

	// キャンバス名変更
	this->getChildByName("Canvas")->setName("Canvas" + std::to_string(m_nowPage));

	// 戻る進む
	auto menu = this->getChildByName("MenuLastButtons");
	menu->setVisible(true);
	menu->getChildByName("LastBackButton")->setVisible(1 <= m_nowPage);
	m_pButtonLabel->setString("タイトルへ");

}

#pragma endregion RelayGameLayer methods

