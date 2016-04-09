/**=============================================================================
	@file		EseMainLayer.cpp
	@brief		画伯あぶり出しレイヤークラス 実装
	@author		Masato Yamamoto
	@date		2015-01-31

	@par		[説明]
				画伯あぶり出しメインレイヤー

	@note

		Copyright (C) 2015  saitsuyo
=============================================================================*/

#include "MultiResolution.h"
#include "EseMainLayer.h"
#include "SceneAtelierMain.h"
#include "OptionLayer.h"
#include "LayerCanvas.hpp"


#include "Random.h"
#include "Common.h"


#include <fstream>
#include <sstream>

#pragma execution_character_set("utf-8")

USING_NS_CC_EXT;
USING_NS_CC;




const float EseMainLayer::TOUCH_APPLY_DELAY = 0.11666667f;	// 0.12秒(7フレーム)


#pragma region EseMainLayer methods

/**
	@brief		デストラクタ
	@param[in]	なし
	@return		なし
	@par		メモリの解放等
*/
EseMainLayer::~EseMainLayer()
{
	m_colors.clear();
}

// on "init" you need to initialize your instance
bool EseMainLayer::init()
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
		spriteBack->setPosition(Vec2(
			visibleSize.width / 2.f + origin.x,
			visibleSize.height / 2.f + origin.y));

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
	// 矢印
	//auto spriteArrow = Sprite::create("0019_4.png");
	//if (spriteArrow)
	//{
	//	spriteArrow->setName("Arrow");
	//
	//	spriteArrow->setVisible(false);
	//	spriteArrow->setScale(0.5f);
	//	spriteArrow->setPosition(
	//		origin.x + visibleSize.width / 2.f,
	//		origin.y + visibleSize.height / 3.f*2.f);
	//
	//	m_arrowPos = spriteArrow->getPosition();
	//
	//	this->addChild(spriteArrow, 5);
	//}

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

	auto spriteSmallBoard2 = Sprite::createWithTexture(spriteSmallBoard->getTexture());
	{
		auto size = spriteSmallBoard2->getContentSize();
		spriteSmallBoard2->setPosition(
			origin.x + visibleSize.width + size.width / 2.f,
			origin.y + size.height / 2.f
			);
		this->addChild(spriteSmallBoard2, ZOrder::BOTTOMBOARD, "SmallBoard2");

		auto label = Label::createWithSystemFont("", "GenShinGothic-Medium.ttf", calcFontSize(40.f));
		label->setHorizontalAlignment(TextHAlignment::CENTER);
		label->setPosition(size/ 2.f);
		spriteSmallBoard2->addChild(label, 0, "Label");
	}


	//-----------------------------------------------------
	// ラベル設定
	// 大テーマ 文字列はrestartで追加する
	
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
	// 戻るボタン
	auto backButton = MenuItemImage::create("backButton.png",
											"backButton2.png",
											[&](Ref* pSender){
		// 戻るボタン

		// おっきなふぇーず
		if (FlowPhase::CONFIRM == m_flowPhase)
		{
			// プレイ中に戻す
			m_flowPhase = FlowPhase::PLAY;
			// ラベル
			std::stringstream ssBigTheme;
			ssBigTheme << "大テーマ[ " << m_bigTheme << " ] ";
			m_pLabel->setString(ssBigTheme.str());

			// ボタン
			m_pNextButton->setVisible(false);
			m_pButtonLabel->setVisible(false);
			// 小テーマ確認
			this->getChildByName("MenuButtons")->getChildByName("CheckThemeButton")->setVisible(true);
			
			// パーティクル
			this->getChildByName("ParticleBatch_Stars")->setVisible(true);
		}
		else if (FlowPhase::PLAY != m_flowPhase)
			return;
		//// メインフェーズ
		//if (m_phase != Phase::DRAWING)
		//	return;



		// 1筆以上あった場合のみ
		if (1 <= m_nowDrawer && !m_touchPoints.empty())
		{
			// 描く人戻す
			--m_nowDrawer;
			// 線(点)消去
			m_touchPoints.pop_back();
			this->_UpdateDrawerDisp();
		}

	});
	if (backButton)
	{
		backButton->setName("BackButton");
		// ボタン非表示
		//backButton->setVisible(false);
		auto canvas = layerCanvas->getSprite();
		Vec2 leftBottom = canvas->convertToWorldSpace(Vec2::ZERO);
		
		//backButton->setPosition(
		//	origin.x + size.width,
		//	origin.y + size.height / 2.f);
		backButton->setPosition(leftBottom / 2.f);
	}
	// 次ボタン
	auto nextButton = MenuItemImage::create("button.png",
											"button2.png",
											CC_CALLBACK_1(EseMainLayer::_ButtonCallback, this));
	if (nextButton)
	{
		nextButton->setName("NextButton");
		// ボタン非表示
		nextButton->setVisible(false);
		auto size = nextButton->getContentSize();
		nextButton->setPosition(
			origin.x + visibleSize.width - size.width / 2.f,
			origin.y + size.height / 2.f);
		
		m_pNextButton = nextButton;
	}
	// 小テーマ確認ボタン
	auto checkThemeButton = MenuItemImage::create("CheckThemeButton.png",
												"CheckThemeButton2.png",
												"CheckThemeButton3.png",
												[&](Ref* pSender){
		// 小テーマ確認
		this->_ShowSmallTheme();
	});
	if (checkThemeButton)
	{
		checkThemeButton->setName("CheckThemeButton");
		//checkThemeButton->setVisible(false);
		auto size = checkThemeButton->getContentSize();
		checkThemeButton->setPosition(
			origin + visibleSize - size / 2.f
			);

	}

	auto menuButtons = Menu::create(backButton, nextButton, checkThemeButton, nullptr);
	if (menuButtons)
	{
		menuButtons->setPosition(Vec2::ZERO);
		menuButtons->setVisible(false);
		this->addChild(menuButtons, ZOrder::MENUBUTTONS, "MenuButtons");
	}
	m_pButtonLabel = Label::createWithSystemFont("次へ", "GenShinGothic-Medium.ttf", calcFontSize(20.f));
	if (m_pButtonLabel)
	{
		m_pButtonLabel->setPosition(nextButton->getPosition());
		m_pButtonLabel->setHorizontalAlignment(TextHAlignment::CENTER);
		// ラベル非表示
		m_pButtonLabel->setVisible(false);
		m_pButtonLabel->setColor(Color3B(5, 5, 5));
		this->addChild(m_pButtonLabel, ZOrder::NEXTBUTTONLABEL, "ButtonLabel");
		
	}
	//-----------------------------------------------------
	// 描く人表示
	m_pTurnLabel = Label::createWithSystemFont(" ", "GenShinGothic-Medium.ttf", calcFontSize(30.f));
	{
		m_pTurnLabel->setPosition(15.f, visibleSize.height - m_pTurnLabel->getContentSize().height);
		m_pTurnLabel->setAnchorPoint(Vec2(0.0f, 0.5f));
		m_pTurnLabel->setVisible(false);
		this->addChild(m_pTurnLabel, ZOrder::TURN_LABEL, "TurnLabel");
	}
	// 描く人ノード
	m_pDrawNodeDrawer = DrawNode::create();
	{
		m_drawerDispRadius = 20.f;
		m_drawerDispMarginY = m_drawerDispRadius*2.f;
		m_drawerDispStaPos.x = m_drawerDispRadius + m_drawerDispMarginY;
		m_drawerDispStaPos.y = (m_pTurnLabel->getPositionY() - m_pTurnLabel->getContentSize().height) - m_drawerDispMarginY;

		m_pDrawNodeDrawer->setPosition(Vec2::ZERO);
		this->addChild(m_pDrawNodeDrawer, ZOrder::DRAWER_NODE, "DrawNodeDrawer");
	}
	// nowパーティクル
	auto particleBatch = ParticleBatchNode::create("particle_stars.png");
	{
		particleBatch->setVisible(false);

		this->addChild(particleBatch, ZOrder::PARTICLE, "ParticleBatch_Stars");
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
	// 投票ボタン
	auto spritePlayerButtonDisabled = SpriteBatchNode::create("PlayerButton_Disabled.png");
	this->addChild(spritePlayerButtonDisabled);
	Vector<MenuItem*> voteButtons;
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			int no = i * 3 + j;	// 通し番号
			std::string pre("PlayerButton_");
			std::stringstream normal;
			std::stringstream selected;
			normal << pre << "Normal" << no << ".png";
			selected << pre << "Selected" << no << ".png";
			
			auto itemButton = MenuItemSprite::create(
				Sprite::create(normal.str()),
				Sprite::create(selected.str()),
				Sprite::createWithTexture(spritePlayerButtonDisabled->getTexture()),
				[&](Ref* pSender)
			{
				auto targetButton = dynamic_cast<MenuItemSprite*>(pSender);
				auto parent = targetButton->getParent();
				// 現在の投票者のボタンを有効化

				if (m_nowVoter < m_playerNum)
				{
					dynamic_cast<MenuItemSprite*>(parent->getChildByTag(m_nowVoter))->setEnabled(true);
					auto tag = targetButton->getTag();

					m_nowVoter++;
					m_pollNums[tag]++;
					if (m_nowVoter == m_playerNum)
					{
						// 全員投票し終わった
						// 全員の投票者ボタンの無効化
						auto buttons = parent->getChildren();
						for (auto button : buttons)
							dynamic_cast<MenuItemSprite*>(button)->setEnabled(false);

						// ボタンとラベルの設定
						m_pNextButton->setVisible(true);
						m_pButtonLabel->setVisible(true);
						m_pButtonLabel->setString("次へ");

						// 上ラベル
						m_pLabel->setString("全員の投票が完了しました");
						m_pLabel->setHorizontalAlignment(TextHAlignment::CENTER);
						m_pLabel->setPositionX(m_pLabel->getParent()->getContentSize().width / 2.f);
						m_pLabel->setColor(Color3B(250, 250, 250));

						// メッセージラベルを削除
						m_pLabel->getParent()->removeChildByName("SubLabel", true);
					}
					else
					{
						// 次の投票者のボタンを無効化
						dynamic_cast<MenuItemSprite*>(parent->getChildByTag(m_nowVoter))->setEnabled(false);
						//---------------------------------------
						// ラベル設定
						std::stringstream ss;
						ss << m_nowVoter + 1 << "P";
						m_pLabel->setString(ss.str());
						m_pLabel->setColor(Color3B(m_colors[m_nowVoter]));
					}
				}
			}
			);
			auto size = itemButton->getContentSize();
			//itemButton->setPosition(origin.x + j*(size.width + 30),
			//						origin.y + i*(size.height + 30));
			float marginX = size.width / 2.f;	// 間隔
			float posX = no - 4.f;
			itemButton->setPosition(posX * (size.width + marginX),
				size.height);

			itemButton->setTag(no);

			auto label = Label::createWithSystemFont("", "GenShinGothic-Regular.ttf", calcFontSize(40.f));
			{
				itemButton->addChild(label, 0, "Label");
				label->setPosition(
					size / 2.f
					);
				label->setAlignment(TextHAlignment::CENTER, TextVAlignment::CENTER);
			}
			voteButtons.pushBack(itemButton);
		}
	}
	// 先頭投票者ボタンの無効化
	voteButtons.front()->setEnabled(false);

	auto menuVoteButtons = Menu::createWithArray(voteButtons);
	{
		menuVoteButtons->setVisible(false);
		auto size = spritePlayerButtonDisabled->getTexture()->getContentSize();
		auto canvasBottom = layerCanvas->getPosition().y - layerCanvas->getContentSize().height*layerCanvas->getAnchorPoint().y;
		menuVoteButtons->setPosition(visibleSize.width / 2.f,
			canvasBottom - size.height*3.5f / 2.f);
		this->addChild(menuVoteButtons, ZOrder::VOTEBUTTONS, "MenuVoteButtons");
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
	//	ADD_BUTTON_PUSHED(buttonOption, EseMainLayer::_ControlButtonPushed);
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
void EseMainLayer::update(float delta)
{
	switch (m_flowPhase)
	{
	case EseMainLayer::FlowPhase::NEXT:
		_MoveArrow(delta);
		break;
	case EseMainLayer::FlowPhase::CHECK:
		_MoveArrow(delta);
		break;
	case EseMainLayer::FlowPhase::PLAY:
		switch (m_phase)
		{
		case EseMainLayer::Phase::ANNOUNCE:
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
		case EseMainLayer::Phase::DRAWING:
			// お絵かき中
		{
			m_nonTouchTime += delta;		// 時間加算
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
		case EseMainLayer::Phase::CONFIRM:
			// ok or cancel
			break;
		default:
			break;
		}

		// 全員2巡した場合
		if (m_nowDrawer >= m_playerNum * 2
			&& TOUCH_APPLY_DELAY <= m_nonTouchTime)
		{
			// 確認
			m_flowPhase = FlowPhase::CONFIRM;
			
			// ラベルの設定
			m_pLabel->setString("全員描き終わりました これでよろしいですか？");

			// ボタンとラベルの設定
			m_pNextButton->setVisible(true);
			m_pButtonLabel->setVisible(true);
			m_pButtonLabel->setString("投票へ");
			
			// 小テーマ確認ボタン非表示
			this->getChildByName("MenuButtons")->getChildByName("CheckThemeButton")->setVisible(false);
		
			// パーティクル
			this->getChildByName("ParticleBatch_Stars")->setVisible(false);
			


			//m_pLabel->setString("画伯だと思う人を指さしてください。");
			//m_pLabel->setVisible(true);


		}

		break;
	case EseMainLayer::FlowPhase::VOTE:
		break;
	case EseMainLayer::FlowPhase::JUDGE:
		break;
	default:
		break;
	}
}

/**
	@brief		画伯メイン開始
	@param[in]	from	元レイヤー
	@return		なし
*/
void EseMainLayer::restart(const std::string& from)
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
			m_bigTheme = it->first;


			// 小テーマ選定
			auto size = m_themes[m_bigTheme].size();
			m_smallTheme = m_themes[m_bigTheme][ns_Random::Random::getInstance()->getRandomInt(0, size - 1)];

			//m_themes.clear();

		}
		else
		{
			// GM入力
			m_bigTheme = scene->getBigTheme();
			m_smallTheme = scene->getSmallTheme();
		}

		//-----------------------------------------------------
		// ラベル設定
		// 大テーマ
		//std::stringstream ss;
		//ss << "大テーマ[ " << m_bigTheme << " ] ";
		//auto bigThemeLabel = this->getChildByName<Label*>("BigThemeLabel");
		//bigThemeLabel->setString(ss.str());
		//bigThemeLabel->setPosition(
		//	origin.x + visibleSize.width / 2.f,
		//	origin.y + visibleSize.height - bigThemeLabel->getContentSize().height / 1.5f);

		// 小テーマ
		//this->getChildByName<Label*>("Label")->setString("小テーマ発表です。周りに人がいないことを確認してから画面をタッチしてください。");

		m_pLabel->setString("小テーマ・画伯発表");

		//-----------------------------------------------------
		// 画伯選出
		m_esePlayer = ns_Random::Random::getInstance()->getRandomInt(0, m_playerNum - 1);

		//-----------------------------------------------------
		// 矢印
//		this->getChildByName("Arrow")->setVisible(true);


		//-----------------------------------------------------
		// 投票ボタン
		// 非参加者のボタン無効化
		auto menuVoteButtons = this->getChildByName<Menu*>("MenuVoteButtons");
		auto voteButtons = menuVoteButtons->getChildren();
		for (int i = m_playerNum; i < 9; ++i)
		{
			dynamic_cast<MenuItem*>(voteButtons.at(i))->setEnabled(false);
		}

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

/**
	@brief		メンバ変数初期化
	@param[in]	なし
	@return		なし
*/
void EseMainLayer::_SetDefaultValues()
{

	// アニメーションフラグ折る
	m_isAnimation = false;

	// 回してね表示へ
	m_flowPhase = FlowPhase::NEXT;

	// 経過時間初期化
	m_time = 0.f;

	// 描きはじめ
	m_nowDrawer = 0;

	// 投票者
	m_nowVoter = 0;

	// 勝敗
	m_gameState = GameState::NONE;
	
	
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
	std::string csvName("ThemeEse.csv");
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

		// 書式は "大テーマ,小テーマ"
		std::string bigTheme;
		std::getline(ss, bigTheme, ',');
		std::string smallTheme;
		std::getline(ss, smallTheme, ',');
		
		std::string kanaTheme;
		ss >> kanaTheme;

		// UTF-8のEOF検知用
		if (kanaTheme.empty())
			break;

		// 配列に追加
		m_themes[bigTheme].push_back(smallTheme);


	}

	//*/
#pragma endregion	// テーマ読み込み

	
}


/**
	@brief		タッチリスナーの設定
	@param[in]	なし
	@return		なし
*/
void EseMainLayer::_SetTouchListener()
{
	// シングルタッチイベントリスナー作成
	//auto listener1 = EventListenerTouchOneByOne::create();
	// マルチタッチリスナー
	auto multiListener1 = EventListenerTouchAllAtOnce::create();
	auto dispatcher = this->getEventDispatcher();

	//listener1->setSwallowTouches(true);
	multiListener1->setEnabled(true);

	// タッチした場合
	//listener1->onTouchBegan = [&](Touch* touch, Event* event)
	//{
	//	if (m_isAnimation) return false;
	//	switch (m_flowPhase)
	//	{
	//	case EseMainLayer::FlowPhase::NEXT:
	//		// 次の人へ回してね☆
	//	{
	//		// 役割表示へ
	//		m_flowPhase = FlowPhase::CHECK;
	//		// 表示文字の変更
	//		auto board = this->getChildByName("BigBoard");
	//		auto top = board->getChildByName<Label*>("TopLabel");
	//		auto middle = board->getChildByName<Label*>("MiddleLabel");
	//		auto bottom = board->getChildByName<Label*>("BottomLabel");
	//		//std::stringstream ss;
	//		if (m_nowDrawer == m_esePlayer)
	//		{
	//			// 画伯の人
	//			//ss << "あなたが<< 画伯 >>です。";
	//			top->setString("あなたが");
	//			middle->setString("画伯");
	//		}
	//		else
	//		{
	//			// 芸術家
	//			//ss << "小テーマは[ " << m_smallTheme << " ] です。";
	//			top->setString("小テーマ");
	//			middle->setString(m_smallTheme);
	//		}
	//		if (m_nowDrawer == 0)
	//		{
	//			// 最初の人がタッチした時
	//			// ラベルに大テーマを表示
	//			std::stringstream ssBigTheme;
	//			ssBigTheme << "大テーマ[ " << m_bigTheme << " ] ";
	//			m_pLabel->setString(ssBigTheme.str());
	//		}
	//		if (m_nowDrawer + 1 == m_playerNum)
	//		{
	//			// 最後の人
	//			//ss << "確認したらタッチしてテーマを見た順に、描き始めてください";
	//			bottom->setString("確認したらタッチしてテーマを見た順に\n描き始めてください");
	//		}
	//		else
	//		{
	//			// まだ他に見る人がいる
	//			// ss << "確認したらタッチして次の人に渡してください";
	//			bottom->setString("確認したらタッチして次の人に渡してください");
	//		}
	//		//m_pLabel->setString(ss.str());
	//	}
	//		return false;
	//		break;
	//	case EseMainLayer::FlowPhase::CHECK:
	//		// 自分の役割を表示
	//	{
	//		// 人数分見たら
	//		if (++m_nowDrawer == m_playerNum)
	//		{
	//			// ゲームスタート
	//			_StartDrawing();
	//		}
	//		else
	//		{
	//			// まだ見る人がいるので
	//			m_flowPhase = FlowPhase::NEXT;		// 次の人へ回して画面へ
	//			// 表示文字の変更
	//			//m_pLabel->setString("周りに人がいないことを確認してから画面をタッチしてください。");
	//			auto board = this->getChildByName("BigBoard");
	//			auto top = board->getChildByName<Label*>("TopLabel");
	//			auto middle = board->getChildByName<Label*>("MiddleLabel");
	//			auto bottom = board->getChildByName<Label*>("BottomLabel");
	//			top->setString("周りに人がいないことを確認してから");
	//			middle->setString("");
	//			bottom->setString("画面をタッチしてください");
	//		}
	//	}
	//		return false;
	//		break;
	//	case EseMainLayer::FlowPhase::PLAY:
	//		// お絵かき中
	//	{				
	//		// メインフェーズ
	//		switch (m_phase)
	//		{
	//		case EseMainLayer::Phase::ANNOUNCE:
	//			return false;
	//			break;
	//		case EseMainLayer::Phase::DRAWING:
	//		{
	//			auto canvas = getChildByName<LayerCanvas*>("Canvas")->getSprite();
	//			auto target = event->getCurrentTarget();
	//			//Point locationInNode = target->convertToWorldSpace(touch->getLocation());
	//			Vec2 locationInNode = canvas->convertToWorldSpace(touch->getLocation());
	//			//*
	//			//Size s = target->getContentSize();
	//			//Rect rect = Rect(0, 0, s.width, s.height);
	//			Size s = canvas->getContentSize();
	//			s.width *= canvas->getScaleX();
	//			s.height *= canvas->getScaleY();
	//			//Rect rect(canvas->getPositionX(),
	//			//	canvas->getPositionY() - canvas->getAnchorPoint().y*s.height,
	//			//	s.width, s.height);
	//			Rect rect(canvas->getPositionX() - canvas->getAnchorPoint().x*s.width,
	//				canvas->getPositionY() - canvas->getAnchorPoint().y*s.height,
	//				s.width, s.height);
	//			// キャンバス内なら	
	//			if (rect.containsPoint(locationInNode))
	//				//*/
	//			{
	//				// 誤タッチ防ぎ用(指が離されてからの時間が一定以上)
	//				if (!m_touchPoints.empty() && TOUCH_APPLY_DELAY >= m_nonTouchTime)
	//				{
	//					// 誤タッチと認識。描いてる人をひとり前に
	//					--m_nowDrawer;
	//				}
	//				//log("nowD:%u size:%u", m_nowDrawer, m_touchPoints.size());
	//				size_t i = m_touchPoints.size();
	//				for (; i <= m_nowDrawer; ++i)
	//				{
	//					m_touchPoints.push_back(std::vector<Vec2>());
	//				}
	//				m_touchPoints[m_nowDrawer].push_back(locationInNode);
	//			}
	//			else
	//			{
	//				return false;
	//				break;
	//			}
	//		}
	//			break;
	//		case EseMainLayer::Phase::CONFIRM:
	//			return false;
	//			break;
	//		default:
	//			break;
	//		}
	//		
	//	}
	//		break;
	//	case EseMainLayer::FlowPhase::VOTE:
	//		// 画伯投票
	//	{
	//	}
	//		break;
	//	case EseMainLayer::FlowPhase::JUDGE:
	//		// 画伯発表と正誤発表
	//	{
	//	}
	//		break;
	//	case EseMainLayer::FlowPhase::REPEAT:
	//		// 再プレイ？
	//	{
	//	}
	//		break;
	//	default:
	//		break;
	//	}
	//	return true;
	//};

	multiListener1->onTouchesBegan = ([&](const std::vector<Touch*>& touches, Event* event)->void
	{
		m_touchNum++;
		log("Began m_touchNum:%d", m_touchNum);
		if (m_isAnimation) return;// false;
		// 最初のタッチのみ
		if (m_touchNum > 1) return;

		auto touch = touches.front();

		switch (m_flowPhase)
		{
		case EseMainLayer::FlowPhase::NEXT:
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
			if (m_nowDrawer == m_esePlayer)
			{
				// 画伯の人
				top->setString("あなたが");
				middle->setString("画伯");
			}
			else
			{
				// 芸術家
				top->setString("小テーマ");
				middle->setString(m_smallTheme);
			}
			if (m_nowDrawer == 0)
			{
				// 最初の人がタッチした時
				// ラベルに大テーマを表示
				std::stringstream ssBigTheme;
				ssBigTheme << "大テーマ[ " << m_bigTheme << " ] ";
				m_pLabel->setString(ssBigTheme.str());
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
		case EseMainLayer::FlowPhase::CHECK:
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
		case EseMainLayer::FlowPhase::PLAY:
			// お絵かき中
		{
			// メインフェーズ
			switch (m_phase)
			{
			case EseMainLayer::Phase::ANNOUNCE:
				//return false;
				break;
			case EseMainLayer::Phase::DRAWING:
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
				//Rect rect(canvas->getPositionX(),
				//	canvas->getPositionY() - canvas->getAnchorPoint().y*s.height,
				//	s.width, s.height);
				//Rect rect(canvas->getPositionX() - canvas->getAnchorPoint().x*s.width,
				//	canvas->getPositionY() - canvas->getAnchorPoint().y*s.height,
				//	s.width, s.height);
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
						--m_nowDrawer;
						this->_UpdateDrawerDisp();
					}

					//log("nowD:%u size:%u", m_nowDrawer, m_touchPoints.size());

					size_t i = m_touchPoints.size();
					for (; i <= m_nowDrawer; ++i)
					{
						m_touchPoints.push_back(std::vector<Vec2>());
					}
					m_touchPoints[m_nowDrawer].push_back(locationInNode);
					
					m_isDrew = true;
				}
				else
				{
				//	return false;
					m_isDrew = false;
					break;
				}
			}
				break;
			case EseMainLayer::Phase::CONFIRM:
				//return false;
				break;
			default:
				break;
			}


		}
			break;
		case EseMainLayer::FlowPhase::VOTE:
			// 画伯投票
		{

		}
			break;
		case EseMainLayer::FlowPhase::JUDGE:
			// 画伯発表と正誤発表
		{

		}
			break;
		case EseMainLayer::FlowPhase::REPEAT:
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
	//listener1->onTouchMoved = [&](Touch* touch, Event* event)
	//{
	//	log("Move m_touchNum:%d", m_touchNum);
	//	// おっきなふぇーず
	//	if (m_flowPhase != FlowPhase::PLAY)
	//		return;
	//	// メインフェーズ
	//	if (m_phase != Phase::DRAWING)
	//		return;

	//	auto target = event->getCurrentTarget();
	//	Point locationInNode = target->convertToNodeSpace(touch->getLocation());




	//	auto canvas = getChildByName("Canvas");
	//	Size s = canvas->getContentSize();
	//	s.width *= canvas->getScaleX();
	//	s.height *= canvas->getScaleY();
	//	//Rect rect(canvas->getPositionX(),
	//	Rect rect(canvas->getPositionX() - canvas->getAnchorPoint().x*s.width,
	//		canvas->getPositionY() - canvas->getAnchorPoint().y*s.height,
	//		s.width, s.height);
	//	if (rect.containsPoint(locationInNode))
	//	{
	//		if (m_nowDrawer < m_touchPoints.size())
	//		{
	//			m_touchPoints[m_nowDrawer].push_back(locationInNode);
	//		}
	//	}

	//};
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

	// タッチイベントが終了した場合
	//listener1->onTouchEnded = [&](Touch* touch, Event* event)
	//{
	//	// おっきなふぇーず
	//	if (m_flowPhase != FlowPhase::PLAY)
	//		return;
	//	// メインフェーズ
	//	if (m_phase != Phase::DRAWING)
	//		return;

	//	// タッチしてない時間初期化
	//	m_nonTouchTime = 0.f;

	//	// 次描く人へ
	//	++m_nowDrawer;
	//};
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

		// タッチしてない時間初期化
		m_nonTouchTime = 0.f;

		// 次描く人へ
		++m_nowDrawer;
		this->_UpdateDrawerDisp();
	};

	//auto children = this->getChildren();
	//const char* names[] = {
	//	"Close", 
	//	"Canvas", 
	//	"NextButton", 
	//	"GM",
	//	"Gasho", 
	//	"ArrowLeft",
	//	"ArrowRight",
	//	"Decide",
	//};
	//
	//
	//
	//for (auto& name : names)
	//{
	//	auto listener = listener1->clone();
	//	auto child = getChildByName(name);
	//	if (child)
	//	{
	//		dispatcher->addEventListenerWithSceneGraphPriority(listener, child);
	//	}
	//}

	//for (auto& child : children)
	//{
	//	auto listener = listener1->clone();
	//	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, child);
	//	//dispatcher->addEventListenerWithFixedPriority(listener, 30);
	//}
	//_eventDispatcher->addEventListenerWithSceneGraphPriority(listener1, this);
	
	//dispatcher->addEventListenerWithSceneGraphPriority(listener1, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(multiListener1, this);



	//_eventDispatcher->addEventListenerWithFixedPriority(listener1, 1);
}


/**
	@brief		矢印の上下移動
	@param[in]	delta	フレームタイム
	@return		なし
*/
void EseMainLayer::_MoveArrow(float delta)
{
	return;
	m_time += delta;
	// 矢印上下移動
	auto arrow = this->getChildByName("Arrow");
	Vec2 pos = m_arrowPos;
	pos.y += std::sinf(m_time*5.f) * 20.f;
	arrow->setPosition(pos);
}



/**
	@brief		お絵かき開始
	@param[in]	なし
	@return		なし
*/
void EseMainLayer::_StartDrawing()
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
		// 戻るボタン表示
		this->getChildByName("MenuButtons")->setVisible(true);

		// 描く人情報更新
		m_pTurnLabel->setVisible(true);
		this->_UpdateDrawerDisp();

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
	m_nowDrawer = 0;				// 描く人
	m_time = 0.f;					// アニメーション用時間
	m_nonTouchTime = 0.f;			// タッチしていない時間初期化
}


// 次ボタン
void EseMainLayer::_ButtonCallback(cocos2d::Ref* pSender)
{
	switch (m_flowPhase)
	{
	//case EseMainLayer::FlowPhase::NEXT:
	//	break;
	//case EseMainLayer::FlowPhase::CHECK:
	//	break;
	//case EseMainLayer::FlowPhase::PLAY:
	//	break;
	case EseMainLayer::FlowPhase::CONFIRM:
		this->_StartVote();
		break;
	case EseMainLayer::FlowPhase::VOTE:
		// 開票
		this->_StartCounting();
		break;
	case EseMainLayer::FlowPhase::COUNTING:
		// 開票終了
		this->_StartedCounting();
		break;
	case EseMainLayer::FlowPhase::JUDGE:
		this->_StartShowCanvas();
		break;
	case EseMainLayer::FlowPhase::SHOW_CANVAS:
		this->_StartOpenTheme();
		break;
	case EseMainLayer::FlowPhase::OPEN_THEME:
		dynamic_cast<SceneAtelierMain*>(this->getParent()->getParent())->toTitle();
		//restart(this->getName());
		break;
	case EseMainLayer::FlowPhase::REPEAT:
		break;
	case EseMainLayer::FlowPhase::END:
		break;
	default:
		break;
	}
	
//	if (FlowPhase::VOTE == m_flowPhase)
//	{
//		// 投票完了
//
//		// フェーズ移行 画伯発表へ
//		m_flowPhase = FlowPhase::JUDGE;
//		// ボタンラベルの変更
//		m_pButtonLabel->setString("小テーマ表示");
//
//
//		// ラベル設定
//		m_pLabel->setString("画伯は");
//		m_pLabel->setPosition(m_pBigThemeLabel->getPosition());
//
//		m_pBigThemeLabel->setColor(Color3B(m_colors[m_esePlayer]));	// 画伯のペン色
//		std::stringstream ss;
//		ss << "Player" << (m_esePlayer + 1);		// 画伯名
//		m_pBigThemeLabel->setString(ss.str());
//		Vec2 offset(0.f, -30.f);	// 小テーマラベルからの距離
//		m_pBigThemeLabel->setPosition(m_pLabel->getPosition() + offset);
//
//	}
//	else if (FlowPhase::JUDGE == m_flowPhase)
//	{
//		// 画伯発表後
//
//		// 再プレイ(αではタイトルへもどる)選択へ
//		m_flowPhase = FlowPhase::REPEAT;
//
//
//		// ボタンラベルの変更
//		m_pButtonLabel->setString("タイトルへ");
//
//		// ラベル設定
//		std::stringstream ss;
//		ss << "大テーマ : " << m_bigTheme;
//		m_pBigThemeLabel->disableEffect();
//		m_pBigThemeLabel->setSystemFontSize(24.f*MultiResolution::getFontScale());
//#ifdef _WINDOWS
//		m_pBigThemeLabel->setColor(Color3B(5, 5, 5));	// 元のラベルカラー
//#else
//		m_pBigThemeLabel->setTextColor(Color4B(5, 5, 5, 255));	// 元のラベルカラー
//#endif
//		m_pBigThemeLabel->setString(ss.str());
//		m_pBigThemeLabel->setPosition(m_pLabel->getPosition());
//		m_pBigThemeLabel->enableOutline(Color4B::WHITE, 3);	// 黒枠線
//		ss.str("");
//		ss << "小テーマ : " << m_smallTheme;
//		Vec2 offset(0.f, -30.f);	// 大テーマラベルからの距離
//		m_pLabel->setString(ss.str());
//		m_pLabel->setPosition(m_pBigThemeLabel->getPosition() + offset);
//
//	}
//	//*
//	else if (FlowPhase::REPEAT == m_flowPhase)
//	{
//		// 再プレイする
//		if (!m_isAnimation)
//		{
//			m_isAnimation = true;
//
//			//auto canvas = this->getChildByName("Canvas");
//			//auto pos = canvas->getPosition();
//			//auto size = canvas->getContentSize();
//			//
//			//auto endFunc = CallFunc::create([&](){
//			//	auto curtain = this->getChildByName("CurtainLayer");
//			//	curtain->runAction(Sequence::create(
//			//		Show::create(),
//			//		ScaleTo::create(0.5f, 1.f),
//			//		nullptr));
//			//
//			//	dynamic_cast<SceneAtelierMain*>(this->getParent()->getParent())->changeLayer(this->getName(), "RepeatLayer");
//			//});
//			//
//			//Vec2 to(
//			//	pos.x,
//			//	pos.y - size.height*0.2f
//			//	);
//			//auto action = Spawn::create(
//			//	ScaleTo::create(0.5f, 0.75f),
//			//	MoveTo::create(0.5f, to),
//			//	endFunc,
//			//	nullptr
//			//	);
//			//
//			//canvas->runAction(action);
//
//			dynamic_cast<SceneAtelierMain*>(this->getParent()->getParent())->toTitle();
//		}
//	}
////*/
}



/**
	@brief		ボタンタッチ
	@param[in]	pSender		ボタン
	@param[in]	eventType	イベントタイプ
	@return		なし
*/
void EseMainLayer::_ControlButtonPushed(Ref* pSender, Control::EventType eventType)
{
	auto button = dynamic_cast<ControlButton*>(pSender);
	if (!button || m_isAnimation)
		return;

	std::string name = button->getName();
	if ("Option" == name)
	{
		// オプションレイヤーへ
		m_isAnimation = true;
		dynamic_cast<SceneAtelierMain*>(this->getParent()->getParent())->changeLayer(this->getName(), "OptionLayer");

		//auto canvas = this->getChildByName<Sprite*>("Canvas");
		//canvas->runAction(ScaleTo::create(2.f, 0.75f));
	}
}


/**
	@brief		描画者情報更新
	@param[in]	なし
	@return		なし
*/
void EseMainLayer::_UpdateDrawerDisp()
{
	// ターン数
	std::stringstream ss;
	ss << MIN(m_nowDrawer / m_playerNum + 1, 2) << " / 2";
	m_pTurnLabel->setString(ss.str());

	// 描画者
	m_pDrawNodeDrawer->clear();
	if (m_nowDrawer / m_playerNum >= 2)
	{
		return;
	}
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
	@brief		小テーマ確認レイヤーの表示
	@param[in]	なし
	@return		なし
*/
void EseMainLayer::_ShowSmallTheme()
{
	if (FlowPhase::PLAY != m_flowPhase) return;
	auto winSize = Director::getInstance()->getWinSize();
	auto layer = Layer::create();
	if (!layer)
		return;
	auto sprite = Sprite::create();	// 背景用
	layer->addChild(sprite, 0);
	sprite->setTextureRect(Rect(0,0,winSize.width,winSize.height));
	sprite->setPosition(winSize / 2.f);
	sprite->setColor(Color3B::BLACK);
	sprite->setOpacity(128);
	auto board = this->getChildByName<Sprite*>("BigBoard");
	if (!board)
	{
		layer->release();
		return;
	}
	
	// ボード表示
	board->setVisible(true);
	
	// タッチスクリーン用レイヤー
	// z: ボード > レイヤー > キャンバス(その他)
	this->addChild(layer, 10000/*this->getChildByName("Canvas")->getLocalZOrder() + 1*/, "");
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
	if (_CheckEse(m_nowDrawer))
	{
		// 画伯の人
		top->setString("あなたが");
		middle->setString("画伯");
	}
	else
	{
		// 芸術家
		top->setString("小テーマ");
		middle->setString(m_smallTheme);
	}
	bottom->setString("確認したら画面をタッチ");
	
}


/**
	@brief		投票開始
	@param[in]	なし
	@return		なし
*/
void EseMainLayer::_StartVote()
{
	m_flowPhase = FlowPhase::VOTE;

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	// プレイヤー人数分に拡張、初期化
	m_pollNums.assign(m_playerNum, 0);

	m_nowVoter = 0;	// 投票者リセット

	// 戻るボタン非表示
	this->getChildByName("MenuButtons")->getChildByName("BackButton")->setVisible(false);
	// 進むボタン
	m_pNextButton->setVisible(false);
	m_pButtonLabel->setVisible(false);

	// パーティクル
	this->stopActionByTag(Tag::PARTICLE_ACTION);
	this->removeChildByName("ParticleBatch_Stars");
	// 描く人表示
	this->removeChildByName("DrawNodeDrawer");

	// 小テーマ確認ボタン使用不可 全員書き終わった時にしてる
	//auto themeButton = this->getChildByName("MenuButtons")->getChildByName<MenuItemImage*>("CheckThemeButton");
	//themeButton->setEnabled(false);
	//themeButton->setVisible(false);

	// 下ボード
	auto smallBoard = this->getChildByName("SmallBoard2");
	// 投票ボタン出現
	auto menuVoteButtons = this->getChildByName<Menu*>("MenuVoteButtons");
	const float duration = 1.0f;
	menuVoteButtons->setPositionX(visibleSize.width + menuVoteButtons->getContentSize().width);
	menuVoteButtons->runAction(
		Sequence::create(
			Show::create(),
			Spawn::create(
				TargetedAction::create(	// ボードを移動
					smallBoard,
					EaseBackOut::create(
						MoveTo::create(duration,
							Vec2(visibleSize.width / 2.f,
							smallBoard->getPositionY()
							)	// 画面中央まで
						)
					)
				),
				EaseBackOut::create(
					MoveTo::create(duration,
						Vec2(visibleSize.width / 2.f,
							menuVoteButtons->getPositionY()
						)
					)
				),
				nullptr
			),
			nullptr
		)
	);

	//---------------------------------------
	// ラベル設定
	std::stringstream ss;
	ss << m_nowVoter + 1 << "P";
	m_pLabel->setString(ss.str());
	m_pLabel->setColor(Color3B(m_colors[m_nowVoter]));
	auto label = Label::createWithSystemFont("さん 画伯だと思う人に投票してください", m_pLabel->getSystemFontName(), m_pLabel->getSystemFontSize());

	// 座標合わせ
	{
		auto tmp = Label::createWithSystemFont(ss.str() + label->getString(), m_pLabel->getSystemFontName(), m_pLabel->getSystemFontSize());
		//tmp->autorelease();
		auto size = tmp->getContentSize();
		
		m_pLabel->setPositionX(m_pLabel->getPositionX() - size.width / 2.f);

	}
	auto size = m_pLabel->getContentSize();
	label->setAnchorPoint(Vec2(0.0f, 0.5f));
	label->setAlignment(TextHAlignment::CENTER, m_pLabel->getVerticalAlignment());
	label->setPosition(
		m_pLabel->getPosition() +
		Vec2(size.width / 2.f,
			0.0f));
	m_pLabel->getParent()->addChild(label, m_pLabel->getLocalZOrder(), "SubLabel");

	
}

/**
	@brief		票数開示開始
	@param[in]	なし
	@return		なし
*/
void EseMainLayer::_StartCounting()
{
	Size visibleSize = Director::getInstance()->getVisibleSize();

	m_flowPhase = FlowPhase::COUNTING;


	auto children = this->getChildByName("MenuVoteButtons")->getChildren();
	//for (auto& button : children)
	for (size_t i = 0; i < m_playerNum; ++i)
	{
		const auto button = dynamic_cast<MenuItemSprite*>(children.at(i));
		auto tag = button->getTag();
		auto label = button->getChildByName<Label*>("Label");
		// ラベルに得票数を表示
		label->setString(std::to_string(m_pollNums[tag]));
		button->setEnabled(true);
	}
	m_pLabel->setString("得票数");
	m_pButtonLabel->setString("画伯発表");
	//this->runAction(
	//	Sequence::create(
	//		DelayTime::create(1.5f),
	//		CallFunc::create(std::bind(std::mem_fn(&EseMainLayer::_StartedCounting), this)),
	//		nullptr
	//	)
	//);
}

/**
	@brief		開票後処理
	@param[in]	なし
	@return		なし
*/
void  EseMainLayer::_StartedCounting()
{
	//-----------------------------------------
	// 最多得票者の算出
	// 最多得票者番号(初期値0番目のプレイヤー)
	std::vector<int> maxers = { 0 };
	maxers.reserve(m_playerNum);
	for (size_t i = 1; i < m_playerNum; ++i)
	{
		// 現在の最高得点
		const auto nowMax = m_pollNums[maxers.front()];
		if (nowMax < m_pollNums[i])
		{
			// 最多得票者更新
			maxers.assign(1, i);
			//maxers.clear(); maxers.push_back(i);
		}
		else if (nowMax == m_pollNums[i])
		{
			// 複数人
			maxers.push_back(i);
		}
	}
	//-------------------------------------------
	if (maxers.size() == 1)
	{
		// 一人だけ
		auto player = maxers.front();
		// 画商選択時
		//if (player == m_gashoPlayer)
		//{
		//	// プレイヤー勝利
		//	// _StartGasho();
		//}
		//else
		{
			_StartJudge(player);	// 画伯 or 芸術家指名
		}
	}
	else
	{
		// 二人以上
		// 画伯勝利
		_StartEseWinProc();
		
	}
}

/**
	@brief		画伯吊り上げ開始
	@param[in]	maxPlayer		最多得票プレイヤー番号
	@return		なし
*/
void EseMainLayer::_StartJudge(const int maxPlayer)
{
	m_flowPhase = FlowPhase::JUDGE;

	m_gameState = (maxPlayer == m_esePlayer) ? GameState::MATCH : GameState::LOSE;

	// ボタン非表示
	m_pNextButton->setVisible(false);
	m_pButtonLabel->setVisible(false);

	auto voteButtons = this->getChildByName("MenuVoteButtons")->getChildren();
	const auto button = voteButtons.at(maxPlayer);

	Size visibleSize = Director::getInstance()->getVisibleSize();

	// 下ボード
	auto smallBoard = this->getChildByName("SmallBoard2");
	m_pLabel->setString("画伯は");

	// ボタンラベル
	//auto label = button->getChildByName<Label*>("Label");
	//std::stringstream ss;
	//ss << maxPlayer + 1 << "P";
	//label->setString(ss.str());


	auto buttonMovedFunc = CallFunc::create([&, visibleSize, button](){
		// 各種ボタンを8角形に配置
		const float radius = MIN(visibleSize.width, visibleSize.height) / 2.f - button->getContentSize().width*2.f;

		float degree = 90.f;

		const auto parentPos = button->getParent()->getPosition();

		auto children = button->getParent()->getChildren();
		
		const float intervalDeg = 360.f / (children.size() - 1);
		const Vec2 center = visibleSize / 2.f;
		
		const float moveTime = 0.2f;	// 移動時間

		int i = 0;
		for (auto child : children)
		{
			if (child == button) continue;

			float rad = CC_DEGREES_TO_RADIANS(degree);

			Vec2 pos(center + (Vec2::forAngle(rad)*radius));

			child->runAction(
				Sequence::create(
					DelayTime::create(moveTime * i),
					MoveTo::create(moveTime, Vec2(
						pos - parentPos
					)),
					nullptr
					)
				);


			degree -= intervalDeg;
			if (degree <= 0.f)
				degree += 360.f;
			++i;
		}

	});


	//auto 

	//
	auto endFunc = CallFunc::create([&, visibleSize](){
		// 溜め後
		// 画伯発表ラベル更新
		std::stringstream ss;
		ss << m_esePlayer + 1 << "P";
		m_pLabel->setString(ss.str());
		m_pLabel->setColor(Color3B(m_colors[m_esePlayer]));
		// ラベル変更
		m_pButtonLabel->setString("キャンバス確認");


		
		// 最多得票者が画伯の場合
		if (GameState::MATCH == m_gameState)
		{

			// パーティクル発生
			// 加算合成なので見やすくなるように少し暗く
			auto layer = Layer::create();
			if (!layer)
				return;
			auto sprite = Sprite::create();	// 背景用
			layer->addChild(sprite, 0);
			sprite->setTextureRect(Rect(0, 0, visibleSize.width, visibleSize.height));
			sprite->setPosition(visibleSize / 2.f);
			sprite->setColor(Color3B::BLACK);
			sprite->setOpacity(64);
			

			auto particleBatch = ParticleBatchNode::create("particle_texture.png");
			layer->addChild(particleBatch, 0, "ParticleBatch_Texture");
			
			this->addChild(layer, ZOrder::PARTICLE, "ParticleLayer");
		}
		else
		{
			//-------------------------------------
			// ボタン表示
			m_pNextButton->setVisible(true);
			m_pButtonLabel->setVisible(true);

		}
	});

	// パーティクル定期発生
	auto paticleFunc = CallFunc::create([&, visibleSize](){
	
		auto layer = this->getChildByName<Layer*>("ParticleLayer");

		auto particleFw = common::createParticle("particle_fireworks_texture.plist");

		auto size = particleFw->getContentSize();

		// 座標決め
		float posX = ns_Random::Random::getInstance()->getRandom<float>(size.width / 2.f, visibleSize.width - size.width / 2.f);
		float posY = ns_Random::Random::getInstance()->getRandom<float>(size.height / 2.f, visibleSize.height - size.height / 2.f);
		particleFw->setPosition(posX, posY);
		
		
		layer->getChildByName("ParticleBatch_Texture")->addChild(particleFw, 0, "Fireworks");

	});

	// パーティクル発生レイヤーを濃くして回転"しながら"小さくなる -> 後消滅
	auto particleEndFunc = CallFunc::create([&](){	
		const float _duration = 1.5f;
		auto layer = this->getChildByName("ParticleLayer");
		// パーティクル停止
		auto children = layer->getChildByName("ParticleBatch_Texture")->getChildren();
		for (auto child : children)
		{
			dynamic_cast<ParticleSystemQuad*>(child)->stopSystem();
		}
		//-------------------------------------
		// ボタン表示
		m_pNextButton->setVisible(true);
		m_pButtonLabel->setVisible(true);

		layer->runAction(
			Sequence::create(
				Spawn::create(
					FadeTo::create(0.1f, 192),
					ScaleBy::create(_duration, 0.001f),
					RotateBy::create(_duration, 360.f * 2.f),
					nullptr
				),
				RemoveSelf::create(),
				//CallFunc::create(std::bind(std::mem_fn(&EseMainLayer::_StartShowCanvas), this)),
				nullptr
			)
		);
	});


	const float moveTime = 1.f;		// 最多得票者が真ん中に移動する時間
	// 最多得票ボタンを拡大しつつ画面中央へ
	// 同時に投票ボタンを隠すようにボード出現(画伯発表用)
	// のち、溜め(上ラベルに"・"を追加*3)
	// のち、画伯発表
	button->runAction(
		Sequence::create(
			Spawn::create(
				ScaleTo::create(0.5f, 1.5f),	// 拡大
				MoveTo::create(1.f, -button->getParent()->getPosition() + visibleSize / 2.f),	// 移動
				//TargetedAction::create(	// ボードを移動
				//	smallBoard,			
				//	EaseExponentialOut::create(	// 徐々に減速
				//		MoveTo::create(	0.75f,	
				//			Vec2(	visibleSize.width/2.f,
				//					smallBoard->getPositionY()
				//			)	// 画面中央まで
				//		)
				//	)
				//),
				nullptr	// 同時処理終了
			),
			buttonMovedFunc,
			// 溜め
			Repeat::create(
				Sequence::create(
					DelayTime::create(1.f),
					CallFunc::create([&](){
						std::string str = m_pLabel->getString();
						str += "・";
						m_pLabel->setString(str);
					}),
					nullptr
				), 3	// 3回分
			),
			DelayTime::create(1.f),
			endFunc,	// 画伯発表等
			// 最多得票者が画伯だった場合、演出
			(GameState::MATCH == m_gameState) ?
				Sequence::create(
					Repeat::create(
						Sequence::create(
							DelayTime::create(0.75f),
							paticleFunc,			// パーティクル発生
							nullptr
						),5
					),
					DelayTime::create(1.5f),	// 数秒後
					particleEndFunc,
					//------------------
					nullptr)
				: nullptr,
			nullptr		// シーケンス処理終了
		)	// アクション終了
	);

}


/**
	@brief		キャンバス見やすく表示
	@param[in]	なし
	@return		なし
*/
void EseMainLayer::_StartShowCanvas()
{
	auto visibleSize = Director::getInstance()->getVisibleSize();

	// フェーズ移行
	m_flowPhase = FlowPhase::SHOW_CANVAS;
	//--------------------------------------
	// ボタンラベル
	m_pButtonLabel->setString("小テーマ発表");
	//--------------------------------------
	// 上ラベル
	{
		std::stringstream ss;
		ss << "大テーマ[" << m_bigTheme << "]";
		m_pLabel->setString(ss.str());
		m_pLabel->setColor(Color3B(250, 250, 250));	// 白文字
		m_pLabel->enableOutline(Color4B::BLACK, 3);	// 黒枠線
	}
	//--------------------------------------
	// 下ラベル
	auto label = this->getChildByName("SmallBoard2")->getChildByName<Label*>("Label");
	std::stringstream ss;
	if (GameState::NOTMATCH == m_gameState)
	{
		ss << "票が別れたため";
	}
	ss << "画伯( " << m_esePlayer + 1 << "P )";

	if (GameState::MATCH == m_gameState)
	{
		ss << "は小テーマを答えてください";
	}
	else
	{
		// 画伯の勝利
		ss << "の勝利です";
	}
	
	label->setString(ss.str());

	//---------------------------------------
	// ボタン落下
	auto voteButtons = this->getChildByName("MenuVoteButtons")->getChildren();


	for (auto button : voteButtons)
	{
		auto rand = ns_Random::Random::getInstance();

		const float gravity = 1.f;	// 重力加速度
		// 角度
		const float startRadian = rand->getRandom(0.f, static_cast<float>(M_PI*2.0));
		// 初速
		const float force = rand->getRandom(10.f, 100.f);

		// 初期移動
		auto startMove = MoveBy::create(0.5f, Vec2::forAngle(startRadian)*force);

		// 重力処理
		auto applyGravity = [button, gravity](){
			button->setPositionY(button->getPositionY() - gravity);
		};

		button->runAction(
			Sequence::create(
				// 落下
				Spawn::create(
					startMove,
					EaseCubicActionIn::create(MoveBy::create(1.f, Vec2(cosf(startRadian)*force*0.2f, -visibleSize.height-force))),
					nullptr
				),
				// 画伯ボタンのみ最後に表示
				(button == voteButtons.at(m_esePlayer)) ?
					Sequence::create(
						// ボタンのラベル類の操作
						CallFunc::create([&, button](){
							std::stringstream ss;
							ss << m_esePlayer + 1 << "P";
							button->getChildByName<Label*>("Label")->setString(ss.str());
							button->setScale(1.f);
						}),
						MoveTo::create(1.f, -button->getParent()->getPosition() + button->getContentSize()),
						nullptr
					)
					: nullptr,
				nullptr
			)
		);
	}
}


/**
	@brief		画伯の勝利確定時の処理開始
	@param[in]	なし
	@return		なし
*/
void EseMainLayer::_StartEseWinProc()
{
	// 負け
	m_gameState = GameState::NOTMATCH;
	this->_StartShowCanvas();
}

/**
	@brief		小テーマ発表
	@param[in]	なし
	@return		なし
*/
void EseMainLayer::_StartOpenTheme()
{
	// フェーズ移行
	m_flowPhase = FlowPhase::OPEN_THEME;


	//--------------------------------------
	// ボタンラベル
	m_pButtonLabel->setString("タイトルへ");


	//--------------------------------------
	// 下ラベル
	auto label = this->getChildByName("SmallBoard2")->getChildByName<Label*>("Label");
	std::stringstream ss;
	ss << "小テーマ[" << m_smallTheme << "]";
	label->setString(ss.str());



}


#pragma endregion EseMainLayer methods

