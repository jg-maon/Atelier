/**=============================================================================
	@file		LayerCanvas.hpp
	@brief		キャンバスレイヤークラス 定義
	@author		Masato Yamamoto
	@date		2015-02-01

	@par		[説明]
				お絵かき用キャンバスレイヤー
				NodeGrid継承のためアクションに幅がある

	@note

		Copyright (C) 2015  saitsuyo
=============================================================================*/

#ifndef __LAYER_CANVAS_HPP__
#define __LAYER_CANVAS_HPP__

#ifndef __COCOS2D_H__
#include "cocos2d.h"
#endif

#ifndef __GRID_LAYER_H__
#include "GridLayer.h"
#endif

/**
	@brief	キャンバスレイヤークラス
	@par	Sprite＋NodeGrid(GridLayer)
*/
class LayerCanvas : public GridLayer
{
public:
	CREATE_FUNC(LayerCanvas);

	LayerCanvas() :m_pSprite(nullptr){}
	~LayerCanvas()
	{
		//CC_SAFE_RELEASE_NULL(m_pSprite);
	}
	cocos2d::Sprite* createCanvas(const std::string& filename)
	{
		return createSprite(filename);
	}
	cocos2d::Sprite* createSprite(const std::string& filename)
	{
		if (m_pSprite)
			m_pSprite->release();
		m_pSprite = cocos2d::Sprite::create(filename);
		if (m_pSprite == nullptr)
		{
			return nullptr;
		}
		this->addChild(m_pSprite);
		this->setContentSize(m_pSprite->getContentSize());
		this->setAnchorPoint(m_pSprite->getAnchorPoint());
		return m_pSprite;
	}

	cocos2d::Sprite* getSprite(){ return m_pSprite; }
	const cocos2d::Sprite* getSprite() const { return m_pSprite; }

	cocos2d::Sprite* getCanvas(){ return m_pSprite; }
	const cocos2d::Sprite* getCanvas() const { return m_pSprite; }


private:
	cocos2d::Sprite*	m_pSprite;
};

#endif	// __LAYER_CANVAS_HPP__