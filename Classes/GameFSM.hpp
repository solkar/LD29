//
//  GameFSM.h
//  Zombie Tiles
//
//  Created by Karlos Zafra on 23.4.2014.
//
//

#ifndef __LD29__GameFSM__
#define __LD29__GameFSM__

#include "FSM.h"
#include "cocos2d.h"

USING_NS_CC;

class MainScene;

class GameFSM {
    
   
public:
    static GameFSM* create();
    bool init();
    void setGameLayer( MainScene* layer );
    
    void update( float delta );
    
    void setWaitAck( bool value );

    ////
    // Handle player input
public:
    void onPlayerInput(const Point& touchLocation, const Point& playerPos, Size tileSize );
    void setPlayerStopCallback(const std::function<void()>& func);
private:
    std::function<void()> m_fPlayerStopCallback;
    void playerStopCallback();

private:
     FSM *m_pBrain;

    void initState();


    // States
    void checkCollision( Point playerPos );
    void movePlayerAction( Point screenPosition );



    // Actions, state that wont push forward to anothe state
    void wait( float delta );

    void waitPlayerAck();
    bool mWaitAck;
    
    void loadMap(const std::string mapName );
    void spawnPlayerAction();

    void interrupTouch();

    // Game layer state
    MainScene* mGameLayer;

    bool m_bIdle;
};

#endif /* defined(__LD29__GameFSM__) */
