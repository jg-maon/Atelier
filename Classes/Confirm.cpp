/**=============================================================================
	@file		Confirm.cpp
	@brief		�m�F�_�C�A���O�p�N���X ����
	@author		Masato Yamamoto
	@date		2015-01-08
			
	@par		[����]
				JNI
	@note

		Copyright (C) 2014  M.Yamamoto
=============================================================================*/
//cpp�t�@�C��
using namespace MyExt;
//���������Ŏg����`
#define JNI_PACKAGE_CLASS_NAME "Java�̃N���X��(�p�b�P�[�W)"
#define JNI_METHOD_NAME "Java���̃��\�b�h��"
//�R�[���o�b�N�N���X���ꎞ�I�ɕێ�����N���X�ϐ���p�ӂ��Ƃ���
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
