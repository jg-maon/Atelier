/**=============================================================================
	@file		CountDownThemeInputLayer.cpp
	@brief		カウントアップモード時のテーマ入力画面レイヤークラス 実装
	@author		Masato Yamamoto
	@date		2015-02-03
			
	@par		[説明]
				カウントアップモード時のテーマ入力画面レイヤー
				ヒント、テーマ、テーマのよみがな
	@note

		Copyright (C) 2015  saitsuyo
=============================================================================*/

#include "CountDownThemeInputLayer.h"

USING_NS_CC;
USING_NS_CC_EXT;

#pragma execution_character_set("utf-8")


#pragma region CountDownThemeInputLayer methods

//---------------------------------------------------------------------------------------------------
// ボックス表示時に呼ばれる
void CountDownThemeInputLayer::editBoxEditingDidBegin(EditBox* editBox)
{
	log("editBox %p DidBegin !", editBox);
}
// OKボタン押したら呼ばれる
void CountDownThemeInputLayer::editBoxTextChanged(EditBox* editBox, const std::string& text)
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
		if (s == "HintBox")
		{
			scene->setBigTheme(text);
		}
		else if (s == "ThemeBox")
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
void CountDownThemeInputLayer::editBoxEditingDidEnd(EditBox* editBox)
{
	log("editBox %p DidEnd !", editBox);
}

// ボックス終了後に呼ばれる
void CountDownThemeInputLayer::editBoxReturn(EditBox* editBox)
{
	log("editBox %p was returned !", editBox);

}
//---------------------------------------------------------------------------------------------------

/**
	@brief		入力レイヤー開始
	@param[in]	from	元レイヤー
	@return		なし
*/
void CountDownThemeInputLayer::restart(const std::string& from)
{
	super::restart(from);
	//-----------------------------------------------------
	// EditBox有効化
	this->getChildByName<EditBox*>("HintBox")->setEnabled(true);
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
bool CountDownThemeInputLayer::_Init()
{
	// 次レイヤー名
	//m_nextLayerName = "CountDownGameLayer";

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();


	//---------------------------------------------------------------------------------------------------
	// テキストボックス
	// ボックスのサイズ
	auto box = Sprite::create("ThemeInputBox.png");
	Size editBoxSize(box->getContentSize() - Size(40.f, 10.f));

	// ヒントテーマ用
	auto hintBox = EditBox::create(editBoxSize,
								   Scale9Sprite::create());
	if (hintBox)
	{
		// ボックス背景
		auto back = Scale9Sprite::create("ThemeInputBox.png");
		back->setPreferredSize(box->getContentSize());
		back->setPosition(origin.x + visibleSize.width / 2.f,
						  origin.y + visibleSize.height * 5.f / 6.f);
		this->addChild(back, 5);

		// box
		hintBox->setAnchorPoint(Vec2(0.5f, 0.5f));
		hintBox->setPosition(back->getPosition());
		// フォント
		hintBox->setFontName("GenShinGothic-Regular.ttf");	// ファミリー
		hintBox->setFontSize(25);		// サイズ
		hintBox->setFontColor(Color3B(250, 250, 250));	// 色
		// ラベル
		hintBox->getLabel()->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
		hintBox->getLabel()->setPosition(editBoxSize / 2.f);
		// 初期フォント
		hintBox->setPlaceHolder("例：作品");	// 例等初期値(入力時に消える)
		hintBox->setPlaceholderFontColor(Color3B(250, 250, 15));
		// 初期ラベル
		hintBox->getPlaceHolderLabel()->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
		hintBox->getPlaceHolderLabel()->setPosition(editBoxSize / 2.f);

		// Box自体の設定
		hintBox->setMaxLength(40);	// 文字数
		hintBox->setReturnType(EditBox::KeyboardReturnType::DEFAULT);	// Returnキーが押された時
		hintBox->setInputMode(EditBox::InputMode::SINGLE_LINE);			// 1行,自由文字列

		// メソッド
		hintBox->setDelegate(this);

		this->addChild(hintBox, 10, "HintBox");




		// ラベル

		//auto titleLabel = Label::createWithSystemFont("大テーマ", "GenShinGothic-Regular.ttf", calcFontSize(40.f));
		auto titleLabel = Sprite::create("CharaHint.png");
		//if (titleLabel)
		{
			titleLabel->setColor(Color3B(250, 250, 250));
			titleLabel->setPosition(hintBox->getPositionX(),
									editBoxSize.height / 2.f + hintBox->getPositionY() + hintBox->getContentSize().height / 2.f);

			this->addChild(titleLabel, 11, "HintLabel");
		}
	}

	// テーマ用
	auto smallThemeBox = EditBox::create(editBoxSize,
										 Scale9Sprite::create());
	if (smallThemeBox)
	{
		// ボックス背景
		auto back = Scale9Sprite::create("ThemeInputBox.png");
		back->setPreferredSize(box->getContentSize());
		back->setPosition(origin.x + visibleSize.width / 2.f,
						  origin.y + visibleSize.height * 3.f / 6.f);
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

		this->addChild(smallThemeBox, 10, "ThemeBox");

		// ラベル

		//auto titleLabel = Label::createWithSystemFont("小テーマ", "GenShinGothic-Regular.ttf", calcFontSize(40.f));
		auto titleLabel = Sprite::create("CharaTheme.png");
		//if (titleLabel)
		{
			titleLabel->setColor(Color3B(250, 250, 250));
			titleLabel->setPosition(smallThemeBox->getPositionX(),
									editBoxSize.height / 2.f + smallThemeBox->getPositionY() + smallThemeBox->getContentSize().height / 2.f);

			this->addChild(titleLabel, 11, "ThemeLabel");
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
		//back->setContentSize(box->getContentSize());
		back->setPreferredSize(Size(size.width, MAX(size.height*2.0f, box->getContentSize().height)));
		back->setPosition(origin.x + visibleSize.width / 2.f,
						  origin.y + visibleSize.height * 1.f / 6.f);
		this->addChild(back, 5);


//		kanaThemeBox->getLabel()->setDimensions(static_cast<unsigned int>(back->getContentSize().width),
//												static_cast<unsigned int>(back->getContentSize().height));
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

		//auto titleLabel = Label::createWithSystemFont("かなテーマ", "GenShinGothic-Regular.ttf", calcFontSize(40.f));
		auto titleLabel = Sprite::create("CharaKanaTheme.png");
		//if (titleLabel)
		{
			titleLabel->setColor(Color3B(250, 250, 250));
			titleLabel->setPosition(kanaThemeBox->getPositionX(),
									editBoxSize.height / 2.f + kanaThemeBox->getPositionY() + kanaThemeBox->getContentSize().height / 2.f);

			this->addChild(titleLabel, 11, "KanaThemeLabel");
		}
	}

	log("CountDownThemeInputLayer::_Init");


	// ボックスの数にエラーリスト拡張
	m_errorList.assign(3, true);

	m_errorList[0] = false;		// ヒントはなくても良い

	return true;
}


/**
	@brief		入力エラーチェック
	@param[in]	editBoxName		入力されたボックス名
	@param[in]	flag			フラグ
	@return		なし
*/
void CountDownThemeInputLayer::_SetErrorFlag(const std::string& editBoxName, bool flag)
{
	if (editBoxName == "ThemeBox")
	{
		m_errorList[1] = flag;
	}
	else if (editBoxName == "KanaThemeBox")
	{
		m_errorList[2] = flag;
	}
}



/**
	@brief		アニメーション開始
	@param[in]	なし
	@return		なし
*/
void CountDownThemeInputLayer::_StartAnimation()
{
	// アニメーション中の動作拒否
	this->getChildByName<EditBox*>("HintBox")->setEnabled(false);
	this->getChildByName<EditBox*>("ThemeBox")->setEnabled(false);
	this->getChildByName<EditBox*>("KanaThemeBox")->setEnabled(false);
}

#pragma endregion	// CountDownThemeInputLayer methods

