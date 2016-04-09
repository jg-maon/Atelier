/**=============================================================================
	@file		Confirm.h
	@brief		確認ダイアログ用クラス 定義
	@author		Masato Yamamoto
	@date		2015-01-08
			
	@par		[説明]
				JNI
	@note

		Copyright (C) 2014  M.Yamamoto
=============================================================================*/

#ifndef __CONFIRM_H__
#define __CONFIRM_H__

#define ALERTBUTTON_OK 1
#define ALERTBUTTON_CANCEL 2
namespace MyExt{
	//ダイアログ作成用の構造体
	typedef struct _msgBox{
		const char* title; //タイトル
		const char* message; //本文
		const char* okButton; //OKボタンの文字
		const char* cancelButton; //Cancelボタンの文字
	}MessageBox;

	//ボタンの値
	typedef enum _msgBoxResultType{
		kMessageBoxResultOk = ALERTBUTTON_OK,
		kMessageBoxResultCancel = ALERTBUTTON_CANCEL,
	}messageBoxResultType;

	//コールバックのインターフェイス
	class MessageBoxCallback{
	public:
		virtual void onResult(messageBoxResultType type) = 0;
		virtual ~MessageBoxCallback(){};
	};

	//ダイアログ表示のUtilクラス
	class Native{
	public:
		static void openMessageDialog(const MessageBox &msgBox, MessageBoxCallback* callback);
	};
}

#endif	// __CONFIRM_H__