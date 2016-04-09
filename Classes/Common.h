/**=============================================================================
	@file		common.h
	@brief		コモン関数 定義
	@author		Masato Yamamoto
	@date		2014-12-20

	@par		[説明]
				演算子のオーバーロードやCocos2d-x特有の処理
	@note

		Copyright (C) 2015  saitsuyo
=============================================================================*/

#ifndef __COMMON_H__
#define __COMMON_H__


#ifndef __COCOS2D_H__
#include "cocos2d.h"
#endif

#ifndef __COCOS2D_EXT_H__
#ifdef _WINDOWS
#include "cocos-ext.h"
#else
#include "extensions/cocos-ext.h"
#endif
#endif


#ifndef LINE_LOG
#define LINE_LOG(format, ...)	cocos2d::log(format " from %s line:%-4d", ## __VA_ARGS__, __FUNCTION__, __LINE__)
#endif
#ifndef logline
#define logline(format, ...)	cocos2d::log(format " from %s line:%-4d", ## __VA_ARGS__, __FUNCTION__, __LINE__)
#endif

#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>

#ifndef arrayof
#define arrayof(x) (sizeof((x)) / sizeof((x)[0]))
#endif

//*
//======================================================
#pragma region 関数のオーバーロード類

namespace std
{
	class floatFixed
	{
		int m_value;
	public:
		floatFixed(int value){ m_value = value; }
		ostream& operator()(ostream& ost) const
		{
			ost << setprecision(m_value);	// 精度指定
			ost << fixed;					// 固定小数点形式
			return ost;
		}
	};

	ostream& operator << (ostream& ost, const floatFixed& fx);
	// 引数に与えた数値を有効数字にするの
	floatFixed fixval(int value);

	// ostreamを初期設定に戻す
	ostream& orgset(ostream& ost);

}
#pragma endregion	// 関数のオーバーロード類
//*/

namespace common
{
	/**
		@brief		有効数字付き浮動小数点を文字列として取得する
		@param[in]	f	値
		@param[in]	val	有効数字(デフォルト:8)
		@param[in]	w	文字幅(デフォルト:8)
		@return		文字列
	*/
	template<typename T_>	::std::string getFloatString(T_ f, int val, int w)
	{
		::std::ostringstream ss;
		ss << ::std::right << ::std::setw(w) << ::std::fixval(val) << f;
		return ss.str();
	}

	/**
		@brief		有効数字付き浮動小数点を文字列として取得する
		@param[in]	f		値
		@param[in]	fill	埋める文字
		@param[in]	val		有効数字(デフォルト:8)
		@param[in]	w		文字幅(デフォルト:8)
		@return		文字列
	*/
	template<typename T_>		::std::string getFloatString(T_ f, char fill, int val, int w)
	{
		::std::ostringstream ss;
		ss << ::std::right << ::std::setfill(fill) << ::std::setw(w) << ::std::fixval(val) << f;
		return ss.str();
	}

	// 文字画像表示状態
	enum class DispState
	{
		HIDE,					// 非表示
		APPEARING,				// 出現中
		SHOW,					// 表示
		DISAPPEARING,			// 消失中
	};
	// 文字配置
	enum Align
	{
		LEFT,					// 左揃え
		RIGHT,					// 右揃え
	};
	// 文字列の置換
	::std::string StrReplace(const ::std::string& str, const ::std::string& search, const ::std::string& replace);

	// ラベルまでファイルストリームを進める
	bool FindChunk(::std::istream& f, const ::std::string& find);

	// ファイルストリームを先頭に戻す
	::std::istream& SeekSet(::std::istream& f);
	// ファイルストリームを先頭に戻す
	::std::ostream& SeekSet(::std::ostream& f);

	namespace time
	{
		/**
			@brief		実行時の年を取得(1900～)
			@param[in]	なし
			@return		実行時の年
		*/
		unsigned int GetYear();
		/**
			@brief		実行時の月を取得(1～12)
			@param[in]	なし
			@return		実行時の月(1～12)
		*/
		unsigned int GetMonth();
		/**
			@brief		実行時の日を取得(1～31)
			@param[in]	なし
			@return		実行時の日(1～31)
		*/
		unsigned int GetDay();


		/**
			@brief		実行時の時間系構造体の取得
			@param[in]	なし
			@return		時間構造体
		*/
		tm GetTimeData();

	}



	/**
		@brief		パーティクル生成
		@attension	通常のcreate()と同じくreleaseを呼ばないとリークを起こす
		@param[in]	plist	Particle2dxで作成した.plistファイル
		@par		Particle2dxで作成したplistからパーティクルの作成
					パーティクルの再生が終了した際に自動破棄するが、addChildなどしないとリークを起こすので注意
					Particle2dx http://particle2dx.com/
	*/
	::cocos2d::ParticleSystemQuad* createParticle(const ::std::string& plist);


#ifndef ADD_BUTTON_PUSHED
	/**
		@biref	ボタンクリックイベントメソッド追加
		@par	ボタン(button)がクリックされた時メソッド(action)が呼ばれる
	*/
#define ADD_BUTTON_PUSHED(button, action) (button)->addTargetWithActionForControlEvents(this, cccontrol_selector(action), cocos2d::extension::Control::EventType::TOUCH_UP_INSIDE)
#endif

}	// namespace common
#endif	// __COMMON_H__
