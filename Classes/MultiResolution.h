/**=============================================================================
	@file		MultiRedolution.h
	@brief		マルチリゾリューションクラス 定義
	@author		Tomoaki Hatakeyama Masato Yamamoto
	@date		2014-12-21

	@par		[説明]
				マルチディスプレイ定義
				<< モノステートパターン >> MultiResolutuion::
	@note
	
			Copyright (C) 2015  saitsuyo
=============================================================================*/

#ifndef __MULTIRESOLUTION_H__
#define __MULTIRESOLUTION_H__

#ifndef __COCOS2D_H__
#include "cocos2d.h"
#endif

/**
	@class		MultiResolution
	@brief		マルチディスプレイ用クラス
	@par		<< モノステートパターン >>
				GLViewの作成を行った直後にapplyMultiResolutionを呼び出すことでマルチディスプレイに対応する

				(正しく使用するためにはスクリーンサイズ等の設定が必要なため、)
				(GLViewの設定を行った後のsetBaseFrameSizeでスクリーンサイズを設定する)
				(その後スクリーンサイズに応じたDPIの処理を行うためにcalcDpiを呼び出す)
*/
class MultiResolution
{
	//=====================================================
	// 定数(列挙型)宣言
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

	/**
		@brief			マルチディスプレイ対応化
		@attention		内部でFileUtils::setSearchPathsを呼び出しているのでパス設定の初期化に注意
		@param[in/out]	director	適応させるディレクタークラス
		@param[in]		width		想定する画面幅
		@param[in]		height		想定する画面高
		@param[in]		resolutionPolicy	適応のさせ方 詳細はResolutionPolicy参照
		@return			なし
	*/
	static void applyMultiResolution(cocos2d::Director* director, float width, float height, ResolutionPolicy resolutionPolicy);


	/**
		@brief		今起動してるスクリーンサイズの設定
		@param[in]	今起動してるスクリーンサイズ
		@return		なし
	*/
	static void setBaseFrameSize(const cocos2d::Size& frameSize);

	/**
		@brief		dpiの設定
		@param[in]	なし
		@return		なし
	*/
	static void calcDpi();

	/**
		@brief		画像参照ディレクトリの取得
		@param[in]	なし
		@return		ディレクトリ名
	*/
	static std::string getDirectry();

	/**
		@brief		今起動してるスクリーンサイズのゲッター
		@param[in]	なし
		@return		今起動してるスクリーンサイズ
	*/
	static cocos2d::Size getBaseSize();

	/**
		@brief		計算したdpiのスクリーンサイズのゲッター
		@param[in]	なし
		@return		計算したdpiのスクリーンサイズ
	*/
	static cocos2d::Size getFrameSize();


	/**
		@brief		フォントサイズの倍率取得
		@param[in]	なし
		@return		フォントサイズの倍率
	*/
	static float getFontScale(){ return m_fontScale; }



	//=====================================================
	// メソッド宣言
	//=====================================================
public:




protected:



private:


	MultiResolution(){}
	//MultiResolution(const MultiResolution&) = delete;
	//MultiResolution& operator=(const MultiResolution&) = delete;
	CC_DISALLOW_COPY_AND_ASSIGN(MultiResolution);

	

	/**
		@brief		高さをみてmdpi判断
		@param[in]	今起動してるスクリーンサイズ
		@return		高さがmpiか
		@retval		true	mdpi
		@retval		false	mdpiではない
	*/
	static bool _isMdpi(float height)
	{
		return 640.f > height;
	}

	/**
		@brief		高さをみてhdpi判断
		@param[in]	今起動してるスクリーンサイズ
		@return		高さがhpiか
		@retval		true	hdpi
		@retval		false	hdpiではない
	*/
	static bool _isHdpi(float height)
	{
		return 960.f > height;
	}

	/**
		@brief		高さをみてxdpi判断
		@param[in]	今起動してるスクリーンサイズ
		@return		高さがxpiか
		@retval		true	xdpi
		@retval		false	xdpiではない
	*/
	static bool _isXdpi(float height)
	{
		return 1280.f > height;
	}



	//=====================================================
	// 変数宣言
	//=====================================================
protected:

private:
	static cocos2d::Size	m_frameSize;		// 画面解像度判定
	static cocos2d::Size	m_baseFrameSize;	// 今起動してる画面のスクリーンサイズ
	static std::string		m_directry;			// ディレクトリの名前格納用

	static float			m_fontScale;		//!< フォントサイズの倍率(960*640)から考えて。
};

/**
	@brief		フォントサイズの倍率取得
	@param[in]	なし
	@return		フォントサイズの倍率
*/
extern inline float getFontScale();


template<typename T>
static T calcFontSize(T fontSize)
{
	return static_cast<T>(fontSize * MultiResolution::getFontScale());
}

template<>
static float calcFontSize<float>(float fontSize)
{
	return fontSize * MultiResolution::getFontScale();
}

#endif	// __MULTIRESOLUTION_H__
