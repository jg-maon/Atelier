/**=============================================================================
	@file		MultiRedolution.cpp
	@brief		マルチリゾリューションクラス 実装
	@author		Tomoaki Hatakeyama Masato Yamamoto
	@date		2014-12-21

	@par		[説明]
				マルチディスプレイ実装
	@note

		Copyright (C) 2015  saitsuyo
=============================================================================*/

#include "MultiResolution.h"

USING_NS_CC;

// 実体
cocos2d::Size MultiResolution::m_frameSize;			// 画面解像度判定
cocos2d::Size MultiResolution::m_baseFrameSize;		// 今起動してる画面のスクリーンサイズ
std::string	MultiResolution::m_directry;			// ディレクトリの名前格納用
float	MultiResolution::m_fontScale = 1.f;			//!< フォントサイズの倍率



/**
	@brief			マルチディスプレイ対応化
	@param[in/out]	director	適応させるディレクタークラス
	@param[in]		width		想定する画面幅
	@param[in]		height		想定する画面高
	@param[in]		resolutionPolicy	適応のさせ方 詳細はResolutionPolicy参照
	@return			なし
*/
void MultiResolution::applyMultiResolution(cocos2d::Director* director, float width, float height, ResolutionPolicy resolutionPolicy)
{
	auto glview = director->getOpenGLView();

	auto baseSize = glview->getFrameSize();

	auto winSize = director->getWinSize();


	// 現在のスクリーンサイズセット
	MultiResolution::setBaseFrameSize(glview->getFrameSize());
	// スクリーンサイズに応じたDPIとフレームサイズの算出
	MultiResolution::calcDpi();
	// 各dpiに応じたサイズの取得
	auto frameSize = MultiResolution::getFrameSize();

	// デザインサイズの設定
	//glview->setDesignResolutionSize(
	//	width,
	//	height,
	//	resolutionPolicy);

	auto resolutionSize = glview->getDesignResolutionSize();
	float scaleFactor = MIN(
		frameSize.height / width,
		frameSize.width / height
		);
	director->setContentScaleFactor(scaleFactor);

	m_fontScale = MIN(
		baseSize.width / width,
		baseSize.height / height
		);

	// デザインサイズの設定
	glview->setDesignResolutionSize(
		frameSize.height,
		frameSize.width,
		resolutionPolicy);

	log("fontScale:%f", m_fontScale);
	log("frameSize:%f,%f", frameSize.width, frameSize.height);
	log("resolutionSize:%f, %f", resolutionSize.width, resolutionSize.height);
	log("bbaseSuize:%f, %f", baseSize.width, baseSize.height);

	log("winSize:%f, %f", winSize.width, winSize.height);

	

	// 使用する画像の振り分け設定
	std::vector<std::string> resolution;
	// dpi毎のフォルダ名の取得
	resolution.push_back(MultiResolution::getDirectry());
	resolution.push_back("fonts/");
	FileUtils::getInstance()->setSearchPaths(resolution);
	FileUtils::getInstance()->setSearchResolutionsOrder(resolution);

	//for (auto& path : resolution)
	//{
	//	FileUtils::getInstance()->addSearchPath(path);
	//	FileUtils::getInstance()->addSearchResolutionsOrder(path);
	//}

}

/**
	@brief		今起動してるスクリーンサイズの設定
	@param[in]	今起動してるスクリーンサイズ
	@return		なし
*/
void MultiResolution::setBaseFrameSize(const cocos2d::Size& frameSize)
{
	m_baseFrameSize = frameSize;
}

/**
	@brief		dpiの設定
	@param[in]	なし
	@return		なし
*/
void MultiResolution::calcDpi()
{
	//CCAssert(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID, "Androidにしか対応していません！");
	const float& height = m_baseFrameSize.height;
	if (_isXdpi(height))
	{
		m_directry = "xdpi";
		m_frameSize = cocos2d::Size(720, 1280);
	}
	else if (_isHdpi(height))
	{
		m_directry = "hdpi";
		m_frameSize = cocos2d::Size(540, 960);
	}
	else if (_isMdpi(height))
	{
		m_directry = "mdpi";
		m_frameSize = cocos2d::Size(360, 640);
	}
	else
	{
		// 例外
		//m_directry = "";
		//cocos2d::Size(0, 0);
		m_directry = "xdpi";
		m_frameSize = cocos2d::Size(720, 1280);
	}
}

/**
	@brief		画像参照ディレクトリの取得
	@param[in]	なし
	@return		ディレクトリネーム
*/
std::string MultiResolution::getDirectry()
{
	return m_directry;
}

/**
	@brief		今起動してるスクリーンサイズのゲッター
	@param[in]	なし
	@return		今起動してるスクリーンサイズ
*/
cocos2d::Size MultiResolution::getBaseSize()
{
	return m_baseFrameSize;
}

/**
	@brief		計算したdpiのスクリーンサイズのゲッター
	@param[in]	なし
	@return		計算したdpiのスクリーンサイズ
*/
cocos2d::Size MultiResolution::getFrameSize()
{
	return m_frameSize;
}



/**
	@brief		フォントサイズの倍率取得
	@param[in]	なし
	@return		フォントサイズの倍率
*/
inline float getFontScale()
{
	return MultiResolution::getFontScale();
}
