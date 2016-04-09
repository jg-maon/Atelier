/**=============================================================================
	@file		ScenePreLoad.h
	@brief		ゲーム使用リソースロードシーンクラス 定義
	@author		Masato Yamamoto
	@date		2015-02-10
			
	@par		[説明]
				ゲーム使用リソースロードシーン
				NowLoading...ってやつ
	@note

		Copyright (C) 2015  saitsuyo
=============================================================================*/

#ifndef __SCENE_PRELOAD_H__
#define __SCENE_PRELOAD_H__


#ifndef __COCOS2D_H__
#include "cocos2d.h"
#endif

/**
	@class		ScenePreLoad
	@brief		リソースロードシーン
*/
class ScenePreLoad : public cocos2d::Layer
{
	//=====================================================
	// 定数(列挙型)宣言
	//=====================================================
public:
	/**
		@brief	フェーズ
	*/
	enum class Phase
	{
		LOADING,		//!< ロード中
		COMPLETED,		//!< ロード完了
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
	// there's no 'id' in cpp, so we recommend returning the class instance pointer
	static cocos2d::Scene* createScene();

	//=====================================================
	// メソッド宣言
	//=====================================================
public:


	// Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
	virtual bool init();


	
	// implement the "static create()" method manually
	CREATE_FUNC(ScenePreLoad);


	/**
		@brief		更新
		@param[in]	delta	1フレーム間隔
		@return		なし
	*/
	void update(float delta) override;

protected:
	

private:
	
	/**
		@brief		タッチリスナーの設定
		@param[in]	なし
		@return		なし
	*/
	void _SetTouchListener();


	/**
		@brief		リソースのロード
		@param[in]	なし
		@return		なし
	*/
	void _LoadResource();

	/**
		@brief		1つSpriteが読み込まれたら呼ばれる
		@param[in]	pTexture		読み込んだSpriteのテクスチャ
		@return		なし
	*/
	void _LoadedResource(cocos2d::Texture2D* pTexture);



	//=====================================================
	// 変数宣言
	//=====================================================
protected:

private:
	Phase	m_phase;			//!< フェーズ
	
	size_t	m_imageNum;			//!< ロードする枚数
	size_t	m_loadedNum;		//!< ロードが済んだ個数


};


#endif	// __SCENE_PRELOAD_H__