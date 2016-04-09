/**=============================================================================
	@file		RelayGameLayer.h
	@brief		お絵かきリレーモードレイヤークラス 定義
	@author		Masato Yamamoto
	@date		2015-03-12

	@par		[説明]
				お絵かきリレーモード用レイヤークラス
				テーマ当てるまでの時間を競う

	@note

		Copyright (C) 2015  saitsuyo
=============================================================================*/

#ifndef __RELAY_GAME_LAYER_H__
#define __RELAY_GAME_LAYER_H__

#ifndef __COCOS2D_H__
#include "cocos2d.h"
#endif

#ifndef __GRID_LAYER_H__
#include "GridLayer.h"
#endif


#ifndef __COCOS2D_EXT_H__
#ifdef _WINDOWS
#include "cocos-ext.h"
#else
#include "../extensions/cocos-ext.h"
#endif
#endif


/**
	@class		RelayGameLayer
	@brief		お絵かきリレーモードレイヤークラス
*/
class RelayGameLayer : public GridLayer, public cocos2d::extension::EditBoxDelegate
{
	//=====================================================
	// 定数(列挙型)宣言
	//=====================================================
public:
	/**
		@brief	流れ
	*/
	enum class FlowPhase
	{
		CHECK,			//!< 自分の順番の確認
		HIDE,			//!< 隠してタッチしてね
		THEME,			//!< テーマ発表
		START,			//!< スタート待機

		PLAY,			//!< お絵かき

		SHOW,			//!< ひとり前の人の絵を見る

		JUDGE,			//!< テーマが合っているか
		
		ANSWERED,		//!< 解答完了

		REPEAT,			//!< 再プレイアナウンスまでのつなぎ

		END,			//!< ゲームメイン終了
	};
	/**
		@brief	ゲーム中のフェーズ
	*/
	enum class Phase
	{
		ANNOUNCE,		//!< 描く人案内
		DRAWING,		//!< お絵かき！
		CONFIRM,		//!< OK or Cancel
	};


	/**
		@brief	Zオーダー
	*/
	enum ZOrder
	{
		BACK = 0,				//!< 背景
		BIGBOARD = 200,			//!< テーマ表示板
		CANVAS = 300,			//!< キャンバス
		TOPBOARD = 500,			//!< 上板

		
		BOX = 1000,				//!< 解答入力ボックス
		MENUBUTTONS,			//!< 戻る進むテーマ確認ボタン
		NEXTBUTTONLABEL
			= MENUBUTTONS + 1,	//!< 進むボタンラベル

		MENULASTBUTTONS,		//!< 最後一覧

		PARTICLE = 2000,			//!< パーティクル

		CURTAIN = 10000,			//!< カーテン

		MOST_FRONT = 99999,		//!< 最前面

	};

	/**
		@brief	タグ
	*/
	enum Tag
	{
		PARTICLE_ACTION = 100,		//!< 現在描いている人表示パーティクル
	};

	//static const float TOUCH_APPLY_DELAY;			//!< 誤タッチ認識間隔
	static const float BONUS_TIME;					//!< ボーナス時間(max(ボーナス-使用時間,0))
	static const float ONE_DRAW_TIME;				//!< タッチ時間
	//=====================================================
	// 内部クラス宣言
	//=====================================================
public:
	struct Theme
	{
		std::string theme;		//!< テーマ
		std::string kana;		//!< よみがな
		bool operator == (const Theme& theme) const
		{
			return this->theme == theme.theme;
		}
		bool operator != (const Theme& theme) const
		{
			return this->theme != theme.theme;
		}
	};
	//=====================================================
	// 型別名定義
	//=====================================================
public:
	typedef std::vector<cocos2d::Vec2> points;
	typedef std::vector<points> page_t;


	//=====================================================
	// 静的メソッド宣言
	//=====================================================
public:

	//=====================================================
	// メソッド宣言
	//=====================================================
public:

	/**
		@brief		デストラクタ
		@param[in]	なし
		@return		なし
		@par		メモリの解放等
	*/
	~RelayGameLayer();


	// Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
	virtual bool init();



	// implement the "static create()" method manually
	CREATE_FUNC(RelayGameLayer);


	/**
		@brief		更新
		@attention	init内で	this->scheduleUpdate();を呼ぶこと
		@param[in]	delta		1フレームの間隔
		@return		なし
	*/
	void update(float delta);


	/**
		@brief		ゲームメイン開始
		@param[in]	from	元レイヤー
		@return		なし
	*/
	void restart(const std::string& from) override;



	

	// ボックス表示時に呼ばれる
	virtual void editBoxEditingDidBegin(cocos2d::extension::EditBox* editBox) override;
	// OKボタン押したら呼ばれる
	virtual void editBoxTextChanged(cocos2d::extension::EditBox* editBox, const std::string& text) override;
	// ボックス終了時に呼ばれる
	virtual void editBoxEditingDidEnd(cocos2d::extension::EditBox* editBox) override;
	// ボックス終了後に呼ばれる
	virtual void editBoxReturn(cocos2d::extension::EditBox* editBox) override;




protected:



private:

	/**
		@brief		メンバ変数初期化
		@param[in]	なし
		@return		なし
	*/
	void _SetDefaultValues();


	/**
		@brief		タッチリスナーの設定
		@param[in]	なし
		@return		なし
	*/
	void _SetTouchListener();

	/**
		@brief		テーマの表示
		@param[in]	なし
		@return		テーマ表示ボード
	*/
	cocos2d::Sprite* _ShowTheme();

	/**
		@brief		最初の人が戻るを押してテーマを表示
		@param[in]	なし
		@return		なし
	*/
	void _BackShowTheme();



	/**
		@brief		お絵かき開始
		@param[in]	なし
		@return		なし
	*/
	void _StartDrawing();


	/**
		@brief		キャンバス初期化
		@param[in]	なし
		@return		なし
	*/
	void _NextCanvas();


	/**
		@brief		描きすぎレイヤー表示設定
		@param[in]	isVisible	表示するか
		@return		なし
	*/
	void _SetOverTouchLayerVisible(bool isVisible);

		
	/**
		@brief		累計時間ラベル更新
		@param[in]	なし
		@return		なし
	*/
	void _UpdateTimeLabel();


	
	/**
		@brief		解答後アニメーション
		@param[in]	isAnswerd	正解したか
		@return		なし
	*/
	void _StartAnsweredAnimation(bool isAnswerd);


	/**
		@brief		最後一覧表示等メニュー処理
		@param[in]	なし
		@return		なし		
	*/
	void _StartEnd();

	//=====================================================
	// 変数宣言
	//=====================================================
protected:

private:
	bool				m_isAnimation;			//!< アニメーション中フラグ
	FlowPhase			m_flowPhase;			//!< メインフロー
	Phase				m_phase;				//!< メインフェーズ

	std::vector<Theme>	m_themes;				//!< テーマ
	
	Theme				m_nowTheme;				//!< 現在のテーマ

	unsigned int		m_playerNum;			//!< プレイ人数
	unsigned int		m_nowDrawer;			//!< 今描いている人番号
	std::vector<unsigned int>	m_drawOrder;	//!< 描く人順番決定用

	cocos2d::Label*		m_pLabel;				//!< 上ラベル

	cocos2d::MenuItemLabel*		m_pMiddleLabel;			//!< テーマ発表板真ん中ラベル

	int					m_touchNum;				//!< 同時タッチ数
	cocos2d::DrawNode*	m_pDrawNode;			//!< 線描画ノード

	std::vector<cocos2d::Color4F> m_colors;		//!< ペン色
	float				m_brushSize;			//!< ブラシサイズ(半径)

	float				m_oneDrawTime;			//!< 1枚の描いている時間[unit:second]
	bool				m_isCounting;			//!< 時間計測中
	
	float				m_nonTouchTime;			//!< タッチしてない時間
	std::vector<page_t>	m_touchPoints;			//!< タッチした場所群[枚数][本数][座標群]
	unsigned int		m_nowPage;				//!< 現在描いているページ数
	unsigned int		m_nowLine;				//!< 本数
	bool				m_isDrew;				//!< 正しく描いた


	cocos2d::MenuItemImage*	m_pNextButton;		//!< ボタン
	cocos2d::Label*		m_pButtonLabel;			//!< ボタンラベル

	cocos2d::extension::EditBox* m_pEditBox;	//!< 解答入力ボックス


	cocos2d::EventListenerTouchOneByOne* m_pListener;		//!< 描きすぎレイヤーのリスナー

};


#endif // __RELAY_GAME_LAYER_H__
