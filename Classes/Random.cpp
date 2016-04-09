/**=============================================================================
	@file		Random.cpp
	@brief		乱数発生クラス 実装
	@author		Masato Yamamoto
	@date		2014-11-24
			
	@par		[説明]
				乱数発生器の実装
	@note

		Copyright (C) 2014  M.Yamamoto
=============================================================================*/
#include "Random.h"


namespace ns_Random
{
#pragma region class Random : public ::cocos2d::Ref

	static Random *s_SharedRandomGenerator = nullptr;

	/**
		@brief		インスタンス取得
		@param[in]	なし
		@return		乱数発生器
	*/
	Random* Random::getInstance()
	{
		if (!s_SharedRandomGenerator)
		{
			s_SharedRandomGenerator = new (std::nothrow) Random();
			CCASSERT(s_SharedRandomGenerator, "FATAL: Not enough memory");
			s_SharedRandomGenerator->init();
		}

		return s_SharedRandomGenerator;
	}
		
	/**
		@brief		デストラクタ
		@param[in]	なし
		@return		なし			
	*/
	Random::~Random()
	{
		// グローバル削除
		//CC_SAFE_DELETE(s_SharedRandomGenerator);
		s_SharedRandomGenerator = nullptr;

	}

	/**
		@brief		乱数シード初期化
		@param[in]	なし
		@return		成功判定			
	*/
	bool Random::init()
	{
		::std::random_device device;
		_engine.seed(device());
		return true;
	}

	
	/**
		@brief		インスタンス破棄
		@param[in]	なし
		@return		成功判定
	*/
	bool Random::finalize() const
	{
		if (!s_SharedRandomGenerator) return false;
		delete s_SharedRandomGenerator;
		return true;
	}



	/**
		@brief		コンストラクタ
		@param[in]	なし
	*/
	Random::Random()
	{

	}
#pragma endregion	// Random

}	// ns_Random
