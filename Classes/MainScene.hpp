//
//  MainScene.hpp
//  LD29
//
//  Created by Karlos Zafra on 25.4.2014.
//
//

#ifndef __LD29__MainScene__
#define __LD29__MainScene__

#include <iostream>
#include "cocos2d.h"
#include "extensions/cocos-ext.h"
#include "cocosbuilder/CocosBuilder.h"

enum{
    zBackground=-1,
    zMap,
    zHero,
    zHud,
};

USING_NS_CC;

class MainSceneLoader;
class GameFSM;

/*
 * Note: for some pretty hard fucked up reason, the order of inheritance is important!
 * When Layer is the 'first' inherited object:
 * During runtime the method call to the (pure virtual) 'interfaces' fails jumping into a bogus method or just doing nothing:
 *  #0    0x000cf840 in non-virtual thunk to HelloCocos....
 *  #1    ....
 *
 * This thread describes the problem:
 * http://www.cocoabuilder.com/archive/xcode/265549-crash-in-virtual-method-call.html
 */
class MainScene
: public cocos2d::Layer
, public cocosbuilder::CCBSelectorResolver
, public cocosbuilder::CCBMemberVariableAssigner
, public cocosbuilder::NodeLoaderListener
{
public:
    
    static cocos2d::Scene* createScene();
    
    CCB_STATIC_NEW_AUTORELEASE_OBJECT_WITH_INIT_METHOD(MainScene, create);
    
    MainScene();
    virtual ~MainScene();
    
    
    virtual cocos2d::SEL_MenuHandler onResolveCCBCCMenuItemSelector(cocos2d::Ref * pTarget, const char * pSelectorName);
    virtual cocos2d::extension::Control::Handler onResolveCCBCCControlSelector(cocos2d::Ref * pTarget, const char * pSelectorName);
    virtual bool onAssignCCBMemberVariable(cocos2d::Ref * pTarget, const char * pMemberVariableName, cocos2d::Node * node);
    virtual void onNodeLoaded(cocos2d::Node * node, cocosbuilder::NodeLoader * nodeLoader);
    
    // tilemap helpers
    Point tileCoordForPosition(Point position);
    Point positionForTileCoord(Point position);
    bool tileIsCollidable( Point tile );
    bool tileIsExit( Point tile );
    bool tileIsBlockedMobile( Point tile );
    bool tileIsSwitch( Point tile );
    std::string getMapNameForExitInTile( Point tile );

    bool tileHasProperty( Point tile, const std::string property , TMXLayer* layer);
    void swapGID( TMXLayer* layer, Point oldTile, Point newTile );
    void copyGID( TMXLayer* originLayer, TMXLayer* destinationLayer );
    
    Sprite* getHero();
    void loadMap( const std::string& name );
    void setPlayerInSpawnPoint();

    void enableRockAtEgo();

    // switch tiles
    void enableSwitchAt( Point tile );
    void enableTextBoardAt( Point tile );
    void enableLockAt( const Point& tile );

    void clearTextBoard();

    cocos2d::TMXLayer *mCharacterLayer;

    void removeKeyFromMap( const Point& tile );
private:
    
    void initTouchControl();
    
    virtual void update( float dt );
    
    
    cocos2d::TMXTiledMap * mTileMap;
    cocos2d::TMXLayer *mMetaLayer;
    cocos2d::TMXLayer *mDoorLayer;
    cocos2d::Size mTileSize;
    cocos2d::TMXObjectGroup *mExitObject;
    cocos2d::Point mSpawnPoint;
    cocos2d::Label* mTopLabel;
    cocos2d::extension::Scale9Sprite* mTopBackground;
    
    cocos2d::Sprite* mHero;
    

    bool onTouchBegan(Touch* touch, Event  *event);
    void onTouchEnded(Touch* touch, Event  *event);

    GameFSM *mGameFsm;
    

    Point getSpawnTile();
    Sprite* mBackground;

    void updateSwitches( TMXLayer* layer );
    
    ////
    // Controls
public:

private:
    bool m_bTouchOn;
    Point m_ptLastTouchPosition;
    Point m_ptHeroReference;
    void onPlayerStop();
};

class MainSceneLoader : public cocosbuilder::LayerLoader {
public:
    CCB_STATIC_NEW_AUTORELEASE_OBJECT_METHOD(MainSceneLoader, loader);
    
protected:
    CCB_VIRTUAL_NEW_AUTORELEASE_CREATECCNODE_METHOD(MainScene);

};
#endif /* defined(__LD29__MainScene__) */
