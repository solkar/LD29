//
//  GameState.h
//

#ifndef _GameState_
#define _GameState_

#include "cocos2d.h"

class GameState : public cocos2d::Ref
{
    
public:
    /** Returns the shared instance of the Game Manager */
    static GameState* getInstance(void);

public:
    virtual ~GameState();
    bool init(void);
public:


    /**
     * Reproduce game music if not running.
     * If it's already playing does nothing
     */
    void playMusic();
    void stopMusic();

    void resetGame();
    void restartGame();
    
    cocos2d::TMXLayer* getPersistentLayerData( const std::string& );
    void setPersistentLayerData( const std::string& layerName, cocos2d::TMXLayer* );

    cocos2d::TMXTiledMap* getPersistentMapData( const std::string& mapName );
    void setPersistentMapData( const std::string& mapName, cocos2d::TMXTiledMap* map  );
    
private:
    cocos2d::Map<std::string, cocos2d::TMXLayer*> m_mLayerData;
    cocos2d::Map<std::string, cocos2d::TMXTiledMap*> m_mMapData;

};


#endif
