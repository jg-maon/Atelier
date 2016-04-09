/**=============================================================================
	@file		Gauge.cpp
	@brief		ゲージクラス 実装
	@author		Masato Yamamoto
	@date		2015-03-13
			
	@par		[説明]
				ゲージ画像クラス
				

	@note

		Copyright (C) 2015  saitsuyo
=============================================================================*/
#include "Gauge.h"
USING_NS_CC;

/**
	@brief		初期化
	@param[in]	filename	ゲージファイル名
	@param[in]	direction	ゲージ配置方向
	@return		ゲージクラス
*/
Gauge* Gauge::create(const std::string& filename, Direction direction)
{
	Gauge* _gauge = Gauge::create();
	if (_gauge && _gauge->_InitWithFile(filename))
	{
		_gauge->setDirection(direction);
		return _gauge;
	}
	return nullptr;
}

/**
	@brief		コンストラクタ
	@param[in]	なし
	@return		なし
*/
Gauge::Gauge()
	: m_direction(Direction::HORIZONTAL)
	, m_pGaugeSprite(nullptr)
	, m_gaugeSize()
	, m_isTurn(false)
	, m_val(1.0f)
{

}

/**
	@brief		デストラクタ
	@param[in]	なし
	@return		なし
*/
Gauge::~Gauge()
{

}

// on "init" you need to initialize your instance
bool Gauge::init()
{
	if (!super::init())
	{
		return false;
	}
	setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	return true;
}


/**
	@brief		アンカーポイントの設定
	@param[in]	anchorPoint	アンカーポイント
	@return		なし
*/
void Gauge::setAnchorPoint(const cocos2d::Vec2& anchorPoint)
{
	super::setAnchorPoint(anchorPoint);
	if (m_pGaugeSprite)
		this->_UpdatePosition();
}


/**
	@brief		配置方向の設定
	@param[in]	direction	配置方向
	@return		なし
*/
void Gauge::setDirection(Direction direction)
{
	if (m_direction == direction)
	{
		return;
	}

	m_direction = direction;
	
	
	this->_UpdateDirection();
	this->_UpdateGauge();
}

/**
	@brief		反転配置フラグの設定
	@param[in]	isTurn	反転配置フラグ
	@return		なし
*/
void Gauge::setTurn(bool isTurn)
{
	if (m_isTurn == isTurn)
	{
		return;
	}
	
	m_isTurn = isTurn;
	this->_UpdateDirection();
	this->_UpdateGauge();
}

/**
	@biref		値の設定
	@param[in]	value	値(0.0~1.0)
	@return		なし
*/
void Gauge::setValue(float value)
{
	m_val = value;
	this->_UpdateGauge();
}



/**
	@brief		ゲージ画像読み込み
	@param[in]	filename	ゲージファイル名
	@return		読み込み成功したか
*/
bool Gauge::_InitWithFile(const std::string& filename)
{
	// ファイル読み込み
	m_pGaugeSprite = Sprite::create(filename);
	if (!m_pGaugeSprite)
	{
		return false;
	}
	this->addChild(m_pGaugeSprite);
	
	m_gaugeSize = m_pGaugeSprite->getContentSize();

	this->_UpdateDirection();
	
	return true;
}

/**
	@brief		ゲージ画像の座標調整
	@param[in]	なし
	@return		なし
*/
void Gauge::_UpdatePosition()
{
	auto gaugeAnchor = m_pGaugeSprite->getAnchorPoint();
	Vec2 pos((gaugeAnchor.x - _anchorPoint.x),
			 (gaugeAnchor.y - _anchorPoint.y));
	if (m_isTurn)
	{
		if (Direction::HORIZONTAL == m_direction)
			pos.x -= m_pGaugeSprite->getScaleX();
		else 
			pos.y -= m_pGaugeSprite->getScaleY();
	}
	pos.x *= m_gaugeSize.width;
	pos.y *= m_gaugeSize.height;
	//
	//pos.x *= m_pGaugeSprite->getScaleX();
	//pos.y *= m_pGaugeSprite->getScaleY();
	m_pGaugeSprite->setPosition(pos);
}


/**
	@brief		ゲージ画像の向きを合わせる
	@param[in]	なし
	@return		なし
*/
void Gauge::_UpdateDirection()
{	
	if (Direction::HORIZONTAL == m_direction)	// 横
	{
		if (m_isTurn)
		{
			m_pGaugeSprite->setScaleX(-1.f);
		}
		else
		{
			m_pGaugeSprite->setScaleX(1.f);
		}
		m_pGaugeSprite->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
	}
	else	// 縦
	{
		if (m_isTurn)
		{
			m_pGaugeSprite->setScaleY(-1.f);
		}
		else
		{
			m_pGaugeSprite->setScaleY(1.f);
		}
		m_pGaugeSprite->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
	}

	this->_UpdatePosition();
}
/**
	@brief		ゲージ画像の中身を調整
	@param[in]	なし
	@return		なし
*/
void Gauge::_UpdateGauge()
{
	Rect rect(0.f, 0.f, m_gaugeSize.width, m_gaugeSize.height);
		
	if (Direction::HORIZONTAL == m_direction)
	{
		// 横方向
		rect.size.width = rect.size.width * m_val;
	}
	else
	{
		// 縦
		rect.origin.y = rect.size.height * (1.f - m_val);
		rect.size.height = rect.size.height * m_val;
	}
		

	m_pGaugeSprite->setTextureRect(rect);
}
