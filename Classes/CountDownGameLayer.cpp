/**=============================================================================
	@file		CountDownGameLayer.cpp
	@brief		タイムアタックモードレイヤークラス 実装
	@author		Masato Yamamoto
	@date		2015-02-08

	@par		[説明]
				タイムアタックモード用レイヤークラス
				テーマ当てるまでの時間を競う

	@note

		Copyright (C) 2015  saitsuyo
=============================================================================*/

#include "MultiResolution.h"
#include "CountDownGameLayer.h"
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


//const float CountDownGameLayer::TOUCH_APPLY_DELAY = 0.11666667f;	//!< 誤タッチ認識間隔 0.12秒(7フレーム)
const float CountDownGameLayer::BONUS_TIME = 5.f;		//!< ボーナス時間(max(ボーナス-使用時間,0))

#pragma region CountDownGameLayer methods

/**
	@brief		デストラクタ
	@param[in]	なし
	@return		なし
	@par		メモリの解放等
*/
CountDownGameLayer::~CountDownGameLayer()
{
}

// on "init" you need to initialize your instance
bool CountDownGameLayer::init()
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
	// パスボタン
	auto passButton = MenuItemImage::create("PassButton.png",
											"PassButton2.png",
											"PassButton3.png",
											[this, visibleSize](Ref* pSender){
		// パス

		std::stringstream ss;
		ss << "テーマは\n\n[　　　　　"
			<< m_nowTheme.theme
			<< "　　　　　]\n\nでした。\n\n"
			<< "次のテーマ発表です。\n周りに人がいないことを確認してから\n"
			<< "画面をタッチしてください。";
		this->_NextTheme(ss.str());
#if NEXT_CANVAS_TIMING == 1
		// キャンバスめくり
		this->_NextCanvas();
#endif
	});
	if (passButton)
	{
		passButton->setName("PassButton");
		//passButton->setVisible(false);
		//passButton->setEnabled(false);
		auto size = passButton->getContentSize();
		passButton->setPosition(
			origin + visibleSize - size / 2.f
			);

	}

	auto menuButtons = Menu::create(nextButton, passButton, nullptr);
	if (menuButtons)
	{
		menuButtons->setPosition(Vec2::ZERO);
		menuButtons->setVisible(false);
		this->addChild(menuButtons, ZOrder::MENUBUTTONS, "MenuButtons");
	}


	//-----------------------------------------------------
	// 最後ボタン
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
						// ページ追加
						//--m_nowPage;
						//m_touchPoints.push_back(page_t());
						//m_nowLine = 0;
						//--------------------------
						// 解答ボックス初期化
						m_pEditBox->setText(m_historyThemes[m_nowPage].theme.c_str());
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
						// ページ追加
						//--m_nowPage;
						//m_touchPoints.push_back(page_t());
						//m_nowLine = 0;
						//--------------------------
						// 解答ボックス初期化
						m_pEditBox->setText(m_historyThemes[m_nowPage].theme.c_str());
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
	// そこまで
	auto nodeMark = Node::create();
	{
		// ø
		auto spriteMark = Sprite::create("mark.png");
		{
			spriteMark->setPosition(Vec2::ZERO);
			nodeMark->addChild(spriteMark, 0, "Mark");
		}
		// 「そこまで」
		auto spriteDrawEnd = Sprite::create("CharaDrawEnd.png");
		{
			spriteDrawEnd->setPosition(spriteMark->getPosition());
			nodeMark->addChild(spriteDrawEnd, 1, "DrawEnd");
		}
		nodeMark->setPosition(visibleSize.width + spriteMark->getContentSize().width*spriteMark->getAnchorPoint().x,
							  visibleSize.height / 2.f);
		this->addChild(nodeMark, ZOrder::MOST_FRONT, "MarkNode");
	}

	return true;
}


/**
	@brief		お絵かきメイン
	@param[in]	delta		1フレームの間隔
	@return		なし
*/
void CountDownGameLayer::update(float delta)
{
	if (m_isPenalty || m_isCounting)
	{
		m_drawableTime -= delta;		// 時間減算
		if (m_isCounting)
			m_oneDrawTime += delta;		// ペナルティ中は加算しない
		if (m_drawableTime <= 0.0f)
		{
			m_drawableTime = 0.f;
			m_isPenalty = m_isCounting = false;
			this->_StartEndAnimation();
		}
		this->_UpdateTimeLabel();
	}

	switch (m_flowPhase)
	{
	case CountDownGameLayer::FlowPhase::CHECK:
		break;
	case CountDownGameLayer::FlowPhase::PLAY:
		switch (m_phase)
		{
		case CountDownGameLayer::Phase::ANNOUNCE:
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
		case CountDownGameLayer::Phase::DRAWING:
			// お絵かき中
		{
			m_nonTouchTime += delta;		// 時間加算
			// タッチ中
			//if (m_isTouching)
			//{
			//	m_drawableTime -= delta;		// 時間減算
			//	m_oneDrawTime += delta;
			//	this->_UpdateTimeLabel();				
			//}
			m_pDrawNode->clear();

			Color4F color(0.05f, 0.05f, 0.05f, 1.f);
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
		case CountDownGameLayer::Phase::CONFIRM:
			// ok or cancel
			break;
		default:
			break;
		}


		break;
	//case CountDownGameLayer::FlowPhase::VOTE:
	//	break;
	//case CountDownGameLayer::FlowPhase::JUDGE:
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
void CountDownGameLayer::restart(const std::string& from)
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

		// 筆太さ
		auto def = UserDefault::getInstance();
		m_brushSize = 5.f;
		m_brushSize = def->getFloatForKey("BrushSize", m_brushSize);

		//-----------------------------------------------------
		// テーマ設定
		this->_ChoiceTheme();

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
void CountDownGameLayer::editBoxEditingDidBegin(EditBox* editBox)
{
	log("editBox %p DidBegin !", editBox);
	// キャンバス非表示
	this->getChildByName("Canvas")->setVisible(false);
	// 時間計測停止
	m_isCounting = false;
}
// OKボタン押したら呼ばれる
void CountDownGameLayer::editBoxTextChanged(EditBox* editBox, const std::string& text)
{
	log("editBox %p TextChanged, text: %s ", editBox, text.c_str());


	// 判定
	m_isAnswerd = (text == m_nowTheme.kana);
	if (m_isAnswerd)
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
		bool isPenalty = true;
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
void CountDownGameLayer::editBoxEditingDidEnd(EditBox* editBox)
{
	log("editBox %p DidEnd !", editBox);

	// キャンバス表示
	this->getChildByName("Canvas")->setVisible(true);
	std::string text = editBox->getText();
	// 時間計測開始 正解時は停止のまま
	m_isCounting = !m_isAnswerd;
}

// ボックス終了後に呼ばれる
void CountDownGameLayer::editBoxReturn(EditBox* editBox)
{
	log("editBox %p was returned !", editBox);

}
//---------------------------------------------------------------------------------------------------




/**
	@brief		メンバ変数初期化
	@param[in]	なし
	@return		なし
*/
void CountDownGameLayer::_SetDefaultValues()
{

	// アニメーションフラグ折る
	m_isAnimation = false;

	// 回してね表示へ
	m_flowPhase = FlowPhase::NEXT;

	// タッチしていない
	//m_isTouching = false;
	m_isPenalty = false;
	// 時間計測停止
	m_isCounting = false;

	m_isAnswerd = false;

	// 描きはじめ
	m_nowPage = 0;
	m_nowLine = 0;
	
	//-----------------------------------------------------
	// テーマ読み込み
#pragma region テーマ読み込み
	//std::string csvName("theme_numeric.csv");
	std::string csvName("ThemeCountDown.csv");
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

	// 履歴拡張
	m_historyThemes.reserve(m_themes.size());

	
}


/**
	@brief		タッチリスナーの設定
	@param[in]	なし
	@return		なし
*/
void CountDownGameLayer::_SetTouchListener()
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
		case CountDownGameLayer::FlowPhase::NEXT:
			// 次の人へ回してね☆
		{
			// 役割表示へ
			m_flowPhase = FlowPhase::CHECK;
			// テーマ表示
			auto board = this->getChildByName<Sprite*>("BigBoard");
			auto top = board->getChildByName<Label*>("TopLabel");
			auto middle = board->getChildByName<Label*>("MiddleLabel");
			auto bottom = board->getChildByName<Label*>("BottomLabel");

			top->setString("テーマ");
			middle->setString(m_nowTheme.theme);
			bottom->setString("確認したらタッチし 描き始めてください");
			// ボード表示
			board->setVisible(true);

		}
			break;
		case CountDownGameLayer::FlowPhase::CHECK:
			// 自分の役割を表示
		{
			// ゲームスタート
			this->_StartDrawing();
		}
			break;
		case CountDownGameLayer::FlowPhase::PLAY:
			// お絵かき中
		{
			// メインフェーズ
			switch (m_phase)
			{
			case CountDownGameLayer::Phase::ANNOUNCE:
				//return false;
				break;
			case CountDownGameLayer::Phase::DRAWING:
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
					// タッチ開始
					//m_isTouching = true;

					// 時間計測停止
					m_isCounting = true;

					// 時間初期化
					m_oneDrawTime = 0.f;

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
			case CountDownGameLayer::Phase::CONFIRM:
				//return false;
				break;
			default:
				break;
			}


		}
			break;
		//case CountDownGameLayer::FlowPhase::VOTE:
		//	// 画伯投票
		//{

		//}
		//	break;
		//case CountDownGameLayer::FlowPhase::JUDGE:
		//	// 画伯発表と正誤発表
		//{

		//}
		//	break;
		case CountDownGameLayer::FlowPhase::REPEAT:
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

		// タッチしてない
		//m_isTouching = false;

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
	@brief		テーマ選定
	@param[in]	なし
	@return		なし
*/
void CountDownGameLayer::_ChoiceTheme()
{
	Theme tmp;
	bool isLoop = true;
	unsigned int i = 0;		// ループ回数
	// 過去出題テーマに被らないように
	while (isLoop)
	{
		// テーマ選定
		int theme = (2 <= m_themes.size()) ? ns_Random::Random::getInstance()->getRandomInt(0, m_themes.size() - 1) : 0;
		tmp = m_themes[theme];
		

		if (++i > m_themes.size())
		{
			// 全部のテーマをやった場合
			auto it = m_historyThemes.end();
			it -= 50;	// 直近50件に含まれていなければループを抜ける
			isLoop = std::find(it, m_historyThemes.end(), tmp) != m_historyThemes.end();
		}
		else
		{
			// 出題テーブルと重複がないか照合
			isLoop = std::find(m_historyThemes.begin(), m_historyThemes.end(), tmp) != m_historyThemes.end();
		}
	}
	m_nowTheme = tmp;

	m_historyThemes.push_back(tmp);

}

/**
	@brief		テーマの表示
	@param[in]	なし
	@return		なし
*/
void CountDownGameLayer::_ShowTheme()
{
	auto winSize = Director::getInstance()->getWinSize();
	// タッチスクリーン用レイヤー
	auto layer = Layer::create();
	// z: ボード > レイヤー > キャンバス(その他)
	this->addChild(layer, 10000/*this->getChildByName("Canvas")->getLocalZOrder() + 1*/, "");

	auto board = this->getChildByName<Sprite*>("BigBoard");
	auto top = board->getChildByName<Label*>("TopLabel");
	auto middle = board->getChildByName<Label*>("MiddleLabel");
	auto bottom = board->getChildByName<Label*>("BottomLabel");
	

	top->setString("テーマ");
	middle->setString(m_nowTheme.theme);
	bottom->setString("確認したらタッチし 描き始めてください");

	// ボード表示
	board->setVisible(true);

	board->setLocalZOrder(layer->getLocalZOrder() + 1);

	auto listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);
	listener->onTouchBegan = [](Touch* touch, Event* event){
		return true;
	};
	listener->onTouchEnded = [this, layer, board](Touch* touch, Event* event){
		board->setVisible(false);
		layer->runAction(RemoveSelf::create());
		
#if NEXT_CANVAS_TIMING == 3
		// キャンバスめくり
		this->_NextCanvas();
#endif
	};
	this->_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, layer);

}
/**
	@brief		次のテーマ表示へ
	@param[in]	msg	メッセージ
	@return		なし
	@par		テーマ表示のための(一人で見るよう)アナウンス～
				新規キャンバスめくるまで
*/
void CountDownGameLayer::_NextTheme(const std::string msg)
{
	// 時間計測停止
	m_isCounting = false;

	auto visibleSize = Director::getInstance()->getVisibleSize();
	// 答え発表兼一人で見てね
	//-----------------------------------------------------
	// パスとか用テーマ発表レイヤー
	auto layerPass = Layer::create();
	{
		this->addChild(layerPass, ZOrder::MOST_FRONT, "PassLayer");

		// 背景用
		auto sprite = Sprite::create();
		sprite->setTextureRect(Rect(0, 0, visibleSize.width, visibleSize.height));
		sprite->setPosition(visibleSize / 2.f);
		sprite->setColor(Color3B::BLACK);
		sprite->setOpacity(192);
		layerPass->addChild(sprite, 0);

		auto label = Label::createWithSystemFont(msg,
												 "GenShinGothic-Medium.ttf",
												 calcFontSize(40.f));
		{
			label->setPosition(layerPass->getContentSize() / 2.f);
			label->setAlignment(TextHAlignment::CENTER, TextVAlignment::CENTER);
			label->setColor(Color3B(250, 250, 15));
			layerPass->addChild(label, 10, "Label");
		}

		auto listener = EventListenerTouchOneByOne::create();
		listener->setSwallowTouches(true);
		listener->onTouchBegan = [this](Touch* touch, Event* event)
		{
			return true;
		};
		listener->onTouchEnded = [this, layerPass](Touch* touch, Event* event){
			layerPass->runAction(RemoveSelf::create());
			// テーマ発表
			this->_ChoiceTheme();
			this->_ShowTheme();
#if NEXT_CANVAS_TIMING == 2
			// キャンバスめくり
			this->_NextCanvas();
#endif
		};
		this->_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, layerPass);
	}
}



/**
	@brief		お絵かき開始
	@param[in]	なし
	@return		なし
*/
void CountDownGameLayer::_StartDrawing()
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
	m_touchPoints.push_back(page_t());



	// 戻るボタン表示
	//this->getChildByName("MenuButtons")->getChildByName("BackButton")->setVisible(true);
	//-------------------------------------
	// 変数初期化
	m_phase = Phase::ANNOUNCE;		// アニメーションフェーズ
	//m_isTouching = false;
	m_oneDrawTime = 0.f;			// タッチ時間初期化
	m_nonTouchTime = 0.f;			// タッチしていない時間初期化
}

/**
	@brief		キャンバス初期化
	@param[in]	なし
	@return		なし
*/
void CountDownGameLayer::_NextCanvas()
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

// わかったボタン
void CountDownGameLayer::_ButtonCallback(cocos2d::Ref* pSender)
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
	////case CountDownGameLayer::FlowPhase::NEXT:
	////	break;
	////case CountDownGameLayer::FlowPhase::CHECK:
	////	break;
	////case CountDownGameLayer::FlowPhase::PLAY:
	////	break;
	//case CountDownGameLayer::FlowPhase::CONFIRM:
	//	this->_StartVote();
	//	break;
	//case CountDownGameLayer::FlowPhase::VOTE:
	//	// 開票
	//	this->_StartCounting();
	//	break;
	//case CountDownGameLayer::FlowPhase::COUNTING:
	//	// 開票終了
	//	this->_StartedCounting();
	//	break;
	//case CountDownGameLayer::FlowPhase::JUDGE:
	//	this->_StartShowCanvas();
	//	break;
	//case CountDownGameLayer::FlowPhase::SHOW_CANVAS:
	//	this->_StartOpenTheme();
	//	break;
	//case CountDownGameLayer::FlowPhase::OPEN_THEME:
	//	dynamic_cast<SceneAtelierMain*>(this->getParent()->getParent())->toTitle();
	//	//restart(this->getName());
	//	break;
	//case CountDownGameLayer::FlowPhase::REPEAT:
	//	break;
	//case CountDownGameLayer::FlowPhase::END:
	//	break;
	//default:
	//	break;
	//}
	


}



/**
	@brief		累計時間ラベル更新
	@param[in]	なし
	@return		なし
*/
void CountDownGameLayer::_UpdateTimeLabel()
{
	std::stringstream ss;
	ss << "残り" << common::getFloatString(m_drawableTime, 1, 8) << "秒";
	m_pLabel->setString(ss.str());
}



/**
	@brief		解答正解アニメーション開始
	@param[in]	なし
	@return		なし
*/
void CountDownGameLayer::_StartAnsweredAnimation()
{
	// 正解

	// 時間計測停止
	m_isCounting = false;

	//m_flowPhase = FlowPhase::ANSWERED;

	// ボーナス
	m_drawableTime += MAX(BONUS_TIME - m_oneDrawTime, 0.0f);
	this->_UpdateTimeLabel();

	// ○
	auto circle = this->getChildByName("CircleCanvas");
	this->runAction(
		Sequence::create(
			TargetedAction::create(circle,
				Sequence::create(
					Show::create(),
					Blink::create(0.3f, 2),
					Hide::create(),
					nullptr
				)
			),
			CallFunc::create([this](){
				std::stringstream ss;
				ss	//< "テーマは\n\n[　　　　　"
					//<< m_nowTheme.theme
					//<< "　　　　　]\n\nでした。\n\n"
					<< "次のテーマ発表です。\n周りに人がいないことを確認してから\n"
					<< "画面をタッチしてください。";
				this->_NextTheme(ss.str());
			}),
			nullptr
		)
	);

	// 編集不可
	//m_pEditBox->setEnabled(false);
	// 中身変換
	m_pEditBox->setText(m_nowTheme.theme.c_str());

	// パスボタン非表示
	//this->getChildByName("MenuButtons")->getChildByName("PassButton")->setVisible(false);

	// パーティクル発生
	// 加算合成なので見やすくなるように少し暗く
	//auto visibleSize = Director::getInstance()->getVisibleSize();
	//auto layer = Layer::create();
	//auto sprite = Sprite::create();	// 背景用
	//layer->addChild(sprite, 0);
	//sprite->setTextureRect(Rect(0, 0, visibleSize.width, visibleSize.height));
	//sprite->setPosition(visibleSize / 2.f);
	//sprite->setColor(Color3B::BLACK);
	//sprite->setOpacity(192);
	//
	//auto particleBatch = ParticleBatchNode::create("particle_texture.png");
	//this->addChild(particleBatch, ZOrder::MOST_FRONT, "ParticleBatch_Texture");
	//
	//this->addChild(layer, ZOrder::PARTICLE, "ParticleLayer");
	//
	//// パーティクル定期発生
	//auto paticleFunc = CallFunc::create([&, visibleSize, particleBatch](){
	//
	//	auto particleFw = common::createParticle("particle_fireworks_texture.plist");
	//
	//	auto size = particleFw->getContentSize();
	//
	//	// 座標決め
	//	float posX = ns_Random::Random::getInstance()->getRandom<float>(size.width / 2.f, visibleSize.width - size.width / 2.f);
	//	float posY = ns_Random::Random::getInstance()->getRandom<float>(size.height / 2.f, visibleSize.height - size.height / 2.f);
	//	particleFw->setPosition(posX, posY);
	//
	//
	//	particleBatch->addChild(particleFw, 0, "Fireworks");
	//
	//});
	//
	//this->runAction(
	//	Sequence::create(
	//		Repeat::create(
	//			Sequence::create(
	//				DelayTime::create(0.75f),
	//				paticleFunc,			// パーティクル発生
	//				nullptr
	//			), 5
	//		),
	//		DelayTime::create(1.5f),	// 数秒後
	//		Spawn::create(
	//			TargetedAction::create(sprite, FadeOut::create(1.f)),
	//			TargetedAction::create(circle, FadeOut::create(1.f)),
	//			nullptr
	//		),
	//		TargetedAction::create(layer, RemoveSelf::create()),	// パーティクルレイヤー削除
	//		TargetedAction::create(circle, RemoveSelf::create()),	// ○削除
	//		TargetedAction::create(m_pNextButton, Show::create()),	// タイトルへボタン表示
	//		//------------------
	//		nullptr
	//	)
	//);

}



/**
	@brief		最後終了アニメーション
	@param[in]	なし
	@return		なし
*/
void CountDownGameLayer::_StartEndAnimation()
{
	m_flowPhase = FlowPhase::ANSWERED;

	// キャンバス名変更
	this->getChildByName("Canvas")->setName("Canvas" + std::to_string(m_nowPage));

	// EditBox回答不可
	m_pEditBox->setEnabled(false);

	// パスボタン
	auto menu = this->getChildByName("MenuButtons");
	menu->getChildByName<MenuItemImage*>("PassButton")->setEnabled(false);

	// 背景
	auto visibleSize = Director::getInstance()->getVisibleSize();
	auto layer = Layer::create();
	layer->setPosition(visibleSize / 2.f);
	auto sprite = Sprite::create();
	sprite->setTextureRect(Rect(0.f, 0.f, visibleSize.width, visibleSize.height));
	sprite->setColor(Color3B::BLACK);
	sprite->setOpacity(64);
	//sprite->setPosition(visibleSize / 2.f);
	layer->addChild(sprite, 0);
	auto pbnode = ParticleBatchNode::create("particle_stars.png");
	layer->addChild(pbnode, 10, "Particle");
	this->addChild(layer, ZOrder::MOST_FRONT - 1);
	auto listener = EventListenerTouchAllAtOnce::create();
	auto dispatcher = Director::getInstance()->getEventDispatcher();
	listener->onTouchesBegan = [layer, pbnode](const std::vector<Touch*>& touches, Event* event)
	{};
	listener->onTouchesMoved = [layer, pbnode](const std::vector<Touch*>& touches, Event* event)
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
	listener->onTouchesEnded = [layer, pbnode](const std::vector<Touch*>& touches, Event* event)
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
	dispatcher->addEventListenerWithSceneGraphPriority(listener, layer);

	// そこまで
	auto node = this->getChildByName("MarkNode");
	auto mark = node->getChildByName("Mark");
	auto size = mark->getContentSize();
	const float duration = 2.f;
	node->runAction(
		Sequence::create(
			Spawn::create(
				// ノード(マーク、文字)移動
				MoveTo::create(duration,
					Vec2(-mark->getContentSize().width*mark->getAnchorPoint().x,
					node->getPositionY())
				),
				// マーク回転
				TargetedAction::create(mark, 
					RotateTo::create(duration, -720.f)
				),
				nullptr	
			),
			CallFunc::create([this, layer](){
				layer->runAction(Sequence::create(
									FadeOut::create(1.f),
									RemoveSelf::create(),
									nullptr));
				// 戻る進む
				auto menu = this->getChildByName("MenuLastButtons");
				menu->setVisible(true);
				menu->getChildByName("LastBackButton")->setVisible(1 <= m_nowPage);
				m_pButtonLabel->setString("タイトルへ");

				// テーマ表示
				m_pEditBox->setText(m_nowTheme.theme.c_str());
			}),
			nullptr
		)
	);
}

#pragma endregion CountDownGameLayer methods

