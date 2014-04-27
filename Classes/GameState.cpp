//
//  GameState.cpp
//
//

#include "GameState.hpp"
#include "SimpleAudioEngine.h"
#include "MainMenu.hpp"

USING_NS_CC;
using namespace CocosDenshion;

static GameState *_sharedGameState = NULL;

GameState* GameState::getInstance()
{
    if (! _sharedGameState)
    {
        _sharedGameState = new GameState();
        _sharedGameState->init();
    }
    
    return _sharedGameState;
}

GameState::~GameState(void)
{
}

bool GameState::init(void)
{
    // preload background music and effect
//    SimpleAudioEngine::getInstance()->preloadBackgroundMusic( MUSIC_FILE );
//    SimpleAudioEngine::getInstance()->preloadEffect( SOME_EFFECT );

    // set default volume
//    SimpleAudioEngine::getInstance()->setEffectsVolume(0.5);
//    SimpleAudioEngine::getInstance()->setBackgroundMusicVolume(0.35);


    return true;
}

void GameState::resetGame()
{

}


void GameState::playMusic()
{
#ifdef NO_MUSIC
    return;
#endif
    
    // play music
    if (SimpleAudioEngine::getInstance()->isBackgroundMusicPlaying())
    {
            CCLOG("background music is playing");

    }    else{

            CCLOG("background music is not playing");
//        SimpleAudioEngine::getInstance()->playBackgroundMusic(MUSIC_FILE, true);
    }
}

void GameState::stopMusic()
{
    SimpleAudioEngine::getInstance()->stopBackgroundMusic();
}


void GameState::restartGame()
{

    Director::getInstance()->replaceScene( MainMenu::createScene() );

}

//void GameState::setGameFsm( GameFSM *ptr ) { mGameFsm = ptr; }

//cocos2d::TMXLayer* GameState::getPersistentMapData( std::string mapName)
//{
//    return m_mLayerData.at( mapName );
//}
//void GameState::setPersistentMapData( std::string mapName, cocos2d::TMXLayer* layer )
//{
//    m_mLayerData.insert( mapName , layer );
//}

cocos2d::TMXTiledMap* GameState::getPersistentMapData(const std::string& mapName)
{
    TMXTiledMap *mapData = nullptr;
    mapData = (TMXTiledMap *)m_mMapData.at( mapName );
    return mapData;
}
void GameState::setPersistentMapData(const std::string& mapName, cocos2d::TMXTiledMap* map )
{
    m_mMapData.insert( mapName , map );
}

