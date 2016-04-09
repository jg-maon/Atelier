/**=============================================================================
	@file		Confirm.cpp
	@brief		確認ダイアログ用クラス 実装
	@author		Masato Yamamoto
	@date		2015-01-08
			
	@par		[説明]
				JNI
	@note

		Copyright (C) 2014  M.Yamamoto
=============================================================================*/
//cppファイル
using namespace MyExt;
//ここだけで使う定義
#define JNI_PACKAGE_CLASS_NAME "Javaのクラス名(パッケージ)"
#define JNI_METHOD_NAME "Java側のメソッド名"
//コールバッククラスを一時的に保持するクラス変数を用意しとくよ
static MessageBoxCallback* sMessageBoxCallback = NULL;

void Native::openMessageDialog(const MessageBox &msgBox, MessageBoxCallback* callback){
	//(1)
	if (sMessageBoxCallback != callback){
		if (sMessageBoxCallback != NULL){
			delete sMessageBoxCallback;
		}
		sMessageBoxCallback = callback;
	}
	//(2)
	JniMethodInfo methodInfo;
	if (JniHelper::getStaticMethodInfo(methodInfo, JNI_PACKAGE_CLASS_NAME, JNI_METHOD_NAME, "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V")){
		//(3)
		jstring title = methodInfo.env->NewStringUTF(msgBox.title);
		jstring message = methodInfo.env->NewStringUTF(msgBox.message);
		jstring okButton = methodInfo.env->NewStringUTF(msgBox.okButton);
		jstring cancelButton = methodInfo.env->NewStringUTF(msgBox.cancelButton);
		//(4)
		methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, title, message, okButton, cancelButton);
		//(5)
		methodInfo.env->DeleteLocalRef(title);
		methodInfo.env->DeleteLocalRef(message);
		methodInfo.env->DeleteLocalRef(okButton);
		methodInfo.env->DeleteLocalRef(cancelButton);
		methodInfo.env->DeleteLocalRef(methodInfo.classID);
	}
}
