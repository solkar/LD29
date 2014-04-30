//
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
#include "Macros.h"

//#define FIRST_MAP "ego-level1"

//#define FAKE_STONE // ego-l1

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

    mGameFsm->setPlayerStopCallback(CC_CALLBACK_0( MainScene::onPlayerStop, this ) );

    // init touch/mouse input
    this->initTouchControl();
    
    // add background
    mBackground = Sprite::create("bkg-ego.png");
    mBackground->setAnchorPoint( Point(0,0) );
    addChild( mBackground, zBackground );

    // load init map
    mTileMap = nullptr;
    //this->loadMap("ego-level1");
    this->loadMap( FIRST_MAP ); // NG, can't pass a constant
    //this->loadMap( "hall1" );
    //this->loadMap( "superEgo-level1" );
   
    // player has no key
    GameState::getInstance()->setPlayerHasKey( false );
    
    Point spawnCell =  this->getSpawnTile();
    
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
    CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "mTopLabel", Label *, this->mTopLabel);
    CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "mTopBackground", Scale9Sprite *, this->mTopBackground);
    return false;
}



Sprite* MainScene::getHero()
{
    assert( mHero != nullptr );
    return mHero; 
};

#pragma mark - Text Board
void MainScene::clearTextBoard()
{
    mTopLabel->setString("");
    mTopBackground->setVisible( false );

}
#pragma mark - Loop
void MainScene::update( float dt )
{
    
    if( m_bTouchOn )
    {
        CCLOG(" call player input");
        mGameFsm->onPlayerInput( m_ptLastTouchPosition , m_ptHeroReference , mTileSize );
    }

    mGameFsm->update( dt );
    
}

#pragma mark - TileMap
void MainScene::loadMap( const std::string& name)
{
   
    
    // clean previos map, if any
    if( mTileMap )
    {
        

        // save old map data
        auto mapName = mTileMap->getProperty("name").asString();
        GameState::getInstance()->setPersistentMapData(mapName, mTileMap);

        //save character layer in ego level
        if( mapName.compare("ego-level1") == 0 )
        {
            auto layer = mTileMap->getLayer("characters");
            GameState::getInstance()->setPersistentLayerData("ego-character", layer );

        }
        
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
        std::string fileName;
        fileName = fileName.append(name + ".tmx");
        mTileMap = TMXTiledMap::create( fileName );
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
    
    
    /** Id level mirrors objects in Ego level **/
    if(mTileMap->getProperty("name").asString().compare("id-level1") != 0 )
    {
        mCharacterLayer = mTileMap->getLayer("characters"); // load this layer
    }else{
        
        /** duplicate layer properties **/
         mCharacterLayer = mTileMap->getLayer("characters"); // load from TileMap
        
        // get layer from ego level
        auto egoLayer = GameState::getInstance()->getPersistentLayerData("ego-character");
        this->copyGID( egoLayer , mCharacterLayer );

        this->updateSwitches( mCharacterLayer );
    }


    // save meta layer data
    mMetaLayer = mTileMap->getLayer("meta");
    mMetaLayer->setVisible( false );
    assert(mMetaLayer != nullptr);
    
    // save doors layer
    mDoorLayer = mTileMap->getLayer("doors");
    
    // change background
    auto mapType = mTileMap->getProperty("psychelevel").asString();
    std::string bckFileName = "bkg-";
    
//    if( mapType.compare("superego") == 0 ){
//        bckFileName="bkg-superego";
//    }else if( mapType.compare("id") == 0 ){
//        bckFileName="bkg-superegoid";
//    }else{
//         bckFileName="bkg-ego";
//    }
    bckFileName = bckFileName.append(mapType + ".png");
    
    // update sprite frame
    mBackground->setSpriteFrame(Sprite::create(bckFileName)->getSpriteFrame());


#ifdef FAKE_STONE
    if( mTileMap->getProperty("name").asString().compare("ego-level1") == 0 )
    {
        mCharacterLayer->setTileGID( GID_ROCK, Point(5,5) );
    }
#endif
    
}

void MainScene::setPlayerInSpawnPoint()
{
    Point spawnCell =  this->getSpawnTile();
    
    // load player avatar
    mHero->setPosition( this->positionForTileCoord( spawnCell ) ); //hero initial position

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
    
    int cTileGid = mCharacterLayer->getTileGIDAt(tile);
    if (cTileGid) {
        auto collision = mTileMap->getPropertiesForGID(cTileGid).asValueMap()["collidable"].asString();
        if ( collision.compare("true") == 0 ) {
            return true;
        }
    }
    
    int dTileGid = mTileMap->getLayer("doors")->getTileGIDAt(tile);
    if (dTileGid) {
        auto collision = mTileMap->getPropertiesForGID(cTileGid).asValueMap()["collidable"].asString();
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
        
        // ignore if not exit
        auto name = dict["name"].asString();
        if( name.compare("exit") != 0)
            continue;



        int posx = x / width;
        int posy = ((mTileMap->getMapSize().height * height) - y) / (height) - 1;
        Point nonIsoCoor = Point(posx, posy);
        
        Point exitTile = this->tileCoordForPosition(Point(x, y));
        
        if (nonIsoCoor == tile) {
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
                MoveBy::create(PLAYER_SPEED, 
                    positionForTileCoord(nextTile)- positionForTileCoord(tile)),
                CallFunc::create( 
                    CC_CALLBACK_0( MainScene::swapGID, this,  mCharacterLayer, tile, nextTile)),
                NULL);
        

        mobileObject->runAction( shiftAction );
        

        
        return false;
    }



}

bool MainScene::tileIsSwitch( Point tile )
{
    bool tileIsSwitch = tileHasProperty( tile, "switch", mMetaLayer );
    
    // detect path at Super Ego
    if( tileIsSwitch == true )
    {
        CCLOG("[%f,%f] Switch stepped", tile.x , tile.y );

        if( tile == Point( 7, 8 ) 
                && mTileMap->getProperty("name").asString().compare("superEgo-level1") == 0 )
        {

            CCLOG("[%f,%f] Switch path active", tile.x , tile.y );
            GameState::getInstance()->setSwitchPathSize( 0 );
            GameState::getInstance()->setSwitchPath( true );
        }

        int size = GameState::getInstance()->getSwitchPathSize( ) + 1;
        GameState::getInstance()->setSwitchPathSize( size );

#ifdef SWITCH_FEEDBACK
        // show enabled switch feedback
        mTileMap->getLayer("floor-deco")->setTileGID( GID_SLABGREEN, tile );
#endif
        
    }else{
            CCLOG("[%f,%f] Switch path corrupted", tile.x , tile.y );

            GameState::getInstance()->setSwitchPath( false );
            GameState::getInstance()->setSwitchPathSize( 0 );

    }

  return tileIsSwitch;
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
    
    return false;
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

/**
 * Copies rock origin layer (at Ego level) into the same tile at destination
 * layer (at Id level)
 */
void MainScene::copyGID( TMXLayer* originLayer, TMXLayer* destinationLayer )
{
    
    for (int i = 0; i < mTileMap->getMapSize().width; i++) {
        for (int j = 0; j < mTileMap->getMapSize().height; j++) {
            // remove old rocks in target layer
            auto destinationData = destinationLayer->getTileGIDAt(Point(i,j));
            if( destinationData == GID_ROCK ){
                destinationLayer->setTileGID(GID_EMPTY, Point(i, j));
            }

            
            // read from origin layer and update
            auto originData = originLayer->getTileGIDAt(Point( i, j));
            if( originData == GID_ROCK){
                destinationLayer->setTileGID(GID_ROCK, Point(i, j));
                auto sprite = destinationLayer->getTileAt(Point(i,j));
                sprite->setOpacity(168);
                

            }
        }

    }
        
}

void MainScene::enableRockAtEgo()
{
    // get layer from memory
    auto layer =  GameState::getInstance()->getPersistentLayerData("ego-character"); 
    layer->setTileGID( GID_ROCK, Point( 7 , 7 ) );

    // save data
    GameState::getInstance()->setPersistentLayerData( "ego-character", layer );
    


}

void MainScene::enableSwitchAt( Point switchTile )
{
    assert(mExitObject != nullptr);
    
    //Check if player ended up in an exit tile
    auto &exits = mExitObject->getObjects();
    for (auto& exit : exits)
    {
        ValueMap& dict = exit.asValueMap();

        // discard not switch objects
        if( dict["name"].asString().compare("switch") != 0 )
            continue;

        // discard wrong position
        float x = dict["x"].asFloat();
        float y = dict["y"].asFloat();
        float width = dict["width"].asFloat();
        float height = dict["height"].asFloat();
        
        int posx = x / width;
        int posy = ((mTileMap->getMapSize().height * height) - y) / (height) - 1;
        Point nonIsoCoor = Point(posx, posy);
        
        if( nonIsoCoor != switchTile )
            continue;
        

        // special swith at S.Ego level
        if( switchTile == Point( 5 , 4 )
                && mTileMap->getProperty("name").asString().compare("superEgo-level1") == 0 )
        {
            if( GameState::getInstance()->getSwitchPathSize() != 7 )
            {
                // path corrupted
                mTopLabel->setString("It's not working");
                mTopBackground->setVisible(true);
                return;

            }else{
                // feedback
                mTopLabel->setString("Guilt rock dropped");
                mTopBackground->setVisible(true);
                
                
                //
                // change switch sprite to enabled
                
                // get floor layer
                auto floorLayer = mTileMap->getLayer("floor");
                // change switch from off to on
                floorLayer->setTileGID(GID_SWITCHON, switchTile );
                
            }

        }

        CCLOG("[%i,%i] Switch enabled", posx, posy);
        
        //
        // add object specified in the object
        // get data from switch object
        auto layerName = dict["targetLayer"].asString();
        int GIDValue = dict["GID"].asInt();
        Point targetTile = Point( dict["targetx"].asInt(), dict["targety"].asInt() );

        // get layer from memory
        auto layer =  GameState::getInstance()->getPersistentLayerData( layerName );
        assert( layer != nullptr);
        layer->setTileGID( GIDValue , targetTile );

        // save data
        GameState::getInstance()->setPersistentLayerData( layerName , layer );
        


    }

}

void MainScene::enableTextBoardAt( Point switchTile )
{
    assert(mExitObject != nullptr);
    
    //Check if player ended up in an exit tile
    auto &exits = mExitObject->getObjects();
    for (auto& exit : exits)
    {
        ValueMap& dict = exit.asValueMap();

        // discard not textboard objects
        if( dict["name"].asString().compare("textboard") != 0 )
            continue;

        // discard wrong position
        float x = dict["x"].asFloat();
        float y = dict["y"].asFloat();
        float width = dict["width"].asFloat();
        float height = dict["height"].asFloat();
        
        int posx = x / width;
        int posy = ((mTileMap->getMapSize().height * height) - y) / (height) - 1;
        Point nonIsoCoor = Point(posx, posy);
        
        if( nonIsoCoor != switchTile )
            continue;
        
        // get text from switch object
        auto text = dict["text"].asString();

        // show text
        mTopLabel->setString( text );
        mTopBackground->setVisible( true );
        

    }

}

void MainScene::enableLockAt( const Point& switchTile )
{
    assert(mExitObject != nullptr);
    
    //Check if player ended up in an exit tile
    auto &exits = mExitObject->getObjects();
    for (auto& exit : exits)
    {
        ValueMap& dict = exit.asValueMap();

        // discard not lock objects
        if( dict["name"].asString().compare("lock") != 0 )
            continue;

        // ignore lock in other positions
        float x = dict["x"].asFloat();
        float y = dict["y"].asFloat();
        float width = dict["width"].asFloat();
        float height = dict["height"].asFloat();
        
        int posx = x / width;
        int posy = ((mTileMap->getMapSize().height * height) - y) / (height) - 1;
        Point nonIsoCoor = Point(posx, posy);
        
        if( nonIsoCoor != switchTile )
            continue;
        

        // get name of the key that opens the door
        auto keyName = dict["keyName"].asString();

        if( GameState::getInstance()->getPlayerHasKey() )
        {

            // get door tile and exit tiles from the lock object
            auto doorTile = Point(dict["doorx"].asInt(),
                    dict["doory"].asInt());
            auto exitTile = Point(dict["exitx"].asInt(),
                    dict["exity"].asInt());

            // GID to open door
            mDoorLayer->setTileGID(GID_L2_OPENDOOR, doorTile);
            
            // change meta from collidable to exit
            mMetaLayer->setTileGID(GID_METAEXIT, exitTile );
            
            // change object prop. open to true: ?
         
            
            //TODO: play door open fx

        }
        

    }

    // update changed objects
//    mExitObject->setObjects(exits);
}
Point MainScene::getSpawnTile()
{

    //Check if player ended up in an exit tile
    auto &exits = mExitObject->getObjects();
    for (auto& exit : exits)
    {
        ValueMap& dict = exit.asValueMap();
        
        // ignore if name of the object is not "spawn"
        auto name = dict["name"].asString();
        if( name.compare("spawn") != 0)
            continue;
        
        // get coordinates of the object
        float x = dict["x"].asFloat();
        float y = dict["y"].asFloat();
        float width = dict["width"].asFloat();
        float height = dict["height"].asFloat();

        // get position, work only with orthogonal
        int posx = x / width;
        int posy = ((mTileMap->getMapSize().height * height) - y) / (height) - 1;
        Point orthoTile = Point(posx, posy);
        return orthoTile;

    }

}


void MainScene::removeKeyFromMap( const Point& tile)
{
    mCharacterLayer->setTileGID(GID_EMPTY, tile );

}

void MainScene::updateSwitches( TMXLayer * layer )
{
    
    // hard coded
    Point idl1SwitchTile = Point( 4 , 5 );
   if( layer->getTileGIDAt( idl1SwitchTile ) == GID_ROCK )
   {
       // get floor layer
       auto floorLayer = mTileMap->getLayer("floor");
       
       // change switch from off to on
       floorLayer->setTileGID(GID_SWITCHON, idl1SwitchTile );
       
       // change door to open
       mTileMap->getLayer("doors")->setTileGID(GID_CHAOSDOOR_L_OPEN, Point(2,3));
       
       mTileMap->getLayer("meta")->setTileGID(GID_EMPTY, Point(2,2) );

   }


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
    
    m_bTouchOn = true;

    auto touchLocation = touch->getLocation();
    //    touchLocation = Director::getInstance()->convertToGL(touchLocation);
    touchLocation = this->convertToNodeSpace(touchLocation);

    m_ptLastTouchPosition = touchLocation;
    m_ptHeroReference = mHero->getPosition();

    mGameFsm->onPlayerInput( touchLocation , m_ptHeroReference, mTileSize );

    return true;
}

void MainScene::onTouchEnded(Touch* touch, Event  *event)
{
    m_bTouchOn = false;
}

void MainScene::onPlayerStop()
{
    m_bTouchOn = false;
}

