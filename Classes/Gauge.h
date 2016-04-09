/**=============================================================================
	@file		Gauge.h
	@brief		ゲージクラス 定義
	@author		Masato Yamamoto
	@date		2015-03-13
			
	@par		[説明]
				ゲージ画像クラス
				値に応じてゲージの幅と高さを自動調整する
				画像の反転表示、方向指定等も行える

	@note

		Copyright (C) 2015  saitsuyo
=============================================================================*/
#ifndef __GAUGE_H__
#define __GAUGE_H__

#ifndef __COCOS2D_H__
#include "cocos2d.h"
#endif
/**
	@class		Gauge
	@brief		ゲージクラス
*/
class Gauge : public cocos2d::Node
{
	//=====================================================
	// 定数(列挙型)宣言
	//=====================================================
public:
	/**
		@brief	方向
	*/
	enum class Direction
	{
		HORIZONTAL,		//!< 水平方向
		VERTICAL,		//!< 垂直方向
	};

	/**
		@brief	計算の基準
	*/
	enum class CalculateBase
	{
		WIDTH,		//!< ゲージ画像の幅
		HEIGHT,		//!< ゲージ画像の高さ
	};
	
	//=====================================================
	// 内部クラス宣言
	//=====================================================
public:

	//=====================================================
	// 型別名定義
	//=====================================================
public:
	typedef cocos2d::Node super;

	//=====================================================
	// 静的メソッド宣言
	//=====================================================
public:
	/**
		@brief		初期化
		@param[in]	filename	ゲージファイル名
		@param[in]	direction	ゲージ配置方向
		@return		ゲージクラス
	*/
	static Gauge* create(const std::string& filename, Direction direction = Direction::HORIZONTAL);
private:
	// implement the "static create()" method manually
	CREATE_FUNC(Gauge);
	//=====================================================
	// メソッド宣言
	//=====================================================
public:


	/**
		@brief		デストラクタ
		@param[in]	なし
		@return		なし
	*/
	virtual ~Gauge();

	
	// Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
	virtual bool init();


	/**
		@brief		ゲージ用スプライトの取得
		@param[in]	なし
		@return		ゲージ用スプライト
	*/
	//cocos2d::Sprite* getGaugeSprite(){ return m_pGaugeSprite; }

	/**
		@brief		アンカーポイントの設定
		@param[in]	anchorPoint	アンカーポイント
		@return		なし
	*/
	void setAnchorPoint(const cocos2d::Vec2& anchorPoint) override;
	

	/**
		@brief		配置方向の設定
		@param[in]	direction	配置方向
		@return		なし
	*/
	void setDirection(Direction direction);

	/**
		@brief		反転配置フラグの設定
		@param[in]	isTurn	反転配置フラグ
		@return		なし
	*/
	void setTurn(bool isTurn);

	/**
		@biref		値の設定
		@param[in]	value	値(0.0~1.0)
		@return		なし
	*/
	void setValue(float value);

	/**
		@brief		値の設定
		@param[in]	value		値
		@param[in]	maxValue	最大値
		@return		なし
	*/
	template<typename _T> void setValue(_T value, _T maxValue)
	{
		setValue(static_cast<float>(value) / maxValue);
	}

	
	/**
		@brief		値の設定
		@param[in]	value	値
		@param[in]	base	値を決める基準
		@return		なし
	*/
	template<typename _T> void setValue(_T value, CalculateBase base)
	{
		auto size = m_pGaugeSprite->getContentSize();
		if (CalculateBase::WIDTH == base)
		{
			setValue(value / size.width);
		}
		else if (CalculateBase::HEIGHT == base)
		{
			setValue(value / size.height);
		}
	}
	

protected:

	/**
		@brief		コンストラクタ
		@param[in]	なし
		@return		なし
	*/
	Gauge();

private:


	/**
		@brief		ゲージ画像読み込み
		@param[in]	filename	ゲージファイル名
		@return		読み込み成功したか
	*/
	bool _InitWithFile(const std::string& filename);


	/**
		@brief		ゲージ画像の座標調整
		@param[in]	なし
		@return		なし
	*/
	void _UpdatePosition();

	/**
		@brief		ゲージ画像の向きを合わせる
		@param[in]	なし
		@return		なし
	*/
	void _UpdateDirection();

	/**
		@brief		ゲージ画像の中身を調整
		@param[in]	なし
		@return		なし
	*/
	void _UpdateGauge();

	//=====================================================
	// 変数宣言
	//=====================================================
protected:

private:
	Direction			m_direction;		//!< 方向
	cocos2d::Sprite*	m_pGaugeSprite;		//!< ゲージ用画像
	cocos2d::Size		m_gaugeSize;		//!< ゲージサイズ
	bool				m_isTurn;			//!< 反転
	float				m_val;				//!< 値 0.0~1.0
};

#endif	// __GAUGE_H__