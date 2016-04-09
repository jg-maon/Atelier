/**=============================================================================
	@file		SceneAtelierMain.h
	@brief		ゲーム本編メインシーンクラス 定義
	@author		Masato Yamamoto
	@date		2015-02-01
			
	@par		[説明]
				ゲーム本編メインシーン
				モードセレクトからの引数で読み込みゲームレイヤーを変える
	@note

		Copyright (C) 2015  saitsuyo
=============================================================================*/

#ifndef __SCENE_ATELIER_MAIN_H__
#define __SCENE_ATELIER_MAIN_H__

// シーンの読み込みをマルチスレッド化するかフラグ
#define USE_MULTITHREAD 0	// 不使用
//#define USE_MULTITHREAD 1	// 使用

#ifndef __COCOS2D_H__
#include "cocos2d.h"
#endif

#ifndef __GRID_LAYER_H__
#include "GridLayer.h"
#endif

#ifndef __COCOS2D_EXT_H__
#ifdef _WINDOWS
#include "cocos-ext.h"
#else
#include "../extensions/cocos-ext.h"
#endif
#endif

#ifndef __SECENE_MODESELECT_H__
#include "SceneModeSelect.h"
#endif

#if USE_MULTITHREAD
#ifndef _THREAD_
#include <thread>
#endif

#ifndef _MUTEX_
#include <mutex>
#endif
#endif

#ifndef _INC_TCHAR

#ifdef UNICODE
typedef wchar_t TCHAR;
#else
typedef char	TCHAR;
#endif

#endif

namespace std {
	typedef basic_string<TCHAR> tstring;
	typedef basic_stringstream<TCHAR> tstringstream;
	typedef basic_istringstream<TCHAR> tistringstream;
	typedef basic_ostringstream<TCHAR> tostringstream;

	typedef basic_fstream<TCHAR> tfstream;
	typedef basic_ifstream<TCHAR> tifstream;
	typedef basic_ofstream<TCHAR> tofstream;

}


/**
	@class		SceneAtelierMain
	@brief		画伯メインシーンクラス
*/
class SceneAtelierMain : public cocos2d::Layer
{
	//=====================================================
	// 定数(列挙型)宣言
	//=====================================================
public:
	/**
		@brief	画伯メイン全体のフェーズ
	*/
	enum class Phase
	{
		SETTING,		//!< 人数入力、画商の選択など
		OPTION,			//!< 音量調整、各種フラグ管理オプション(ソース別ファイル)
		THEME_INPUT,	//!< GM代行なし時テーマ入力
		MAIN,			//!< お絵かき
		REPEAT,			//!< 繰り返しプレイするか
		LAYER_CHANGE,	//!< レイヤー切替中
		SCENE_CHANGE,	//!< シーン遷移中
	};

	/**
	*/
	enum class LayerState
	{
		INPUT = 0,	//!< 入力継続中
		NEXT,		//!< 入力完了(completedの方がいいのか？)
		BACK,		//!< 入力中止(cancelの方が…？)
	};

	//=====================================================
	// 内部クラス宣言
	//=====================================================
public:

	/**
		@class		SettingLayer
		@brief		設定フェーズクラス
	*/
	class SettingLayer : public GridLayer
	{
		//=====================================================
		// 定数(列挙型)宣言
		//=====================================================
	public:
		//enum class Mode
		//{
		//	ESE,			//!< 画伯
		//	COUNTUP,		//!< カウントアップ
		//	COUNTDOWN,		//!< カウントダウン
		//};
		
		//=====================================================
		// 内部クラス宣言
		//=====================================================
	public:
	
		//=====================================================
		// 型別名定義
		//=====================================================
	public:
		typedef std::pair<cocos2d::Sprite*, cocos2d::Sprite*> SpriteOnOff;
	
		//=====================================================
		// 静的メソッド宣言
		//=====================================================
	public:

		//===============================================
		// メソッド宣言
		//=====================================================
	public:

		/**
			@brief		デストラクタ
			@param[in]	なし
			@return		なし
			@par		メモリの解放等
		*/
		~SettingLayer();

	
		// Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
		virtual bool init();
	
	
		
		// implement the "static create()" method manually
		CREATE_FUNC(SettingLayer);
		
		/**
			@brief		モード設定
			@param[in]	mode	モード
			@return		なし
		*/
		void setMode(SceneModeSelect::Select mode);



		/**
			@brief		設定レイヤー開始
			@param[in]	from	元レイヤー
			@return		なし
		*/
		void restart(const std::string& from) override;






		/**
			@brief		状態の取得
			@param[in]	なし
			@return		レイヤー状態
			@retval		INPUT	設定中
			@retval		NEXT	メイン側
			@retval		BACK	オプション
		*/
		SceneAtelierMain::LayerState getState() const { return m_state; }

	protected:
	
	
	
	private:

		
		/**
			@brief		メンバ変数初期化
			@param[in]	なし
			@return		なし
		*/
		void _SetDefaultValues();


		/**
			@brief		タッチリスナーの設定
			@param[in]	なし
			@return		なし
		*/
		void _SetTouchListener();


		/**
			@brief		ボタンタッチ
			@param[in]	pSender		ボタン
			@param[in]	eventType	イベントタイプ
			@return		なし
		*/
		void _ControlButtonPushed(cocos2d::Ref* pSender, cocos2d::extension::Control::EventType eventType);

		//=====================================================
		// 変数宣言
		//=====================================================
	protected:
	
	private:
		LayerState		m_state;			//!< レイヤー状態
		SceneModeSelect::Select	m_mode;		//!< モード
		unsigned int	m_minPlayerNum;		//!< 最少人数
		unsigned int	m_maxPlayerNum;		//!< 最大人数
		unsigned int	m_playerNum;		//!< プレイ人数

		bool			m_isUseGMAgent;		//!< GM代行
		bool			m_isUseGasho;		//!< 画商いる

		cocos2d::Label*	m_pPlayerLabel;		//!< プレイ人数ラベル

		int				m_drawableTime;		//!< お絵かき時間[unit:second]
		cocos2d::Label* m_pDrawableTimeLabel;	//!< お絵かき時間表示ラベル

		//std::vector<SpriteOnOff>		m_GashoOnOff;	//!< 画商○×ボタン
	};

	/**
		@class		ThemeInputLayer
		@brief		GM代行なし時のテーマ入力レイヤークラス
	*/
	class ThemeInputLayer : public GridLayer, public cocos2d::extension::EditBoxDelegate
	{
		//=====================================================
		// 定数(列挙型)宣言
		//=====================================================
	public:
		
		//=====================================================
		// 内部クラス宣言
		//=====================================================
	public:
	
		//=====================================================
		// 型別名定義
		//=====================================================
	public:
		typedef SceneAtelierMain::ThemeInputLayer super;
		//=====================================================
		// 静的メソッド宣言
		//=====================================================
	public:
	
		//=====================================================
		// メソッド宣言
		//=====================================================
	public:
	
	

		/**
			@brief		初期化
			@param[in]	なし
			@return		初期化成功か
			@retval		true	初期化成功
			@retval		false	初期化失敗
		*/
		// Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
		virtual bool init();
	
	
		
		// implement the "static create()" method manually
		CREATE_FUNC(ThemeInputLayer);

		// EditBoxDelegate関数
		// 呼び出されるタイミングはログを参考に
		
		// ボックス表示時に呼ばれる
		virtual void editBoxEditingDidBegin(cocos2d::extension::EditBox* editBox) override;
		// OKボタン押したら呼ばれる
		virtual void editBoxTextChanged(cocos2d::extension::EditBox* editBox, const std::string& text) override;
		// ボックス終了時に呼ばれる
		virtual void editBoxEditingDidEnd(cocos2d::extension::EditBox* editBox) override;
		// ボックス終了後に呼ばれる
		virtual void editBoxReturn(cocos2d::extension::EditBox* editBox) override;



		/**
			@brief		入力レイヤー開始
			@param[in]	from	元レイヤー
			@return		なし
		*/
		virtual void restart(const std::string& from) override;



		/**
			@brief		状態の取得
			@param[in]	なし
			@return		レイヤー状態
			@retval		INPUT	設定中
			@retval		NEXT	決定
			@retval		BACK	戻る
		*/
		SceneAtelierMain::LayerState getState() const { return m_state; }
	
	
	protected:
		
		/**
			@brief		各レイヤー特有の初期化処理
			@param[in]	なし
			@return		初期化成功か
			@retval		true	初期化成功
			@retval		false	初期化失敗
		*/
		virtual bool _Init(){ return true; };
	
	

		/**
			@brief		入力エラーチェック
			@param[in]	editBoxName		入力されたボックス名
			@param[in]	flag			フラグ
			@return		なし
		*/
		virtual void _SetErrorFlag(const std::string& editBoxName, bool flag){};



		/**
			@brief		アニメーション開始
			@param[in]	なし
			@return		なし
		*/
		virtual void _StartAnimation(){}

	private:
		//virtual void _PrivateMethod();
	
		//=====================================================
		// 変数宣言
		//=====================================================
	protected:
		//std::string			m_nextLayerName;			//!< 次レイヤー名
		SceneAtelierMain::LayerState	m_state;		//!< レイヤー状態
		std::vector<bool>				m_errorList;	//!< エラー

	private:


	};

	
	
	/**
		@class		RepeatLayer
		@brief		繰り返しプレイアナウンスレイヤークラス
	*/
	class RepeatLayer : public GridLayer
	{
		//=====================================================
		// 定数(列挙型)宣言
		//=====================================================
	public:


		//=====================================================
		// 内部クラス宣言
		//=====================================================
	public:

		//=====================================================
		// 型別名定義
		//=====================================================
	public:
		
		//=====================================================
		// 静的メソッド宣言
		//=====================================================
	public:

		//===============================================
		// メソッド宣言
		//=====================================================
	public:

		/**
			@brief		デストラクタ
			@param[in]	なし
			@return		なし
			@par		メモリの解放等
		*/
		~RepeatLayer();


		// Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
		virtual bool init();



		// implement the "static create()" method manually
		CREATE_FUNC(RepeatLayer);




		/**
			@brief		繰り返しレイヤー開始
			@param[in]	from	元レイヤー
			@return		なし
		*/
		void restart(const std::string& from) override;



	protected:



	private:

		/**
			@brief		メンバ変数初期化
			@param[in]	なし
			@return		なし
		*/
		void _SetDefaultValues();


		/**
			@brief		タッチリスナーの設定
			@param[in]	なし
			@return		なし
		*/
		void _SetTouchListener();


		/**
			@brief		ボタンタッチ
			@param[in]	pSender		ボタン
			@param[in]	eventType	イベントタイプ
			@return		なし
		*/
		void _ControlButtonPushed(cocos2d::Ref* pSender, cocos2d::extension::Control::EventType eventType);

		//=====================================================
		// 変数宣言
		//=====================================================
	protected:

	private:
		
	};

	

	//=====================================================
	// 型別名定義
	//=====================================================
public:


	//=====================================================
	// 静的メソッド宣言
	//=====================================================
public:
	// there's no 'id' in cpp, so we recommend returning the class instance pointer
	//static cocos2d::Scene* createScene();

	/**
		@brief		各種レイヤー読み込み処理
		@param[in]	mainName		ゲームメインのレイヤー名
		@return		なし
	*/
	static cocos2d::Scene* createScene(const std::string& mainName);

	// implement the "static create()" method manually
	CREATE_FUNC(SceneAtelierMain);

	//=====================================================
	// メソッド宣言
	//=====================================================
public:

	/**
		@brief		コンストラクタ
		@param[in]	なし
		@return		なし
		@par		ロード処理用スレッドの立ち上げ
	*/
	SceneAtelierMain();

	/**
		@brief		デストラクタ
		@param[in]	なし
		@return		なし
		@par		メモリの解放等
	*/
	~SceneAtelierMain();


	// Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
	virtual bool init();



	/**
		@brief		各種レイヤー読み込み処理
		@param[in]	mainName		ゲームメインのレイヤー名
		@return		なし
	*/
	void addLayer(const std::string& mainName);


	/**
		@brief		更新
		@attention	init内で	this->scheduleUpdate();を呼ぶこと
		@param[in]	delta		1フレームの間隔
		@return		なし
	*/
	void update(float delta);

	/**
		@brief		レイヤー切り替え
		@param[in]	from	元レイヤー
		@param[in]	to		次レイヤー
		@return		なし
	*/
	void changeLayer(const std::string& from, const std::string& to);

#if USE_MULTITHREAD
	/**
		@brief		ロード完了
		@param[in]	なし
		@return		なし
	*/
	void loadCompleted()
	{
		m_isLoaded = true;
	}
#endif
	

	/**
		@brief		プレイ人数の取得
		@param[in]	なし
		@return		プレイ人数
	*/
	unsigned int getPlayerNum() const { return m_playerNum; }
	/**
		@brief		プレイ人数の設定
		@param[in]	playerNum	プレイ人数
		@return		なし
	*/
	void setPlayerNum(unsigned int playerNum){ m_playerNum = playerNum; }

	/**
		@brief		GM代行フラグの取得
		@param[in]	なし
		@return		GM代行フラグ
		@retval		true	GM代行する
		@retval		false	GM代行しない
	*/
	bool isUseGMAgent() const { return m_isUseGMAgent; }
	/**
		@brief		GM代行フラグの設定
		@param[in]	isUseGNAgent	GM代行フラグ
		@return		なし
	*/
	void setUseGMAgent(bool isUseGMAgent){ m_isUseGMAgent = isUseGMAgent; }
	
	/**
		@brief		画商フラグの取得
		@param[in]	なし
		@return		画商フラグ
		@retval		true	画商使用する
		@retval		false	画商使用しない
	*/
	bool isUseGasho() const { return m_isUseGasho; }
	/**
		@brief		画商フラグの設定
		@param[in]	isUseGasho	画商フラグ
		@return		なし
	*/
	void setUseGasho(bool isUseGasho){ m_isUseGasho = isUseGasho; }


	/**
		@brief		大テーマの取得
		@param[in]	なし
		@return		大テーマ
	*/
	std::string getBigTheme() const { return m_bigTheme; }
	/**
		@brief		大テーマの設定
		@param[in]	bigTheme	大テーマ
		@return		なし
	*/
	void setBigTheme(const std::string& bigTheme){ m_bigTheme = bigTheme; }


	
	/**
		@brief		小テーマの取得
		@param[in]	なし
		@return		小テーマ
	*/
	std::string getSmallTheme() const { return m_smallTheme; }
	/**
		@brief		小テーマの設定
		@param[in]	smallTheme	小テーマ
		@return		なし
	*/
	void setSmallTheme(const std::string& smallTheme){ m_smallTheme = smallTheme; }


	
	/**
		@brief		かなテーマの取得
		@param[in]	なし
		@return		かなテーマ
	*/
	std::string getKanaTheme() const { return m_kanaTheme; }
	/**
		@brief		かなテーマの設定
		@param[in]	kanaTheme	かなテーマ
		@return		なし
	*/
	void setKanaTheme(const std::string& kanaTheme){ m_kanaTheme = kanaTheme; }





	void toTitle();

protected:



private:
	

	/**
		@brief		メンバ変数初期化
		@param[in]	なし
		@return		なし
	*/
	void _SetDefaultValues();



	/**
		@brief		処理開始
		@param[in]	なし
		@return		なし
	*/
	void _StartEseMain();

	/**
		@brief		タッチリスナーの設定
		@param[in]	なし
		@return		なし
	*/
	void _SetTouchListener();


	/**
		@brief		レイヤー切り替え後コールバック関数
		@param[in]	なし
		@return		なし
		@par		設定レイヤー非表示後に呼ばれる(メインレイヤーの表示)
	*/
	//void _CallBackChangeLayerToMain();

	//=====================================================
	// 変数宣言
	//=====================================================
protected:

private:
#if USE_MULTITHREAD
	std::mutex		m_mutex;				//!< マルチスレッド用
	std::thread		m_loadThread;			//!< ロード処理用スレッド
	bool			m_isLoaded;				//!< ロード完了
#endif
	cocos2d::Layer*	m_pLayers;				//!< レイヤー群

	Phase			m_phase;				//!< フェーズ

	unsigned int	m_playerNum;			//!< プレイ人数 m_pLayerNumではない。

	bool			m_isUseGMAgent;			//!< GM代行
	bool			m_isUseGasho;			//!< 画商いる


	std::string		m_bigTheme;				//!< 大テーマ
	std::string		m_smallTheme;			//!< 小テーマ
	std::string		m_kanaTheme;			//!< かなテーマ


};



///**
//	@class		SubLayer
//	@brief		サブレイヤークラス
//*/
//class SubLayer : public cocos2d::Layer
//{
//	//=====================================================
//	// 定数(列挙型)宣言
//	//=====================================================
//public:
//	
//	//=====================================================
//	// 内部クラス宣言
//	//=====================================================
//public:
//
//	//=====================================================
//	// 型別名定義
//	//=====================================================
//public:
//
//	//=====================================================
//	// 静的メソッド宣言
//	//=====================================================
//public:
//	// there's no 'id' in cpp, so we recommend returning the class instance pointer
//	static cocos2d::Scene* createScene();
//
//	// implement the "static create()" method manually
//	CREATE_FUNC(SubLayer);
//
//	//=====================================================
//	// メソッド宣言
//	//=====================================================
//public:
//
//
//	// Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
//	virtual bool init();
//
//
//	
//
//
//
//protected:
//	//virtual void _ProtectedMethod();
//
//
//private:
//	//virtual void _PrivateMethod();
//
//	//=====================================================
//	// 変数宣言
//	//=====================================================
//protected:
//
//private:
//};



#endif // __SCENE_ATELIER_MAIN_H__
