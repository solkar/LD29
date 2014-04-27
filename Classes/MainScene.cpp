
//  MainScene.cpp
//  LD29
//
//  Created by Karlos Zafra on 25.4.2014.
//
//

#include "MainScene.hpp"
#include "HudLayer.hpp"
#include "GameFSM.hpp"
#include "GameState.hpp"

USING_NS_CC;
USING_NS_CC_EXT;
using namespace cocosbuilder;

Scene* MainScene::createScene()
{
    
    auto scene = Scene::create();
    
    
    /* Create an autorelease NodeLoaderLibrary. */
    NodeLoaderLibrary * ccNodeLoaderLibrary = NodeLoaderLibrary::newDefaultNodeLoaderLibrary();
    ccNodeLoaderLibrary->registerNodeLoader("HudLayer", HudLayerLoader::loader());
    ccNodeLoaderLibrary->registerNodeLoader("MainScene", MainSceneLoader::loader());
    
    /* Create an autorelease CCBReader. */
    cocosbuilder::CCBReader * ccbReader = new cocosbuilder::CCBReader(ccNodeLoaderLibrary);
//    ccbReader->autorelease();
    
    /* Read a ccbi file. */
    auto node = ccbReader->readNodeGraphFromFile("MainScene.ccbi");
    
    
    if(node != NULL) {
        scene->addChild(node);
    }
    
    ccbReader->release();
    
    return scene;
}


MainScene::MainScene()
{}

MainScene::~MainScene()
{

}


void MainScene::onNodeLoaded(cocos2d::Node * node,  cocosbuilder::NodeLoader * nodeLoader) {

    // game FSM
    mGameFsm = GameFSM::create();
    mGameFsm->setGameLayer( this );

    // init touch/mouse input
    this->initTouchControl();
    
    // load init map
    mTileMap = nullptr;
    this->loadMap( "ego-level1" );
    
    Point spawnCell = Point( 8 , 1 );
    
    // load player avatar
    mHero = Sprite::createWithSpriteFrameName("4-player-l.png");
    assert( mHero != nullptr );
    mHero->setPosition( this->positionForTileCoord( spawnCell ) ); //hero initial position
    mHero->setZOrder(0);
    mHero->setAnchorPoint( Point( 0.5 , 0 ) ) ;
    this->addChild( mHero, zHero ); 

    Point cell = tileCoordForPosition( mHero->getPosition() );
    CCLOG("Cell is %f:%f %f:%f", spawnCell.x, cell.x, spawnCell.y, cell.y );
    assert( cell.x == spawnCell.x && cell.y == spawnCell.y );

    this->scheduleUpdate();
}


SEL_MenuHandler MainScene::onResolveCCBCCMenuItemSelector(Ref * pTarget, const char * pSelectorName) {
    return NULL;
}

Control::Handler MainScene::onResolveCCBCCControlSelector(Ref * pTarget, const char * pSelectorName) {
    return NULL;
}

bool MainScene::onAssignCCBMemberVariable(Ref * pTarget, const char * pMemberVariableName, Node * pNode) {
//    CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "mComponent", Node *, this->mNode);
    return false;
}



Sprite* MainScene::getHero()
{
    assert( mHero != nullptr );
    return mHero; 
};
#pragma mark - Loop
void MainScene::update( float dt )
{
    
    mGameFsm->update( dt );
    
}

#pragma mark - TileMap
void MainScene::loadMap( std::string name)
{
   
    
    // clean previos map, if any
    if( mTileMap )
    {
        // save old layer data
        auto mapName = mTileMap->getProperty("name").asString();
        GameState::getInstance()->setPersistentMapData(mapName, mTileMap);
        
        // Don't clean up, we want persistent data
        // clean up and load new
//        mTileMap->removeAllChildrenWithCleanup(true);
        removeChild( mTileMap, true );
        mTileMap = NULL;
        
    }
    
    // load map from memory, or file
    mTileMap = GameState::getInstance()->getPersistentMapData(name);
    if( mTileMap == nullptr)
    {
        // load form file
        name = name.append(".tmx");
        mTileMap = TMXTiledMap::create( name );
    }
    
    // get map from file
    //mTileMap = TMXTiledMap::create( name );
    assert( mTileMap != nullptr );
    mTileMap->setAnchorPoint(Point(0,0));
    addChild(mTileMap, zMap); // hero is 0
    mTileSize = mTileMap->getTileSize();
    
    Size CC_UNUSED s = mTileMap->getContentSize();
    CCLOG("ContentSize: %f, %f", s.width,s.height);
    
    // save group data with the doors between maps: exits
    mExitObject = mTileMap->getObjectGroup("exits");
    if( mExitObject != nullptr)
    {
        // save spawn point positions, in "objects" group
        ValueMap dict = mExitObject->getObject("spawn");
        
        
        
        // get values and make a Point
        float x = dict["x"].asFloat();
        float y = dict["y"].asFloat();
        float width = dict["width"].asFloat();
        float height = dict["height"].asFloat();
        
        mSpawnPoint = Point( x + width * 0.5, y + height * 0.5 );
    }
    
    // load layer from memory
    //mCharacterLayer = GameState::getInstance()->getPersistentMapData(mTileMap->getProperty("name").asString());
    //if( mCharacterLayer == nullptr)
    //{
        //mCharacterLayer = mTileMap->getLayer("characters"); // load form file
    //}
    

    mCharacterLayer = mTileMap->getLayer("characters"); // load form file

    // save meta layer data
    mMetaLayer = mTileMap->getLayer("meta");
    mMetaLayer->setVisible( false );
    assert(mMetaLayer != nullptr);
    
    
}

Point MainScene::tileCoordForPosition(Point position) {

    float screenX = position.x - mTileMap->getMapSize().width * mTileSize.width * 0.5;
    float screenY = mTileMap->getMapSize().height * mTileSize.height - position.y;
    float TILE_WIDTH_HALF = mTileSize.width * 0.5;
    float TILE_HEIGHT_HALF = mTileSize.height * 0.5;

    int x = (screenX / TILE_WIDTH_HALF + screenY / TILE_HEIGHT_HALF) /2 - 1; // first pixel is 0
    int y = (screenY / TILE_HEIGHT_HALF -(screenX / TILE_WIDTH_HALF)) /2 - 1; // first pixel is 0
    return Point(x, y);
}

/**
 * Given a tile coordinate, returns the position on screen
 */
Point MainScene::positionForTileCoord(Point map) {

    float TILE_WIDTH_HALF = mTileSize.width * 0.5;
    float TILE_HEIGHT_HALF = mTileSize.height * 0.5;
    
    Point screen;
    screen.x = (map.x - map.y) * TILE_WIDTH_HALF + mTileMap->getMapSize().width * mTileSize.width * 0.5;
    screen.y = mTileMap->getMapSize().height * mTileSize.height - (map.x + map.y + 2) * TILE_HEIGHT_HALF ;

    
    return screen;
}

bool MainScene::tileIsCollidable( Point tile )
{
    int tileGid = mMetaLayer->getTileGIDAt(tile);
    if (tileGid) {
        auto collision = mTileMap->getPropertiesForGID(tileGid).asValueMap()["collidable"].asString();
        if ( collision.compare("true") == 0 ) {
            return true;
        }
    }

    return false;
}

bool MainScene::tileIsExit( Point tile )
{
    assert(mExitObject != nullptr);
    
    Point testPos = this->positionForTileCoord( tile );

    //Check if player ended up in an exit tile
    auto &exits = mExitObject->getObjects();
    for (auto& exit : exits)
    {
        ValueMap& dict = exit.asValueMap();
        
        float x = dict["x"].asFloat();
        float y = dict["y"].asFloat();
        float width = dict["width"].asFloat();
        float height = dict["height"].asFloat();
        
        int posx = x / width;
        int posy = ((mTileMap->getMapSize().height * height) - y) / (height) - 1;
        Point nonIsoCoor = Point(posx, posy);
        
        Point exitTile = this->tileCoordForPosition(Point(x, y));
        
        if (nonIsoCoor == tile) {
            auto name = dict["destination"].asString();
            Point heroPos = this->positionForTileCoord( Point( dict["startx"].asFloat() ,
                                  dict["starty"].asFloat() ) );
            
            return true;
        }

    }
    
    return false;
}

/**
 * Check if there's a mobile object 
 * and move it if possible
 */
bool MainScene::tileIsBlockedMobile( Point tile )
{

    // detect mobile presence
    if( this->tileHasProperty( tile, "mobile", mCharacterLayer ) == false )
        return false;
    

    //
    // decide is the mobile can move in the desired direction
    // get movement direction
    Point currentTile = this->tileCoordForPosition(mHero->getPosition() );
    Point nextTile = tile + tile - currentTile;
    // check next tile is empty 
    bool isNextTileFull = tileIsCollidable( nextTile );

    if(isNextTileFull){
        
        return true;
    }else{
        
        Sprite *mobileObject = mCharacterLayer->getTileAt(tile);
        //mobileObject->setAnchorPoint( Point( 0.5,0 ));
        
        // shift position of the mobile sprite
        auto shiftAction = Sequence::create(
                MoveBy::create(0.4, 
                    positionForTileCoord(nextTile)- positionForTileCoord(tile)),
                CallFunc::create( 
                    CC_CALLBACK_0( MainScene::swapGID, this,  mCharacterLayer, tile, nextTile)),
                NULL);
        

        mobileObject->runAction( shiftAction );
        

        
        return false;
    }



}

bool MainScene::tileHasProperty( Point tile , const std::string propertyName, TMXLayer* layer)
{
    
    int tileGid = layer->getTileGIDAt(tile);
    if (tileGid) {
        auto property = mTileMap->getPropertiesForGID(tileGid).asValueMap()[propertyName].asString();
        if ( property.compare("true") == 0 ) {
            return true;
        }
    }

}

std::string MainScene::getMapNameForExitInTile( Point tile )
{
    assert(mExitObject != nullptr);
    
    //Check if player ended up in an exit tile
    auto &exits = mExitObject->getObjects();
    for (auto& exit : exits)
    {
        ValueMap& dict = exit.asValueMap();
        
        float x = dict["x"].asFloat();
        float y = dict["y"].asFloat();
        float width = dict["width"].asFloat();
        float height = dict["height"].asFloat();
        
        int posx = x / width;
        int posy = ((mTileMap->getMapSize().height * height) - y) / (height) - 1;
        Point nonIsoCoor = Point(posx, posy);
        
        Point exitTile = this->tileCoordForPosition(Point(x, y));
        
        if (nonIsoCoor == tile) {
            auto name = dict["destination"].asString();
            //Point heroPos = this->positionForTileCoord( Point( dict["startx"].asFloat() ,
                                  //dict["starty"].asFloat() ) );
            
            return name;
        }

    }

    return "";

}



void MainScene::swapGID( TMXLayer* layer, Point oldTile, Point newTile )
{
        // swap tile Gid info
        auto oldGID = layer->getTileGIDAt( oldTile );
        auto newGID = layer->getTileGIDAt( newTile );
        mCharacterLayer->setTileGID( oldGID , newTile ); // update value on new tile
        mCharacterLayer->setTileGID( newGID , oldTile ); //erase old tile
}

#pragma mark - Controls
void MainScene::initTouchControl()
{
    auto listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(MainScene::onTouchBegan, this);
    listener->onTouchEnded = CC_CALLBACK_2(MainScene::onTouchEnded, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}
bool MainScene::onTouchBegan(Touch* touch, Event  *event)
{
    return true;
}

void MainScene::onTouchEnded(Touch* touch, Event  *event)
{

    auto touchLocation = touch->getLocation();
    //    touchLocation = Director::getInstance()->convertToGL(touchLocation);
    touchLocation = this->convertToNodeSpace(touchLocation);

    mGameFsm->onPlayerInput( touchLocation , mHero->getPosition(), mTileSize );




}
