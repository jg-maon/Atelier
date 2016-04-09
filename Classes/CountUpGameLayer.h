/**=============================================================================
	@file		CountUpGameLayer.h
	@brief		タイムアタックモードレイヤークラス 定義
	@author		Masato Yamamoto
	@date		2015-02-01

	@par		[説明]
				タイムアタックモード用レイヤークラス
				テーマ当てるまでの時間を競う

	@note

		Copyright (C) 2015  saitsuyo
=============================================================================*/

#ifndef __COUNTUP_GAME_LAYER_H__
#define __COUNTUP_GAME_LAYER_H__

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
	@class		CountUpGameLayer
	@brief		タイムアタックモードレイヤークラス
*/
class CountUpGameLayer : public GridLayer, public cocos2d::extension::EditBoxDelegate
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
		//CONFIRM,		//!< 描き終わり確認(最後の人が戻るかどうか)
		//VOTE,			//!< 画伯投票
		//COUNTING,		//!< 票数開示
		JUDGE,			//!< テーマが合っているか
		//SHOW_CANVAS,	//!< キャンバスを見やすく表示

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
		BIGBOARD = 2,			//!< テーマ表示板
		CANVAS = 3,				//!< キャンバス
		TOPBOARD = 5,			//!< 上板

		TURN_LABEL,				//!< 描く人ラベル
		DRAWER_NODE,			//!< 描く人ノード
		CANT_DRAWER,			//!< 描けない人×

		BOX = 100,				//!< 解答入力ボックス
		MENUBUTTONS,			//!< 戻る進むテーマ確認ボタン
		NEXTBUTTONLABEL
		= MENUBUTTONS + 1,	//!< 進むボタンラベル
		PARTICLE = 200,			//!< パーティクル

		CURTAIN = 1000,			//!< カーテン

		MOST_FRONT = 99999,		//!< 最前面

	};

	/**
		@brief	タグ
	*/
	enum Tag
	{
		PARTICLE_ACTION = 100,		//!< 現在描いている人表示パーティクル
	};

	static const float TOUCH_APPLY_DELAY;			//!< 誤タッチ認識間隔
	static const float ONE_DRAW_TIME;				//!< タッチ時間
	//=====================================================
	// 内部クラス宣言
	//=====================================================
public:
	struct Theme
	{
		std::string theme;		//!< テーマ
		std::string kana;		//!< よみがな
	};
	//=====================================================
	// 型別名定義
	//=====================================================
public:
	typedef std::unordered_map<std::string, std::vector<Theme>> Theme_Map;


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
	~CountUpGameLayer();


	// Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
	virtual bool init();



	// implement the "static create()" method manually
	CREATE_FUNC(CountUpGameLayer);


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
		@brief		お絵かき開始
		@param[in]	なし
		@return		なし
	*/
	void _StartDrawing();


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
		@brief		描画一時終了
		@param[in]	なし
		@return		なし
	*/
	void _StopDrawing();

	/**
		@brief		描画者情報更新
		@param[in]	なし
		@return		なし
	*/
	void _UpdateDrawerDisp();

	/**
		@brief		累計時間ラベル更新
		@param[in]	なし
		@return		なし
	*/
	void _UpdateTimeLabel();

	/**
		@brief		ヒントレイヤーの表示
		@param[in]	なし
		@return		なし
	*/
	void _ShowHint();


	/**
		@brief		"弟子"かチェックする
		@param[in]	no	プレイヤー番号
		@return		弟子か
		@retval		true		"弟子"
		@retval		false		"弟子"でない
	*/
	bool _CheckPupil(unsigned int no)
	{
		return (no % m_playerNum) == m_pupilPlayer;
	}


	/**
		@brief		解答正解アニメーション開始
		@param[in]	なし
		@return		なし
	*/
	void _StartAnsweredAnimation();




	//=====================================================
	// 変数宣言
	//=====================================================
protected:

private:
	bool				m_isAnimation;			//!< アニメーション中フラグ
	FlowPhase			m_flowPhase;			//!< メインフロー
	Phase				m_phase;				//!< メインフェーズ


	unsigned int		m_playerNum;			//!< プレイ人数

	Theme_Map			m_themes;				//!< テーマ(大テーマがキー)
	
	cocos2d::Label*		m_pLabel;				//!< 上ラベル


	int					m_touchNum;				//!< 同時タッチ数
	cocos2d::DrawNode*	m_pDrawNode;			//!< 線描画ノード
	std::vector<cocos2d::Color4F> m_colors;		//!< ペン色
	float				m_brushSize;			//!< ブラシサイズ


	bool				m_isTouching;			//!< タッチしてるよ
	float				m_drawTime;				//!< タッチしている累計時間
	float				m_oneDrawTime;			//!< 1回の描いている時間
	bool				m_isPenalty;			//!< ペナルティ中
	
	unsigned int		m_pupilPlayer;			//!< 弟子番号

	unsigned int		m_nowDrawer;			//!< 今描いている人番号
	cocos2d::DrawNode*	m_pDrawNodeDrawer;		//!< 描く人
	cocos2d::Vec2		m_drawerDispStaPos;		//!< 描く人表示の描画始点
	float				m_drawerDispMarginY;	//!< 描く人表示の余白
	float				m_drawerDispRadius;		//!< 描く人表示の半径
	
		
	std::string			m_hint;					//!< ヒント
	std::string			m_smallTheme;			//!< 小テーマ
	std::string			m_kanaTheme;			//!< かなテーマ



	float				m_nonTouchTime;			//!< タッチしてない時間
	std::vector<std::vector<cocos2d::Vec2>> m_touchPoints;	//!< タッチした場所群[記入者][座標群]

	bool				m_isDrew;				//!< 正しく描いた



	cocos2d::MenuItemImage*	m_pNextButton;		//!< ボタン

	cocos2d::extension::EditBox* m_pEditBox;	//!< 解答入力ボックス

};


#endif // __COUNTUP_GAME_LAYER_H__
