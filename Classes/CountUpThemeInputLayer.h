/**=============================================================================
	@file		CountUpThemeInputLayer.h
	@brief		カウントアップモード時のテーマ入力画面レイヤークラス 定義
	@author		Masato Yamamoto
	@date		2015-02-01
			
	@par		[説明]
				カウントアップモード時のテーマ入力画面レイヤー
				ヒント、テーマ、テーマのよみがな
	@note

		Copyright (C) 2015  saitsuyo
=============================================================================*/

#ifndef __COUNTUP_THEME_INPUT_LAYER_H__
#define __COUNTUP_THEME_INPUT_LAYER_H__

#ifndef __SCENE_ATELIER_MAIN_H__
#include "SceneAtelierMain.h"
#endif


/**
	@class		CountUpThemeInputLayer
	@brief		GM代行なし時のテーマ入力レイヤークラス
*/
class CountUpThemeInputLayer : public SceneAtelierMain::ThemeInputLayer
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
	
	//=====================================================
	// メソッド宣言
	//=====================================================
public:
	
		
	// implement the "static create()" method manually
	CREATE_FUNC(CountUpThemeInputLayer);

		
	// ボックス表示時に呼ばれる
	virtual void editBoxEditingDidBegin(cocos2d::extension::EditBox* editBox) override;
	// OKボタン押したら呼ばれる
	virtual void editBoxTextChanged(cocos2d::extension::EditBox* editBox, const std::string& text) override;
	// ボックス終了時に呼ばれる
	virtual void editBoxEditingDidEnd(cocos2d::extension::EditBox* editBox) override;
	// ボックス終了後に呼ばれる
	virtual void editBoxReturn(cocos2d::extension::EditBox* editBox) override;



	/**
		@brief		入力レイヤー開始
		@param[in]	from	元レイヤー
		@return		なし
	*/
	void restart(const std::string& from) override;


protected:
	
	/**
		@brief		各レイヤー特有の初期化処理
		@param[in]	なし
		@return		初期化成功か
		@retval		true	初期化成功
		@retval		false	初期化失敗
	*/
	virtual bool _Init() override;
	
	

	/**
		@brief		入力エラーチェック
		@param[in]	editBoxName		入力されたボックス名
		@param[in]	flag			フラグ
		@return		なし
	*/
	virtual void _SetErrorFlag(const std::string& editBoxName, bool flag) override;
	
	
	/**
		@brief		アニメーション開始
		@param[in]	なし
		@return		なし
	*/
	virtual void _StartAnimation() override;
	
private:
	//virtual void _PrivateMethod();
	
	//=====================================================
	// 変数宣言
	//=====================================================
protected:
	
private:

};

#endif	// __COUNTUP_THEME_INPUT_LAYER_H__

	