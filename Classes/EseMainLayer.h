/**=============================================================================
	@file		EseMainLayer.h
	@brief		画伯あぶり出しレイヤークラス 定義
	@author		Masato Yamamoto
	@date		2015-01-31

	@par		[説明]
				画伯あぶり出しメインレイヤー

	@note

		Copyright (C) 2015  saitsuyo
=============================================================================*/

#ifndef __ESEMAIN_LAYER_H__
#define __ESEMAIN_LAYER_H__

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
	@class		EseMainLayer
	@brief		お絵かきメインレイヤークラス
*/
class EseMainLayer : public GridLayer
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
		CONFIRM,		//!< 描き終わり確認(最後の人が戻るかどうか)
		VOTE,			//!< 画伯投票
		COUNTING,		//!< 票数開示
		JUDGE,			//!< 画伯を指したか
		SHOW_CANVAS,	//!< キャンバスを見やすく表示
		OPEN_THEME,		//!< 小テーマ発表
		REPEAT,			//!< 再プレイアナウンスまでのつなぎ

		END,			//!< 画伯メイン終了
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

	enum class GameState
	{
		NONE,			//!< 
		MATCH,			//!< 画伯吊り上げ成功
		NOTMATCH,		//!< 画伯吊り上げ失敗
		LOSE,			//!< 画伯逃げ切り
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

		BOTTOMBOARD = 99,		//!< 下板
		VOTEBUTTONS
		= BOTTOMBOARD + 1,	//!< 投票ボタン
		MENUBUTTONS,			//!< 戻る進むテーマ確認ボタン
		NEXTBUTTONLABEL
		= MENUBUTTONS + 1,	//!< 進むボタンラベル
		PARTICLE
		= BOTTOMBOARD + 1,	//!< パーティクル

		CURTAIN = 1000,			//!< カーテン

	};

	/**
		@brief	タグ
	*/
	enum Tag
	{
		PARTICLE_ACTION = 100,		//!< 現在描いている人表示パーティクル
	};

	static const float	TOUCH_APPLY_DELAY;			//!< 誤タッチ認識間隔
	//=====================================================
	// 内部クラス宣言
	//=====================================================
public:

	//=====================================================
	// 型別名定義
	//=====================================================
public:
	typedef std::unordered_map<std::string, std::vector<std::string>> Theme_Map;


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
	~EseMainLayer();


	// Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
	virtual bool init();



	// implement the "static create()" method manually
	CREATE_FUNC(EseMainLayer);


	/**
		@brief		更新
		@attention	init内で	this->scheduleUpdate();を呼ぶこと
		@param[in]	delta		1フレームの間隔
		@return		なし
	*/
	void update(float delta);


	/**
		@brief		画伯メイン開始
		@param[in]	from	元レイヤー
		@return		なし
	*/
	void restart(const std::string& from) override;






	/**
		@brief		画伯メイン終了か
		@param[in]	なし
		@return		画伯メイン終了か
		@retval		true	画伯メイン終了
		@retval		false	画伯メイン継続
	*/
	bool isEnded() const { return FlowPhase::END == m_flowPhase; }


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
		@brief		矢印の上下移動
		@param[in]	delta	フレームタイム
		@return		なし
	*/
	void _MoveArrow(float delta);



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
	void _ControlButtonPushed(cocos2d::Ref* pSender, cocos2d::extension::Control::EventType eventType);


	/**
		@brief		描画者情報更新
		@param[in]	なし
		@return		なし
	*/
	void _UpdateDrawerDisp();

	/**
		@brief		小テーマ確認レイヤーの表示
		@param[in]	なし
		@return		なし
	*/
	void _ShowSmallTheme();


	/**
		@brief		画伯かチェックする
		@param[in]	no	プレイヤー番号
		@return		画伯か
		@retval		true		画伯
		@retval		false		画伯でない
	*/
	bool _CheckEse(unsigned int no)
	{
		return (no % m_playerNum) == m_esePlayer;
	}


	/**
		@brief		投票開始
		@param[in]	なし
		@return		なし
	*/
	void _StartVote();

	/**
		@brief		票数開示開始
		@param[in]	なし
		@return		なし
	*/
	void _StartCounting();

	/**
		@brief		開票後処理
		@param[in]	なし
		@return		なし
	*/
	void _StartedCounting();

	/**
		@brief		画伯吊り上げ開始
		@param[in]	maxPlayer		最多得票プレイヤー番号
		@return		なし
	*/
	void _StartJudge(const int maxPlayer);




	/**
		@brief		キャンバス見やすく表示
		@param[in]	なし
		@return		なし
	*/
	void _StartShowCanvas();

	/**
		@brief		画伯の勝利確定時の処理開始
		@param[in]	なし
		@return		なし
	*/
	void _StartEseWinProc();


	/**
		@brief		小テーマ発表
		@param[in]	なし
		@return		なし
	*/
	void _StartOpenTheme();



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

	int					m_touchNum;				//!< 同時タッチ数

	cocos2d::DrawNode*	m_pDrawNode;			//!< 線描画ノード
	float				m_brushSize;			//!< ブラシサイズ(半径)
	cocos2d::Label*		m_pLabel;				//!< 表示ラベル

	float				m_time;					//!< update時間
	cocos2d::Vec2		m_arrowPos;				//!< 矢印初期座標


	unsigned int		m_esePlayer;			//!< 画伯番号

	unsigned int		m_nowDrawer;			//!< 今描いている人番号
	cocos2d::DrawNode*	m_pDrawNodeDrawer;		//!< 描く人
	cocos2d::Vec2		m_drawerDispStaPos;		//!< 描く人表示の描画始点
	float				m_drawerDispMarginY;	//!< 描く人表示の余白
	float				m_drawerDispRadius;		//!< 描く人表示の半径
	cocos2d::Label*		m_pTurnLabel;			//!< ターン数表示


	std::string			m_bigTheme;				//!< 大テーマ
	std::string			m_smallTheme;			//!< 小テーマ

	std::vector<cocos2d::Color4F> m_colors;		//!< ペン色


	float				m_nonTouchTime;			//!< タッチしてない時間
	std::vector<std::vector<cocos2d::Vec2>> m_touchPoints;	//!< タッチした場所群[記入者][座標群]

	bool				m_isDrew;				//!< 正しく描いた



	cocos2d::MenuItemImage*	m_pNextButton;		//!< ボタン
	cocos2d::Label*		m_pButtonLabel;			//!< ボタン用ラベル


	// 投票用
	unsigned int		m_nowVoter;				//!< 現在投票中の人

	std::vector<int>	m_pollNums;				//!< 得票数

	GameState			m_gameState;			//!< ゲーム勝敗状況


};


#endif // __ESEMAIN_LAYER_H__
