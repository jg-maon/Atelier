/**=============================================================================
	@file		SceneTitle.h
	@brief		タイトルシーンクラス 定義
	@author		Masato Yamamoto
	@date		2014-12-04
			
	@par		[説明]
				タイトルシーン
				画面タッチでモードセレクトシーンへ遷移

	@note

		Copyright (C) 2015  saitsuyo
=============================================================================*/

// インクルード
#ifndef __SECENE_TITLE_H__
#define __SECENE_TITLE_H__

#ifndef __COCOS2D_H__
#include "cocos2d.h"
#endif



/**
	@class		SceneTitle
	@brief		タイトルシーンクラス
*/
class SceneTitle : public cocos2d::Layer
{
	//=====================================================
	// 定数(列挙型)宣言
	//=====================================================
public:
	enum class FadeState
	{
		FADE_IN,			//!< フェードイン
		FADE_OUT,			//!< フェードアウト
	};
	
	//=====================================================
	// 型別名定義
	//=====================================================
public:

	//=====================================================
	// 静的メソッド宣言
	//=====================================================
public:
	// there's no 'id' in cpp, so we recommend returning the class instance pointer
	static cocos2d::Scene* createScene();

	//=====================================================
	// メソッド宣言
	//=====================================================
public:


	// Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
	virtual bool init();


	// a selector callback
	void menuCloseCallback(cocos2d::Ref* pSender);
	
	// implement the "static create()" method manually
	CREATE_FUNC(SceneTitle);


	/**
		@brief		更新
		@attention	init内で	this->scheduleUpdate();を呼ぶこと
		@param[in]	delta		1フレームの間隔
		@return		なし
	*/
	void update(float delta);



	/**
		@brief		タッチしたイベント
		@param[in]	touch		タッチ情報
		@param[in]	event		イベントを起こしたオブジェクト
		@return		タッチを有効にするか
		@retval		true		タッチを有効とする(onTouchEndedが呼び出される)
		@retval		false		タッチを無効とする(onTouchEndedが呼び出されない)
	*/
	bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event) override;


	
	/**
		@brief		タッチし終わったイベント
		@param[in]	touch		タッチ情報
		@param[in]	event		イベントを起こしたオブジェクト
		@return		なし
	*/
	void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event) override;

protected:



private:

	//=====================================================
	// 変数宣言
	//=====================================================
protected:

private:

	cocos2d::Label*		m_pLabel;			//!< タッチスクリーン

	float				m_labelScale;		//!< ラベル大きさ
	bool				m_isSizeUp;			//!< ラベルを大きく
	float				m_time;				//!< 経過時間

	float				m_fadeTime;			//!< フェードアウト時間
	FadeState			m_fadeState;		//!< フェードアウト状態

};


#endif	// __SECENE_TITLE_H__