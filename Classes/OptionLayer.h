/**=============================================================================
	@file		OptionLayer.h
	@brief		オプションレイヤークラス 定義
	@author		Ken Fujishima
	@date		2015-01-22
			
	@par		[説明]
				音量調整、フラグ管理オプションレイヤー
				SceneAtelierMain::SettingLayerから呼ばれる
	@note

		Copyright (C) 2015  saitsuyo
=============================================================================*/

#ifndef __OPTION_LAYER_H__
#define __OPTION_LAYER_H__

#ifndef __COCOS2D_H__
#include "cocos2d.h"
#endif

#ifndef __GRID_LAYER_H__
#include "GridLayer.h"
#endif

#ifndef __SCENE_ATELIER_MAIN_H__
#include "SceneAtelierMain.h"
#endif

/**
	@class		OptionLayer
	@brief		オプションレイヤークラス
*/
class OptionLayer : public GridLayer
{
	//=====================================================
	// 定数(列挙型)宣言
	//=====================================================
public:

		
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

	//===============================================
	// メソッド宣言
	//=====================================================
public:

	/**
		@brief		デストラクタ
		@param[in]	なし
		@return		なし
		@par		メモリの解放等
	*/
	~OptionLayer();

	
	// Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
	virtual bool init();
	
	
		
	// implement the "static create()" method manually
	CREATE_FUNC(OptionLayer);
	



	/**
		@brief		オプションレイヤー開始
		@param[in]	from	元レイヤー
		@return		なし
	*/
	void restart(const std::string& from) override;



	/**
		@brief		状態の取得
		@param[in]	なし
		@return		レイヤー状態
		@retval		INPUT	設定中
		@retval		NEXT	決定
		@retval		BACK	戻る
	*/
	SceneAtelierMain::LayerState getState() const { return m_state; }

	
	/**
		@brief		ブラシサイズの取得
		@param[in]	なし
		@return		ブラシサイズ
	*/
	float getBrushSize() const { return m_brushSize; }
	/**
		@brief		ブラシサイズの設定
		@param[in]	size	ブラシサイズ
		@return		なし
	*/
	void setBrushSize(float size){ m_brushSize = size; }


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
		@brief		各値のスプライト位置を計算
		@param[in]	なし
		@return		ない
	*/
	void _CalcValueStartPosition();

	//=====================================================
	// 変数宣言
	//=====================================================
protected:
	
private:

	std::string			m_fromLayerName;	//!< オプション画面に入ったレイヤー名

	float				m_bgmVolume;		//!< 音量(0.0f~1.0f)
	float				m_seVolume;			//!< 音量(0.0f~1.0f)


	//cocos2d::Size	m_lineSize;		//!< 線幅
	//cocos2d::Vec2	m_linePos;		//!< 線始点

	std::vector<cocos2d::Vec2>		m_touchPoints;	//!< タッチ座標

	SceneAtelierMain::LayerState	m_state;	//!< レイヤー状態


	float				m_brushSize;		//!< ブラシサイズ(半径)(0.5f~20.0f)
	float				m_minBrushSize;		//!< ブラシ最小サイズ
	float				m_maxBrushSize;		//!< ブラシ最大サイズ
	cocos2d::Vec2		m_previewPos;		//!< ブラシプレビューサイズ
	cocos2d::DrawNode*	m_pDrawNode;		//!< ブラシプレビュー


};


//namespace Cocos2dExt {
//	class NativeCodeLauncher
//	{
//	public:
//		static void openTweetDialog(char const *tweet, char const *filePath);
//	};
//}
//

/**
	@class	SceneOption
	@brief	オプションレイヤー貼付けシーン
*/
class SceneOption : public cocos2d::Layer
{
public:
	static cocos2d::Scene* createScene();

	virtual bool init();

	void func();

	CREATE_FUNC(SceneOption);

};

#endif // __OPTION_LAYER_H__