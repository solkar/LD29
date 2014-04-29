//
//  GameFSM.cpp
//  Zombie Tiles
//
//  Created by Karlos Zafra on 23.4.2014.
//
//

#include "GameFSM.hpp"
#include "MainScene.hpp"
#include "GameState.hpp"
#include "Macros.h"
#include "MapTransition.h"

#pragma mark - Init
GameFSM* GameFSM::create()
{
    GameFSM* pRet = new GameFSM();
    if (pRet && pRet->init())
    {
    
    }
    
    return pRet;
    
}

bool GameFSM::init()
{

    // create the FSM
    m_pBrain = new FSM();
    m_pBrain->pushState( std::bind( &GameFSM::initState, this ) );
    
    
    return true;
    
}

void GameFSM::setGameLayer( MainScene* layer )
{
    mGameLayer = layer; 
}

void GameFSM::update( float delta)
{
    m_pBrain->update( delta );
    
}

void GameFSM::setWaitAck( bool value )
{
    mWaitAck = value;
}

#pragma mark - States
void GameFSM::initState()
{
//    CCLOG("[FSM] Init state");

}


void GameFSM::waitPlayerAck()
{

    //TODO:
    
    if( mWaitAck )
    {
        mWaitAck = false;
        m_pBrain->popState();

    }
}

#pragma mark - Player input
void GameFSM::onPlayerInput( Point touchLocation, Point playerPos, Size tileSize )
{

    CCLOG("[FSM] Hanle player input");


    //Point diff = touchLocation - playerPos;
    Point diff = this->mGameLayer->tileCoordForPosition( touchLocation ) + Point( 1 , 1 ) 
        - this->mGameLayer->tileCoordForPosition( playerPos );
    
    CCLOG("Touch tile (%f,%f)", this->mGameLayer->tileCoordForPosition( touchLocation ).x +1 ,
          this->mGameLayer->tileCoordForPosition( touchLocation ).y + 1);
    CCLOG("Player tile (%f,%f)",this->mGameLayer->tileCoordForPosition( playerPos ).x,
          this->mGameLayer->tileCoordForPosition( playerPos ).y);
    CCLOG("Diff tile (%f,%f)",diff.x,diff.y);


    // ignore touch in the same tile the player is
    if( diff.x == 0 && diff.y == 0)
    {
        m_pBrain->popState();
        m_pBrain->pushState( CC_CALLBACK_0( GameFSM::initState , this ) );
        return;
    }
    
    // ignore diagonals
    if( abs( diff.x ) > 0 && abs( diff.y ) > 0)
    {
        m_pBrain->popState();
        m_pBrain->pushState( CC_CALLBACK_0( GameFSM::initState , this ) );
        return;
    }
    
    // hero only moves in one direction, one step each
        if (diff.x > 0) {
            playerPos.x += tileSize.width * 0.5;
            playerPos.y -= tileSize.height * 0.5;
        } else if( diff.x < 0){
            playerPos.x -= tileSize.width * 0.5;
            playerPos.y += tileSize.height * 0.5;
        }

        if (diff.y > 0) {
            playerPos.x -= tileSize.width * 0.5;
            playerPos.y -= tileSize.height * 0.5;
        } else if( diff.y < 0) {
            playerPos.x += tileSize.width * 0.5;
            playerPos.y += tileSize.height * 0.5;
        }
    

    //
    if (playerPos.x <= (tileSize.width * tileSize.width) &&
            playerPos.y <= (tileSize.height * tileSize.height) &&
            playerPos.y >= 0 &&
            playerPos.x >= 0 )
    {
        m_pBrain->pushState( CC_CALLBACK_0( GameFSM::checkCollision , this, playerPos ) );
//        this->setPlayerPosition(playerPos);
    }
}

void GameFSM::checkCollision( Point playerPos )
{
    CCLOG("[FSM] Check collisions");
    
	Point tileCoord = mGameLayer->tileCoordForPosition( playerPos );
    
    // clear text board
    mGameLayer->clearTextBoard();
    
    if( mGameLayer->tileIsCollidable( tileCoord ) ){
        m_pBrain->popState();
        m_pBrain->pushState( CC_CALLBACK_0( GameFSM::initState , this ) );
        return;
    }

    if( mGameLayer->tileIsBlockedMobile( tileCoord ) )
    {
        // mobile is blocked player can't go on
        m_pBrain->popState();
        m_pBrain->pushState( CC_CALLBACK_0( GameFSM::initState , this ) );
        return;

    }

    if( mGameLayer->tileIsExit( tileCoord ) ){

        std::string destinationMap = this->mGameLayer->getMapNameForExitInTile( tileCoord );

        m_pBrain->popState();
        m_pBrain->pushState( CC_CALLBACK_0( GameFSM::initState, this ) );
        m_pBrain->pushState( CC_CALLBACK_0( GameFSM::spawnPlayerAction , this ) );
        m_pBrain->pushState( CC_CALLBACK_0( GameFSM::loadMap , this, destinationMap ) );
        //m_pBrain->pushState( CC_CALLBACK_0( GameFSM::fadePlayerAction , this ) );
        return;
    }

    if( mGameLayer->tileIsSwitch( tileCoord ) ){

        // enable rock
        //mGameLayer->enableRockAtEgo();
        mGameLayer->enableSwitchAt( tileCoord );

        mGameLayer->enableTextBoardAt( tileCoord );

        mGameLayer->enableLockAt( tileCoord );
    }

    
    if( mGameLayer->tileHasProperty( tileCoord, "collectible", mGameLayer->mCharacterLayer ) )
    {
        // remove key from the map
        
        GameState::getInstance()->setPlayerHasKey( true );
        mGameLayer->removeKeyFromMap( tileCoord );
            
    }
    
    m_pBrain->popState();
    m_pBrain->pushState( CC_CALLBACK_0( GameFSM::initState, this ) );
    m_pBrain->pushState( CC_CALLBACK_0( GameFSM::movePlayerAction , this , playerPos ) );

}

void GameFSM::movePlayerAction( Point screenPosition )
{
    CCLOG("[FSM] Move player");


    // disable new input until player reaches next tile
    //mCanWalk = false;
    
    // Animate the player
    auto moveAction = Sequence::create(
                                       MoveTo::create( PLAYER_SPEED ,screenPosition),
                                       //CallFunc::create( CC_CALLBACK_0(GameScene::heroFinishedWalking,this)),
                                       NULL);
    
    // Play actions
    if( mGameLayer->getHero()->getNumberOfRunningActions() == 0 )
    {
        mGameLayer->getHero()->runAction(moveAction);
        //mHero->setPosition( position );
    }
    

    //   finished
    m_pBrain->popState();
}

void GameFSM::loadMap(const std::string mapName )
{
    CCLOG("[FSM] load map");
    
    this->mGameLayer->loadMap(mapName);
    
    Director::getInstance()->pushScene( MapTransition::createScene());
    
    // forward to idle state
    m_pBrain->popState();
}

void GameFSM::spawnPlayerAction()
{

    this->mGameLayer->setPlayerInSpawnPoint();

    m_pBrain->popState();
}
