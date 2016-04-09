/**=============================================================================
	@file		OptionLayer.h
	@brief		�I�v�V�������C���[�N���X ��`
	@author		Ken Fujishima
	@date		2015-01-22
			
	@par		[����]
				���ʒ����A�t���O�Ǘ��I�v�V�������C���[
				SceneAtelierMain::SettingLayer����Ă΂��
	@note

		Copyright (C) 2015  saitsuyo
=============================================================================*/

#ifndef __OPTION_LAYER_H__
#define __OPTION_LAYER_H__

#ifndef __COCOS2D_H__
#include "cocos2d.h"
#endif

#ifndef __GRID_LAYER_H__
#include "GridLayer.h"
#endif

#ifndef __SCENE_ATELIER_MAIN_H__
#include "SceneAtelierMain.h"
#endif

/**
	@class		OptionLayer
	@brief		�I�v�V�������C���[�N���X
*/
class OptionLayer : public GridLayer
{
	//=====================================================
	// �萔(�񋓌^)�錾
	//=====================================================
public:

		
	//=====================================================
	// �����N���X�錾
	//=====================================================
public:
	
	//=====================================================
	// �^�ʖ���`
	//=====================================================
public:
	
	//=====================================================
	// �ÓI���\�b�h�錾
	//=====================================================
public:

	//===============================================
	// ���\�b�h�錾
	//=====================================================
public:

	/**
		@brief		�f�X�g���N�^
		@param[in]	�Ȃ�
		@return		�Ȃ�
		@par		�������̉����
	*/
	~OptionLayer();

	
	// Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
	virtual bool init();
	
	
		
	// implement the "static create()" method manually
	CREATE_FUNC(OptionLayer);
	



	/**
		@brief		�I�v�V�������C���[�J�n
		@param[in]	from	�����C���[
		@return		�Ȃ�
	*/
	void restart(const std::string& from) override;



	/**
		@brief		��Ԃ̎擾
		@param[in]	�Ȃ�
		@return		���C���[���
		@retval		INPUT	�ݒ蒆
		@retval		NEXT	����
		@retval		BACK	�߂�
	*/
	SceneAtelierMain::LayerState getState() const { return m_state; }

	
	/**
		@brief		�u���V�T�C�Y�̎擾
		@param[in]	�Ȃ�
		@return		�u���V�T�C�Y
	*/
	float getBrushSize() const { return m_brushSize; }
	/**
		@brief		�u���V�T�C�Y�̐ݒ�
		@param[in]	size	�u���V�T�C�Y
		@return		�Ȃ�
	*/
	void setBrushSize(float size){ m_brushSize = size; }


protected:
	
	
	
private:

		
	/**
		@brief		�����o�ϐ�������
		@param[in]	�Ȃ�
		@return		�Ȃ�
	*/
	void _SetDefaultValues();


	/**
		@brief		�^�b�`���X�i�[�̐ݒ�
		@param[in]	�Ȃ�
		@return		�Ȃ�
	*/
	void _SetTouchListener();

	
	/**
		@brief		�e�l�̃X�v���C�g�ʒu���v�Z
		@param[in]	�Ȃ�
		@return		�Ȃ�
	*/
	void _CalcValueStartPosition();

	//=====================================================
	// �ϐ��錾
	//=====================================================
protected:
	
private:

	std::string			m_fromLayerName;	//!< �I�v�V������ʂɓ��������C���[��

	float				m_bgmVolume;		//!< ����(0.0f~1.0f)
	float				m_seVolume;			//!< ����(0.0f~1.0f)


	//cocos2d::Size	m_lineSize;		//!< ����
	//cocos2d::Vec2	m_linePos;		//!< ���n�_

	std::vector<cocos2d::Vec2>		m_touchPoints;	//!< �^�b�`���W

	SceneAtelierMain::LayerState	m_state;	//!< ���C���[���


	float				m_brushSize;		//!< �u���V�T�C�Y(���a)(0.5f~20.0f)
	float				m_minBrushSize;		//!< �u���V�ŏ��T�C�Y
	float				m_maxBrushSize;		//!< �u���V�ő�T�C�Y
	cocos2d::Vec2		m_previewPos;		//!< �u���V�v���r���[�T�C�Y
	cocos2d::DrawNode*	m_pDrawNode;		//!< �u���V�v���r���[


};


//namespace Cocos2dExt {
//	class NativeCodeLauncher
//	{
//	public:
//		static void openTweetDialog(char const *tweet, char const *filePath);
//	};
//}
//

/**
	@class	SceneOption
	@brief	�I�v�V�������C���[�\�t���V�[��
*/
class SceneOption : public cocos2d::Layer
{
public:
	static cocos2d::Scene* createScene();

	virtual bool init();

	void func();

	CREATE_FUNC(SceneOption);

};

#endif // __OPTION_LAYER_H__