/**=============================================================================
	@file		CountDownGameLayer.h
	@brief		タイムアタックモードレイヤークラス 定義
	@author		Masato Yamamoto
	@date		2015-02-09

	@par		[説明]
				タイムアタックモード用レイヤークラス
				テーマ当てるまでの時間を競う

	@note

		Copyright (C) 2015  saitsuyo
=============================================================================*/

#ifndef __COUNTDOWN_GAME_LAYER_H__
#define __COUNTDOWN_GAME_LAYER_H__

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
	@class		CountDownGameLayer
	@brief		タイムアタックモードレイヤークラス
*/
class CountDownGameLayer : public GridLayer, public cocos2d::extension::EditBoxDelegate
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
		NEXT,			//!< 次の人へまわしてね☆
		CHECK,			//!< 自分の役割の確認
		PLAY,			//!< お絵かき
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
	~CountDownGameLayer();


	// Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
	virtual bool init();



	// implement the "static create()" method manually
	CREATE_FUNC(CountDownGameLayer);


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


	/**
		@brief		描ける時間の設定
		@param[in]	t	時間
		@return		なし
	*/
	void setDrawableTime(float t) { m_drawableTime = t; }


	

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
		@brief		テーマ選定
		@param[in]	なし
		@return		なし
	*/
	void _ChoiceTheme();
	

	/**
		@brief		テーマの表示
		@param[in]	なし
		@return		なし
	*/
	void _ShowTheme();


	/**
		@brief		次のテーマ表示へ
		@param[in]	msg	メッセージ
		@return		なし
		@par		テーマ表示のための(一人で見るよう)アナウンス～
					新規キャンバスめくるまで
	*/
	void _NextTheme(const std::string msg);


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



	// a selector callback
	// 次ボタン
	void _ButtonCallback(cocos2d::Ref* pSender);


	/**
		@brief		ボタンタッチ
		@param[in]	pSender		ボタン
		@param[in]	eventType	イベントタイプ
		@return		なし
	*/
	//void _ControlButtonPushed(cocos2d::Ref* pSender, cocos2d::extension::Control::EventType eventType);

	
	/**
		@brief		累計時間ラベル更新
		@param[in]	なし
		@return		なし
	*/
	void _UpdateTimeLabel();


	
	/**
		@brief		解答正解アニメーション開始
		@param[in]	なし
		@return		なし
	*/
	void _StartAnsweredAnimation();


	/**
		@brief		最後終了アニメーション
		@param[in]	なし
		@return		なし
	*/
	void _StartEndAnimation();




	//=====================================================
	// 変数宣言
	//=====================================================
protected:

private:
	bool				m_isAnimation;			//!< アニメーション中フラグ
	FlowPhase			m_flowPhase;			//!< メインフロー
	Phase				m_phase;				//!< メインフェーズ

	std::vector<Theme>	m_themes;				//!< テーマ
	std::vector<Theme>	m_historyThemes;		//!< 出題テーマ一覧

	Theme				m_nowTheme;				//!< 現在のテーマ


	cocos2d::Label*		m_pLabel;				//!< 上ラベル

	int					m_touchNum;				//!< 同時タッチ数
	cocos2d::DrawNode*	m_pDrawNode;			//!< 線描画ノード

	float				m_brushSize;			//!< ブラシサイズ(半径)

	//bool				m_isTouching;			//!< タッチしてるよ
	float				m_drawableTime;			//!< 全体を通して描ける時間[unit:second]
	float				m_oneDrawTime;			//!< 1枚の描いている時間[unit:second]
	bool				m_isPenalty;			//!< ペナルティ中
	bool				m_isCounting;			//!< 時間計測中
	bool				m_isAnswerd;			//!< 正解した

	float				m_nonTouchTime;			//!< タッチしてない時間
	std::vector<page_t>	m_touchPoints;			//!< タッチした場所群[枚数][本数][座標群]
	unsigned int		m_nowPage;				//!< 現在描いているページ数
	unsigned int		m_nowLine;				//!< 本数
	bool				m_isDrew;				//!< 正しく描いた


	cocos2d::MenuItemImage*	m_pNextButton;		//!< ボタン
	cocos2d::Label*		m_pButtonLabel;			//!< ボタンラベル

	cocos2d::extension::EditBox* m_pEditBox;	//!< 解答入力ボックス

};


#endif // __COUNTDOWN_GAME_LAYER_H__
