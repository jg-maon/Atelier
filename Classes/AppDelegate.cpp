#include "AppDelegate.h"

#include "MultiResolution.h"

#include "Random.h"
USING_NS_RANDOM;

#pragma execution_character_set("utf-8")

#include "SceneTitle.h"			// 始めのシーン
#include "HelloWorldScene.h"			// 始めのシーン
#include "SceneAtelierMain.h"

#include "SimpleAudioEngine.h"	// 音関係

USING_NS_CC;




#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
#define WINDOWS

#ifndef TEXT
#ifndef __TEXT
#define __TEXT(quote) L##quote
#endif
#define TEXT(quote) __TEXT(quote) 
#endif

#define FONT_DIRECTORY TEXT("fonts/")
// 使用フォント一覧
#define USE_FONT	TEXT("Marker Felt.ttf"), \
	TEXT("meiryo.ttc"), \
	TEXT("GenShinGothic-Bold.ttf"), \
	TEXT("GenShinGothic-ExtraLight.ttf"), \
	TEXT("GenShinGothic-Heavy.ttf"), \
	TEXT("GenShinGothic-Light.ttf"), \
	TEXT("GenShinGothic-Medium.ttf"), \
	TEXT("GenShinGothic-Normal.ttf"), \
	TEXT("GenShinGothic-Regular.ttf"),


#endif





AppDelegate::AppDelegate()
{
#ifdef WINDOWS
	// 使用フォントの一時インストール
	std::vector<std::tstring> fonts =
	{
		USE_FONT
	};
	int i = 0;
	for (auto& font : fonts)
	{
		std::tstring fontName(FONT_DIRECTORY);
		if (!AddFontResourceEx((fontName + font).c_str(), FR_PRIVATE, nullptr))
		{
			std::stringstream msg;
			msg << "failed AddFontResourceEx ErrorFontNo:" << i;
			CCASSERT(false, msg.str().c_str());
		}
		++i;
	}
#endif
}

AppDelegate::~AppDelegate() 
{
	Random::getInstance()->finalize();
	CocosDenshion::SimpleAudioEngine::getInstance()->end();

#ifdef WINDOWS
	// 使用フォントのアンインストール
	std::vector<std::tstring> fonts =
	{
		USE_FONT
	};
	int i = 0;
	for (auto& font : fonts)
	{
		std::tstring fontName(TEXT("fonts/"));
		if (!RemoveFontResourceEx((fontName + font).c_str(), FR_PRIVATE, nullptr))
		{
			std::stringstream msg;
			msg << "failed RemoveFontResourceEx ErrorFontNo:" << i;
			CCASSERT(false, msg.str().c_str());
			//CCASSERT(false, "failed RemoveFontResourceEx:%s", font.c_str());
		}
		++i;
	}
#endif
	

}

bool AppDelegate::applicationDidFinishLaunching() {
    // initialize director
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    
	if(!glview) {
        glview = GLView::create("Atelier");
    	//glview = GLView::create("ese");
        director->setOpenGLView(glview);
    }

	MultiResolution::applyMultiResolution(director, 1280.f, 720.f, ResolutionPolicy::EXACT_FIT);

	//// デザインサイズの設定
	//glview->setDesignResolutionSize(
	//	1280,
	//	720,
	//	ResolutionPolicy::EXACT_FIT);

	//// 現在のスクリーンサイズセット
	//MultiResolution::setBaseFrameSize(glview->getFrameSize());
	//// スクリーンサイズに応じたDPIとフレームサイズの算出
	//MultiResolution::calcDpi();
	//// 各dpiに応じたサイズの取得
	//Size frameSize = MultiResolution::getFrameSize();

	//auto resolutionSize = glview->getDesignResolutionSize();
	//director->setContentScaleFactor(MIN(
	//	frameSize.height / resolutionSize.width,
	//	frameSize.width / resolutionSize.height
	//	));

	//// マルチレゾリューション設定
	//std::vector<std::string> resolution;
	//// dpiの取得
	//resolution.push_back(MultiResolution::getDirectry());
	//FileUtils::getInstance()->setSearchPaths(resolution);
	//FileUtils::getInstance()->setSearchResolutionsOrder(resolution);


	
    // turn on display FPS
    //director->setDisplayStats(true);
	director->setDisplayStats(false);
	

    // set FPS. the default value is 1.0/60 if you don't call this
	director->setAnimationInterval(1.0 / 60.0);


	// random　init
	const auto random = Random::getInstance();


    // create a scene. it's an autorelease object
    auto scene = SceneTitle::createScene();
	//auto scene = HelloWorld::createScene();
    // run
    director->runWithScene(scene);

    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();

    // if you use SimpleAudioEngine, it must be pause
   CocosDenshion::SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();

    // if you use SimpleAudioEngine, it must resume here
	CocosDenshion::SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
}


#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
#undef WINDOWS
#endif
