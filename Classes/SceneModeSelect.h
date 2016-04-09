/**=============================================================================
	@file		SceneModeSelect.h
	@brief		モードセレクトクラス 定義
	@author		Ken Fujishima	Masato Yamamoto
	@date		2015-03-11
			
	@par		[説明]
				モードセレクト

	@note

		Copyright (C) 2015  saitsuyo
=============================================================================*/

// インクルード
#ifndef __SECENE_MODESELECT_H__
#define __SECENE_MODESELECT_H__


#ifndef __COCOS2D_H__
#include "cocos2d.h"
#endif


#ifndef __COCOS2D_EXT_H__
#ifdef _WINDOWS
#include "cocos-ext.h"
#else
#include "../extensions/cocos-ext.h"
#endif
#endif

/**
	@class		SceneModeSelect
	@brief		モードセレクトシーン
*/
class SceneModeSelect : public cocos2d::Layer, public cocos2d::extension::ScrollViewDelegate
{
	//=====================================================
	// 定数(列挙型)宣言
	//=====================================================
public:
	/**
		@brief	ボタンの種類
	*/
	enum Select
	{
		COUNTUP,		//!< カウントアップゲーム
		COUNTDOWN,		//!< カウントダウンゲーム
		RELAY,			//!< お絵かきリレー
		OPTION,			//!< オプション
		NUM,			//!< シーン数
		ESE,			//!< 画伯

	};
	//=====================================================
	// 内部クラス宣言
	//=====================================================
public:
	//=====================================================
	// 型別名定義
	//=====================================================
public:
	//=====================================================
	// 静的メソッド宣言
	//=====================================================
public:
	static cocos2d::Scene* createScene();
	static cocos2d::Scene* createScene(Select fromScene);
	CREATE_FUNC(SceneModeSelect);
	//=====================================================
	// メソッド宣言
	//=====================================================

public:
	/**
		@brief		デストラクタ
		@param[in]	なし
		@return		なし
	*/
	~SceneModeSelect();

	/**
		@brief		初期化
		@param[in]	なし
		@return		初期化成功したか
		@retval		true	成功
		@rerval		false	失敗
	*/
	virtual bool init();

	
	/**
		@brief		スクロール初期位置
		@attention	Select::NUMは指定しないこと。(動くけどバグる)	
		@param[in]	fromScene	入ってきたシーン
		@return		なし
	*/
	void initPos(Select fromScene);


	void scrollViewDidScroll(cocos2d::extension::ScrollView* s){
		//cocos2d::log("Scroooooooooool");
	}
	void scrollViewDidZoom(cocos2d::extension::ScrollView*){}


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
		@brief		シーン切り替えアニメーションの開始
		@param[in]	currentTarget
		@return		なし
	*/
	void _StartChangeSceneAnimation(cocos2d::Node* currentTarget);

	/**
		@brief		更新
		@param[in]	delta	更新間隔(1秒おき)
		@return		なし
	*/
	void _OneSecondTimer(float delta);


	//=====================================================
	// 変数宣言
	//=====================================================
protected:

private:

	// スプライト
	std::vector<cocos2d::Sprite*>	m_buttonSprites;	//!< ボタン画像

	std::vector<cocos2d::Sprite*>	m_pieSprites;		//!< 時計にかける扇形画像(SecondTimerにて更新)

	cocos2d::Vec2	m_touchBeganPoint;			//!< タッチしはじめ

	bool			m_isSceneChangeCanceled;	//!< シーン移動キャンセル

	bool			m_isSceneChanging;			//!< シーン切り替え中

	// countdownに使用
	int				m_maskCnt;					//!< 黒いマスク画像描画数
	int				m_maskNum;					//!< 黒いマスク画像数
};


#endif // __SCENE_MODESELECT_H__
