/**=============================================================================
	@file		GridLayer.cpp
	@brief		NodeGrid継承レイヤー基底クラス 実装
	@author		Masato Yamamoto
	@date		2014-12-16
			
	@par		[説明]
				GridActionを使うことのできるレイヤークラス

	@note

		Copyright (C) 2015  saitsuyo
=============================================================================*/

#include "GridLayer.h"

#include <stdarg.h>
#include "2d/CCLayer.h"
#include "base/CCScriptSupport.h"
#include "platform/CCDevice.h"
#include "2d/CCScene.h"
#include "renderer/CCGLProgramState.h"
#include "renderer/CCGLProgram.h"
#include "renderer/CCCustomCommand.h"
#include "renderer/CCRenderer.h"
#include "renderer/ccGLStateCache.h"
#include "base/CCDirector.h"
#include "base/CCEventDispatcher.h"
#include "base/CCEventListenerTouch.h"
#include "base/CCEventTouch.h"
#include "base/CCEventKeyboard.h"
#include "base/CCEventListenerKeyboard.h"
#include "base/CCEventAcceleration.h"
#include "base/CCEventListenerAcceleration.h"
#include "math/TransformUtils.h"

#include "deprecated/CCString.h"

#if CC_USE_PHYSICS
#include "physics/CCPhysicsBody.h"
#endif

USING_NS_CC;




// creates a fullscreen black layer
GridLayer* GridLayer::create()
{
	GridLayer *ret = new GridLayer();
	if (ret && ret->init())
	{
		ret->autorelease();
		return ret;
	}
	else
	{
		CC_SAFE_DELETE(ret);
		return nullptr;
	}
}


/* Callback function should not be deprecated, it will generate lots of warnings.
Since 'setAccelerometerEnabled' was deprecated, it will make warnings if developer overrides onAcceleration and invokes setAccelerometerEnabled(true) instead of using EventDispatcher::addEventListenerWithXXX.
*/
void GridLayer::onAcceleration(cocos2d::Acceleration* acc, cocos2d::Event* unused_event)
{
	CC_UNUSED_PARAM(acc);
	CC_UNUSED_PARAM(unused_event);
#if CC_ENABLE_SCRIPT_BINDING
	if (kScriptTypeNone != _scriptType)
	{
		BasicScriptData data(this, (void*)acc);
		ScriptEvent event(kAccelerometerEvent, &data);
		ScriptEngineManager::getInstance()->getScriptEngine()->sendEvent(&event);
	}
#endif
}

/* Callback function should not be deprecated, it will generate lots of warnings.
Since 'setKeyboardEnabled' was deprecated, it will make warnings if developer overrides onKeyXXX and invokes setKeyboardEnabled(true) instead of using EventDispatcher::addEventListenerWithXXX.
*/
void GridLayer::onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* unused_event)
{
	CC_UNUSED_PARAM(keyCode);
	CC_UNUSED_PARAM(unused_event);
}
void GridLayer::onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* unused_event)
{
	CC_UNUSED_PARAM(unused_event);
#if CC_ENABLE_SCRIPT_BINDING
	if (kScriptTypeNone != _scriptType)
	{
		KeypadScriptData data(keyCode, this);
		ScriptEvent event(kKeypadEvent, &data);
		ScriptEngineManager::getInstance()->getScriptEngine()->sendEvent(&event);
	}
#endif
}


// Overrides
std::string GridLayer::getDescription() const
{
	return StringUtils::format("<Layer | Tag = %d>", _tag);
}


GridLayer::GridLayer()
: _touchEnabled(false)
, _accelerometerEnabled(false)
, _keyboardEnabled(false)
, _touchListener(nullptr)
, _keyboardListener(nullptr)
, _accelerationListener(nullptr)
, _touchMode(Touch::DispatchMode::ALL_AT_ONCE)
, _swallowsTouches(true)
{
	_ignoreAnchorPointForPosition = true;
	setAnchorPoint(Vec2(0.5f, 0.5f));
}

GridLayer::~GridLayer()
{
}

// Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
bool GridLayer::init()
{
	Director * director = Director::getInstance();
	setContentSize(director->getWinSize());
	return true;
}


//add the api for avoid use deprecated api
void GridLayer::_addTouchListener()
{}
