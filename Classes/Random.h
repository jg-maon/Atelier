/**=============================================================================
	@file		Random.h
	@brief		乱数発生クラス 定義
	@author		Masato Yamamoto
	@date		2014-11-30
			
	@par		[説明]
				乱数発生器の定義
	@note

		Copyright (C) 2014  M.Yamamoto
=============================================================================*/
#ifndef __RANDOM_H__
#define __RANDOM_H__

#ifndef __COCOS2D_H__
#include "cocos2d.h"
#endif

#ifndef _RANDOM_
#include <random>
#endif

namespace ns_Random
{
	/**
		@class		Random
		@brief		乱数発生クラス
		@par		[説明]
					<< シングルトンパターン >>
					プログラムの最後にRandom::finalize()を呼ぶこと(出ないとリークを起こしいます)。
					乱数クラス生成時にシード初期化、
					メルセンヌツイスター方式で[min, max)の乱数の取得をする
	*/
	class Random : public ::cocos2d::Ref
	{
		//=====================================================
		// メソッド宣言
		//=====================================================
	public:
		
		/**
			@brief		インスタンス取得
			@param[in]	なし
			@return		乱数発生器
		*/
		static Random* getInstance();

		
		/**
			@brief		デストラクタ
			@param[in]	なし
			@return		なし			
		*/
		virtual ~Random();

		/**
			@brief		乱数シード初期化
			@param[in]	なし
			@return		成功判定			
		*/
		virtual bool init();

		
		/**
			@brief		インスタンス破棄
			@param[in]	なし
			@return		成功判定
		*/
		bool finalize() const;

		/**
			@brief		乱数取得
			@param[in]	_min		最小値
			@param[in]	_max		最大値
			@return		[_min, _max)の乱数
		*/
		template<typename _Ty> _Ty getRandom(const _Ty& _min, const _Ty& _max)
		{
			::std::uniform_real_distribution<_Ty> range(_min, _max);
			return range(_engine);
		}
		
		/**
			@brief		乱数取得(整数値)
			@param[in]	_min		最小値
			@param[in]	_max		最大値
			@return		[min, max)の乱数
		*/
		int getRandomInt(const int& _min, const int& _max)
		{
			::std::uniform_int_distribution<> range(_min, _max);
			return range(_engine);
		}

	protected:

	private:
		/**
			@brief		コンストラクタ
			@param[in]	なし
			@return		なし
			@par		乱数シード初期化
		*/
		Random();

		Random(const Random&) = delete;
		Random& operator= (const Random&) = delete;

		//=====================================================
		// 変数宣言
		//=====================================================
	protected:

	private:

		::std::mt19937 _engine;			//!< メルセンヌツイスター

	};



}	// ns_Random




#ifdef __cplusplus
#define USING_NS_RANDOM		using namespace ns_Random
#else
#define USING_NS_RANDOM
#endif


#endif	// __RANDOM_H__
