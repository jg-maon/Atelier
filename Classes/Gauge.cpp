/**=============================================================================
	@file		Gauge.cpp
	@brief		�Q�[�W�N���X ����
	@author		Masato Yamamoto
	@date		2015-03-13
			
	@par		[����]
				�Q�[�W�摜�N���X
				

	@note

		Copyright (C) 2015  saitsuyo
=============================================================================*/
#include "Gauge.h"
USING_NS_CC;

/**
	@brief		������
	@param[in]	filename	�Q�[�W�t�@�C����
	@param[in]	direction	�Q�[�W�z�u����
	@return		�Q�[�W�N���X
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
	@brief		�R���X�g���N�^
	@param[in]	�Ȃ�
	@return		�Ȃ�
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
	@brief		�f�X�g���N�^
	@param[in]	�Ȃ�
	@return		�Ȃ�
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
	@brief		�A���J�[�|�C���g�̐ݒ�
	@param[in]	anchorPoint	�A���J�[�|�C���g
	@return		�Ȃ�
*/
void Gauge::setAnchorPoint(const cocos2d::Vec2& anchorPoint)
{
	super::setAnchorPoint(anchorPoint);
	if (m_pGaugeSprite)
		this->_UpdatePosition();
}


/**
	@brief		�z�u�����̐ݒ�
	@param[in]	direction	�z�u����
	@return		�Ȃ�
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
	@brief		���]�z�u�t���O�̐ݒ�
	@param[in]	isTurn	���]�z�u�t���O
	@return		�Ȃ�
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
	@biref		�l�̐ݒ�
	@param[in]	value	�l(0.0~1.0)
	@return		�Ȃ�
*/
void Gauge::setValue(float value)
{
	m_val = value;
	this->_UpdateGauge();
}



/**
	@brief		�Q�[�W�摜�ǂݍ���
	@param[in]	filename	�Q�[�W�t�@�C����
	@return		�ǂݍ��ݐ���������
*/
bool Gauge::_InitWithFile(const std::string& filename)
{
	// �t�@�C���ǂݍ���
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
	@brief		�Q�[�W�摜�̍��W����
	@param[in]	�Ȃ�
	@return		�Ȃ�
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
	@brief		�Q�[�W�摜�̌��������킹��
	@param[in]	�Ȃ�
	@return		�Ȃ�
*/
void Gauge::_UpdateDirection()
{	
	if (Direction::HORIZONTAL == m_direction)	// ��
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
	else	// �c
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
	@brief		�Q�[�W�摜�̒��g�𒲐�
	@param[in]	�Ȃ�
	@return		�Ȃ�
*/
void Gauge::_UpdateGauge()
{
	Rect rect(0.f, 0.f, m_gaugeSize.width, m_gaugeSize.height);
		
	if (Direction::HORIZONTAL == m_direction)
	{
		// ������
		rect.size.width = rect.size.width * m_val;
	}
	else
	{
		// �c
		rect.origin.y = rect.size.height * (1.f - m_val);
		rect.size.height = rect.size.height * m_val;
	}
		

	m_pGaugeSprite->setTextureRect(rect);
}
