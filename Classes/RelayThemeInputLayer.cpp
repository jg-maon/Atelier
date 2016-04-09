/**=============================================================================
	@file		RelayThemeInputLayer.cpp
	@brief		お絵かきリレーモード時のテーマ入力画面レイヤークラス 実装
	@author		Masato Yamamoto
	@date		2015-02-11
			
	@par		[説明]
				お絵かきリレーモード時のテーマ入力画面レイヤー
				テーマ、よみがな
	@note

		Copyright (C) 2015  saitsuyo
=============================================================================*/

#include "RelayThemeInputLayer.h"


USING_NS_CC;
USING_NS_CC_EXT;

#pragma execution_character_set("utf-8")


#pragma region RelayThemeInputLayer methods



//---------------------------------------------------------------------------------------------------
// ボックス表示時に呼ばれる
void RelayThemeInputLayer::editBoxEditingDidBegin(EditBox* editBox)
{
	log("editBox %p DidBegin !", editBox);
}
// OKボタン押したら呼ばれる
void RelayThemeInputLayer::editBoxTextChanged(EditBox* editBox, const std::string& text)
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
		if (s == "ThemeBox")
		{
			scene->setSmallTheme(text);
		}
		else if (s == "KanaThemeBox")
		{
			scene->setKanaTheme(text);
		}
	}
}

// ボックス終了時に呼ばれる
void RelayThemeInputLayer::editBoxEditingDidEnd(EditBox* editBox)
{
	log("editBox %p DidEnd !", editBox);
}

// ボックス終了後に呼ばれる
void RelayThemeInputLayer::editBoxReturn(EditBox* editBox)
{
	log("editBox %p was returned !", editBox);

}
//---------------------------------------------------------------------------------------------------

/**
	@brief		入力レイヤー開始
	@param[in]	from	元レイヤー
	@return		なし
*/
void RelayThemeInputLayer::restart(const std::string& from)
{
	super::restart(from);
	
	//-----------------------------------------------------
	// EditBox有効化
	this->getChildByName<EditBox*>("ThemeBox")->setEnabled(true);
	this->getChildByName<EditBox*>("KanaThemeBox")->setEnabled(true);
	
}


/**
	@brief		初期化
	@param[in]	なし
	@return		初期化成功か
	@retval		true	初期化成功
	@retval		false	初期化失敗
*/
bool RelayThemeInputLayer::_Init()
{

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	//---------------------------------------------------------------------------------------------------
	// テキストボックス
	// ボックスのサイズ
	auto box = Sprite::create("ThemeInputBox.png");
	Size editBoxSize(box->getContentSize() - Size(40.f, 0.f));

	// テーマ用
	auto themeBox = EditBox::create(editBoxSize,
									Scale9Sprite::create());
	if (themeBox)
	{
		// ボックス背景
		auto back = Scale9Sprite::create("ThemeInputBox.png");
		back->setPreferredSize(box->getContentSize());
		back->setPosition(origin.x + visibleSize.width / 2.f,
						  origin.y + visibleSize.height * 2.f / 3.f);
		this->addChild(back, 5);

		// box
		themeBox->setAnchorPoint(Vec2(0.5f, 0.5f));
		themeBox->setPosition(back->getPosition());
		// フォント
		themeBox->setFontName("GenShinGothic-Regular.ttf");	// ファミリー
		themeBox->setFontSize(25);		// サイズ
		themeBox->setFontColor(Color3B(250, 250, 250));	// 色
		// ラベル
		themeBox->getLabel()->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
		themeBox->getLabel()->setPosition(editBoxSize / 2.f);
		// 初期フォント
		themeBox->setPlaceHolder("例：シンデレラ");	// 例等初期値(入力時に消える)
		themeBox->setPlaceholderFontColor(Color3B(250, 250, 15));
		// 初期ラベル
		themeBox->getPlaceHolderLabel()->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
		themeBox->getPlaceHolderLabel()->setPosition(editBoxSize / 2.f);

		// Box自体の設定
		themeBox->setMaxLength(20);	// 文字数
		themeBox->setReturnType(EditBox::KeyboardReturnType::DEFAULT);	// Returnキーが押された時
		themeBox->setInputMode(EditBox::InputMode::SINGLE_LINE);			// 1行,自由文字列

		// メソッド
		themeBox->setDelegate(this);

		this->addChild(themeBox, 10, "ThemeBox");




		// ラベル

		//auto titleLabel = Label::createWithSystemFont("大テーマ", "GenShinGothic-Regular.ttf", calcFontSize(40.f));
		auto titleLabel = Sprite::create("CharaTheme.png");
		//if (titleLabel)
		{
			titleLabel->setColor(Color3B(250, 250, 250));
			titleLabel->setPosition(themeBox->getPositionX(),
									themeBox->getPositionY() + themeBox->getContentSize().height);

			this->addChild(titleLabel, 11, "BigThemeLabel");
		}
	}

	// かなテーマ用
	auto kanaThemeBox = EditBox::create(editBoxSize,
										 Scale9Sprite::create());
	if (kanaThemeBox)
	{
		// ボックス背景
		auto back = Scale9Sprite::create("ThemeInputBox.png");
		auto size = Label::createWithSystemFont("あああああいいいいいうううううえええええおおおおおかかかかかきききききくくくくく", "GenShinGothic-Medium", 25)->getContentSize();
		back->setPreferredSize(Size(size.width, MAX(size.height*2.0f, box->getContentSize().height)));
		back->setPosition(origin.x + visibleSize.width / 2.f,
						  origin.y + visibleSize.height * 1.f / 3.f);
		this->addChild(back, 5);


		// box
		kanaThemeBox->setAnchorPoint(Vec2(0.5f, 0.5f));
		kanaThemeBox->setPosition(back->getPosition());
		// フォント
		kanaThemeBox->setFontName("GenShinGothic-Regular.ttf");	// ファミリー
		kanaThemeBox->setFontSize(25);		// サイズ
		kanaThemeBox->setFontColor(Color3B(250, 250, 250));	// 色
		// ラベル
		kanaThemeBox->getLabel()->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
		kanaThemeBox->getLabel()->setPosition(editBoxSize / 2.f);
		// 初期フォント
		kanaThemeBox->setPlaceHolder("例：しんでれら");	// 例等初期値(入力時に消える)
		kanaThemeBox->setPlaceholderFontColor(Color3B(250, 250, 15));
		// 初期ラベル
		kanaThemeBox->getPlaceHolderLabel()->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
		kanaThemeBox->getPlaceHolderLabel()->setPosition(editBoxSize / 2.f);


		// Box自体の設定
		kanaThemeBox->setMaxLength(40);	// 文字数
		kanaThemeBox->setReturnType(EditBox::KeyboardReturnType::DEFAULT);	// Returnキーが押された時
		kanaThemeBox->setInputMode(EditBox::InputMode::SINGLE_LINE);		// 1行,自由文字列

		// メソッド
		kanaThemeBox->setDelegate(this);

		this->addChild(kanaThemeBox, 10, "KanaThemeBox");

		// ラベル

		auto titleLabel = Sprite::create("CharaKanaTheme.png");
		//if (titleLabel)
		{
			titleLabel->setColor(Color3B(250, 250, 250));
			titleLabel->setPosition(kanaThemeBox->getPositionX(),
									kanaThemeBox->getPositionY() + kanaThemeBox->getContentSize().height);

			this->addChild(titleLabel, 11, "KanaThemeLabel");
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
void RelayThemeInputLayer::_SetErrorFlag(const std::string& editBoxName, bool flag)
{
	if (editBoxName == "ThemeBox")
	{
		m_errorList[0] = flag;
	}
	else if (editBoxName == "KanaThemeBox")
	{
		m_errorList[1] = flag;
	}
}



/**
	@brief		アニメーション開始
	@param[in]	なし
	@return		なし
*/
void RelayThemeInputLayer::_StartAnimation()
{
	// アニメーション中の動作拒否
	this->getChildByName<EditBox*>("ThemeBox")->setEnabled(false);
	this->getChildByName<EditBox*>("KanaThemeBox")->setEnabled(false);
}

#pragma endregion	// RelayThemeInputLayer methods

