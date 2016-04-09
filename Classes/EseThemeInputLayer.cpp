/**=============================================================================
	@file		EseThemeInputLayer.cpp
	@brief		画伯モード時のテーマ入力画面レイヤークラス 実装
	@author		Masato Yamamoto
	@date		2015-02-01
			
	@par		[説明]
				画伯モード時のテーマ入力画面レイヤー
				大テーマ、小テーマ
	@note

		Copyright (C) 2015  saitsuyo
=============================================================================*/

#include "EseThemeInputLayer.h"


USING_NS_CC;
USING_NS_CC_EXT;

#pragma execution_character_set("utf-8")


#pragma region EseThemeInputLayer methods



//---------------------------------------------------------------------------------------------------
// ボックス表示時に呼ばれる
void EseThemeInputLayer::editBoxEditingDidBegin(EditBox* editBox)
{
	log("editBox %p DidBegin !", editBox);
}
// OKボタン押したら呼ばれる
void EseThemeInputLayer::editBoxTextChanged(EditBox* editBox, const std::string& text)
{
	log("editBox %p TextChanged, text: %s ", editBox, text.c_str());
	const auto s = editBox->getName();
	if (text.empty())
	{
		// エラー
		this->_SetErrorFlag(s, true);
	}
	else
	{
		// エラー無し
		this->_SetErrorFlag(s, false);

		// テーマ更新
		auto scene = dynamic_cast<SceneAtelierMain*>(this->getParent()->getParent());
		if (s == "BigThemeBox")
		{
			scene->setBigTheme(text);
		}
		else if (s == "SmallThemeBox")
		{
			scene->setSmallTheme(text);
		}
	}
}

// ボックス終了時に呼ばれる
void EseThemeInputLayer::editBoxEditingDidEnd(EditBox* editBox)
{
	log("editBox %p DidEnd !", editBox);
}

// ボックス終了後に呼ばれる
void EseThemeInputLayer::editBoxReturn(EditBox* editBox)
{
	log("editBox %p was returned !", editBox);

}
//---------------------------------------------------------------------------------------------------

/**
	@brief		入力レイヤー開始
	@param[in]	from	元レイヤー
	@return		なし
*/
void EseThemeInputLayer::restart(const std::string& from)
{
	super::restart(from);
	
	//-----------------------------------------------------
	// EditBox有効化
	this->getChildByName<EditBox*>("BigThemeBox")->setEnabled(true);
	this->getChildByName<EditBox*>("SmallThemeBox")->setEnabled(true);
	
}


/**
	@brief		初期化
	@param[in]	なし
	@return		初期化成功か
	@retval		true	初期化成功
	@retval		false	初期化失敗
*/
bool EseThemeInputLayer::_Init()
{
	// 次レイヤー名
	//m_nextLayerName = "EseMainLayer";

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	//---------------------------------------------------------------------------------------------------
	// テキストボックス
	// ボックスのサイズ
	auto box = Sprite::create("ThemeInputBox.png");
	Size editBoxSize(box->getContentSize() - Size(40.f, 0.f));

	// 大テーマ用
	auto bigThemeBox = EditBox::create(editBoxSize,
									   Scale9Sprite::create());
	if (bigThemeBox)
	{
		// ボックス背景
		auto back = Scale9Sprite::create("ThemeInputBox.png");
		back->setPreferredSize(box->getContentSize());
		back->setPosition(origin.x + visibleSize.width / 2.f,
						  origin.y + visibleSize.height * 2.f / 3.f);
		this->addChild(back, 5);

		// box
		bigThemeBox->setAnchorPoint(Vec2(0.5f, 0.5f));
		bigThemeBox->setPosition(back->getPosition());
		// フォント
		bigThemeBox->setFontName("GenShinGothic-Regular.ttf");	// ファミリー
		bigThemeBox->setFontSize(25);		// サイズ
		bigThemeBox->setFontColor(Color3B(250, 250, 250));	// 色
		// ラベル
		bigThemeBox->getLabel()->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
		bigThemeBox->getLabel()->setPosition(editBoxSize / 2.f);
		// 初期フォント
		bigThemeBox->setPlaceHolder("例：作品");	// 例等初期値(入力時に消える)
		bigThemeBox->setPlaceholderFontColor(Color3B(250, 250, 15));
		// 初期ラベル
		bigThemeBox->getPlaceHolderLabel()->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
		bigThemeBox->getPlaceHolderLabel()->setPosition(editBoxSize / 2.f);

		// Box自体の設定
		bigThemeBox->setMaxLength(20);	// 文字数
		bigThemeBox->setReturnType(EditBox::KeyboardReturnType::DEFAULT);	// Returnキーが押された時
		bigThemeBox->setInputMode(EditBox::InputMode::SINGLE_LINE);			// 1行,自由文字列

		// メソッド
		bigThemeBox->setDelegate(this);

		this->addChild(bigThemeBox, 10, "BigThemeBox");




		// ラベル

		//auto titleLabel = Label::createWithSystemFont("大テーマ", "GenShinGothic-Regular.ttf", calcFontSize(40.f));
		auto titleLabel = Sprite::create("CharaBigTheme.png");
		//if (titleLabel)
		{
			titleLabel->setColor(Color3B(250, 250, 250));
			titleLabel->setPosition(bigThemeBox->getPositionX(),
									bigThemeBox->getPositionY() + bigThemeBox->getContentSize().height);

			this->addChild(titleLabel, 11, "BigThemeLabel");
		}
	}

	// 小テーマ用
	auto smallThemeBox = EditBox::create(editBoxSize,
										 Scale9Sprite::create());
	if (smallThemeBox)
	{
		// ボックス背景
		auto back = Scale9Sprite::create("ThemeInputBox.png");
		back->setPreferredSize(box->getContentSize());
		back->setPosition(origin.x + visibleSize.width / 2.f,
						  origin.y + visibleSize.height * 1.f / 3.f);
		this->addChild(back, 5);


		// box
		smallThemeBox->setAnchorPoint(Vec2(0.5f, 0.5f));
		smallThemeBox->setPosition(back->getPosition());
		// フォント
		smallThemeBox->setFontName("GenShinGothic-Regular.ttf");	// ファミリー
		smallThemeBox->setFontSize(25);		// サイズ
		smallThemeBox->setFontColor(Color3B(250, 250, 250));	// 色
		// ラベル
		smallThemeBox->getLabel()->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
		smallThemeBox->getLabel()->setPosition(editBoxSize / 2.f);
		// 初期フォント
		smallThemeBox->setPlaceHolder("例：シンデレラ");	// 例等初期値(入力時に消える)
		smallThemeBox->setPlaceholderFontColor(Color3B(250, 250, 15));
		// 初期ラベル
		smallThemeBox->getPlaceHolderLabel()->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
		smallThemeBox->getPlaceHolderLabel()->setPosition(editBoxSize / 2.f);


		// Box自体の設定
		smallThemeBox->setMaxLength(20);	// 文字数
		smallThemeBox->setReturnType(EditBox::KeyboardReturnType::DEFAULT);	// Returnキーが押された時
		smallThemeBox->setInputMode(EditBox::InputMode::SINGLE_LINE);		// 1行,自由文字列

		//auto titleLabel = Label::createWithSystemFont("小テーマ", "GenShinGothic-Regular.ttf", 20);
		//titleLabel->setColor(Color3B(5, 5, 5));
		//smallThemeBox->setTitleLabelForState(titleLabel, EditBox::Control::State::NORMAL);
		//smallThemeBox->needsLayout();

		// メソッド
		smallThemeBox->setDelegate(this);

		this->addChild(smallThemeBox, 10, "SmallThemeBox");

		// ラベル

		//auto titleLabel = Label::createWithSystemFont("小テーマ", "GenShinGothic-Regular.ttf", calcFontSize(40.f));
		auto titleLabel = Sprite::create("CharaSmallTheme.png");
		//if (titleLabel)
		{
			titleLabel->setColor(Color3B(250, 250, 250));
			titleLabel->setPosition(smallThemeBox->getPositionX(),
									smallThemeBox->getPositionY() + smallThemeBox->getContentSize().height);

			this->addChild(titleLabel, 11, "SmallThemeLabel");
		}
	}

	// ボックスの数にエラーリスト拡張
	m_errorList.assign(2, true);

	return true;
}


/**
	@brief		入力エラーチェック
	@param[in]	editBoxName		入力されたボックス名
	@param[in]	flag			フラグ
	@return		なし
*/
void EseThemeInputLayer::_SetErrorFlag(const std::string& editBoxName, bool flag)
{
	if (editBoxName == "BigThemeBox")
	{
		m_errorList[0] = flag;
	}
	else if (editBoxName == "SmallThemeBox")
	{
		m_errorList[1] = flag;
	}
}



/**
	@brief		アニメーション開始
	@param[in]	なし
	@return		なし
*/
void EseThemeInputLayer::_StartAnimation()
{
	// アニメーション中の動作拒否
	this->getChildByName<EditBox*>("BigThemeBox")->setEnabled(false);
	this->getChildByName<EditBox*>("SmallThemeBox")->setEnabled(false);
}

#pragma endregion	// EseThemeInputLayer methods

