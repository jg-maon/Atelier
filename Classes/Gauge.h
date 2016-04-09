/**=============================================================================
	@file		Gauge.h
	@brief		�Q�[�W�N���X ��`
	@author		Masato Yamamoto
	@date		2015-03-13
			
	@par		[����]
				�Q�[�W�摜�N���X
				�l�ɉ����ăQ�[�W�̕��ƍ�����������������
				�摜�̔��]�\���A�����w�蓙���s����

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
	@brief		�Q�[�W�N���X
*/
class Gauge : public cocos2d::Node
{
	//=====================================================
	// �萔(�񋓌^)�錾
	//=====================================================
public:
	/**
		@brief	����
	*/
	enum class Direction
	{
		HORIZONTAL,		//!< ��������
		VERTICAL,		//!< ��������
	};

	/**
		@brief	�v�Z�̊
	*/
	enum class CalculateBase
	{
		WIDTH,		//!< �Q�[�W�摜�̕�
		HEIGHT,		//!< �Q�[�W�摜�̍���
	};
	
	//=====================================================
	// �����N���X�錾
	//=====================================================
public:

	//=====================================================
	// �^�ʖ���`
	//=====================================================
public:
	typedef cocos2d::Node super;

	//=====================================================
	// �ÓI���\�b�h�錾
	//=====================================================
public:
	/**
		@brief		������
		@param[in]	filename	�Q�[�W�t�@�C����
		@param[in]	direction	�Q�[�W�z�u����
		@return		�Q�[�W�N���X
	*/
	static Gauge* create(const std::string& filename, Direction direction = Direction::HORIZONTAL);
private:
	// implement the "static create()" method manually
	CREATE_FUNC(Gauge);
	//=====================================================
	// ���\�b�h�錾
	//=====================================================
public:


	/**
		@brief		�f�X�g���N�^
		@param[in]	�Ȃ�
		@return		�Ȃ�
	*/
	virtual ~Gauge();

	
	// Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
	virtual bool init();


	/**
		@brief		�Q�[�W�p�X�v���C�g�̎擾
		@param[in]	�Ȃ�
		@return		�Q�[�W�p�X�v���C�g
	*/
	//cocos2d::Sprite* getGaugeSprite(){ return m_pGaugeSprite; }

	/**
		@brief		�A���J�[�|�C���g�̐ݒ�
		@param[in]	anchorPoint	�A���J�[�|�C���g
		@return		�Ȃ�
	*/
	void setAnchorPoint(const cocos2d::Vec2& anchorPoint) override;
	

	/**
		@brief		�z�u�����̐ݒ�
		@param[in]	direction	�z�u����
		@return		�Ȃ�
	*/
	void setDirection(Direction direction);

	/**
		@brief		���]�z�u�t���O�̐ݒ�
		@param[in]	isTurn	���]�z�u�t���O
		@return		�Ȃ�
	*/
	void setTurn(bool isTurn);

	/**
		@biref		�l�̐ݒ�
		@param[in]	value	�l(0.0~1.0)
		@return		�Ȃ�
	*/
	void setValue(float value);

	/**
		@brief		�l�̐ݒ�
		@param[in]	value		�l
		@param[in]	maxValue	�ő�l
		@return		�Ȃ�
	*/
	template<typename _T> void setValue(_T value, _T maxValue)
	{
		setValue(static_cast<float>(value) / maxValue);
	}

	
	/**
		@brief		�l�̐ݒ�
		@param[in]	value	�l
		@param[in]	base	�l�����߂�
		@return		�Ȃ�
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
		@brief		�R���X�g���N�^
		@param[in]	�Ȃ�
		@return		�Ȃ�
	*/
	Gauge();

private:


	/**
		@brief		�Q�[�W�摜�ǂݍ���
		@param[in]	filename	�Q�[�W�t�@�C����
		@return		�ǂݍ��ݐ���������
	*/
	bool _InitWithFile(const std::string& filename);


	/**
		@brief		�Q�[�W�摜�̍��W����
		@param[in]	�Ȃ�
		@return		�Ȃ�
	*/
	void _UpdatePosition();

	/**
		@brief		�Q�[�W�摜�̌��������킹��
		@param[in]	�Ȃ�
		@return		�Ȃ�
	*/
	void _UpdateDirection();

	/**
		@brief		�Q�[�W�摜�̒��g�𒲐�
		@param[in]	�Ȃ�
		@return		�Ȃ�
	*/
	void _UpdateGauge();

	//=====================================================
	// �ϐ��錾
	//=====================================================
protected:

private:
	Direction			m_direction;		//!< ����
	cocos2d::Sprite*	m_pGaugeSprite;		//!< �Q�[�W�p�摜
	cocos2d::Size		m_gaugeSize;		//!< �Q�[�W�T�C�Y
	bool				m_isTurn;			//!< ���]
	float				m_val;				//!< �l 0.0~1.0
};

#endif	// __GAUGE_H__