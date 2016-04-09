/**=============================================================================
	@file		GridLayer.h
	@brief		NodeGrid継承レイヤー基底クラス 定義
	@author		Masato Yamamoto
	@date		2014-12-16
			
	@par		[説明]
				GridActionを使うことのできるレイヤークラス

	@note

		Copyright (C) 2015  saitsuyo
=============================================================================*/

#ifndef __GRID_LAYER_H__
#define __GRID_LAYER_H__

#ifndef __COCOS2D_H__
#include "cocos2d.h"
#endif

#ifndef __MULTIRESOLUTION_H__
#include "MultiResolution.h"
#endif


/**
	@class		GridLayer
	@brief		NodeGird継承レイヤークラス
	@par		v3.2
*/
class GridLayer : public cocos2d::NodeGrid
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
	// creates a fullscreen black layer
	static GridLayer *create();

	//=====================================================
	// メソッド宣言
	//=====================================================
public:
	
	/* Callback function should not be deprecated, it will generate lots of warnings.
	Since 'setAccelerometerEnabled' was deprecated, it will make warnings if developer overrides onAcceleration and invokes setAccelerometerEnabled(true) instead of using EventDispatcher::addEventListenerWithXXX.
	*/
	virtual void onAcceleration(cocos2d::Acceleration* acc, cocos2d::Event* unused_event);

	/* Callback function should not be deprecated, it will generate lots of warnings.
	Since 'setKeyboardEnabled' was deprecated, it will make warnings if developer overrides onKeyXXX and invokes setKeyboardEnabled(true) instead of using EventDispatcher::addEventListenerWithXXX.
	*/
	virtual void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* unused_event);
	virtual void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* unused_event);

	// Overrides
	virtual std::string getDescription() const override;

	

	
	// implement the "static create()" method manually
	//CREATE_FUNC(GridLayer);


	/**
		@brief		レイヤーリスタート処理
		@param[in]	from	元レイヤー
		@return		なし
	*/
	virtual void restart(const std::string& from){}




protected:
	GridLayer();
	virtual ~GridLayer();

	// Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
	virtual bool init() override;


private:
	//add the api for avoid use deprecated api
	void _addTouchListener();

	CC_DISALLOW_COPY_AND_ASSIGN(GridLayer);


	//=====================================================
	// 変数宣言
	//=====================================================
protected:

	bool _touchEnabled;
	bool _accelerometerEnabled;
	bool _keyboardEnabled;
	cocos2d::EventListener* _touchListener;
	cocos2d::EventListenerKeyboard* _keyboardListener;
	cocos2d::EventListenerAcceleration* _accelerationListener;

	cocos2d::Touch::DispatchMode _touchMode;
	bool _swallowsTouches;

private:
};

#endif	// __GRID_LAYER_H__
