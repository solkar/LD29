
//  MainScene.cpp
//  LD29
//
//  Created by Karlos Zafra on 25.4.2014.
//
//

#include "MainScene.hpp"
#include "HudLayer.hpp"
#include "GameFSM.hpp"

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
        mTileMap->removeAllChildrenWithCleanup(true);
        removeChild( mTileMap, true );
        mTileMap = NULL;
    }
    
    name = name.append(".tmx");
    
    // get map
    mTileMap = TMXTiledMap::create( name );
    assert( mTileMap != nullptr );
    mTileMap->setAnchorPoint(Point(0,0));
    addChild(mTileMap, zMap); // hero is 0
    mTileSize = mTileMap->getTileSize();
    
    Size CC_UNUSED s = mTileMap->getContentSize();
    CCLOG("ContentSize: %f, %f", s.width,s.height);
    
    // save group data with the doors between maps: exits
    mExitObject = mTileMap->getObjectGroup("exits");
    assert(mExitObject != nullptr);
    // save spawn point positions, in "objects" group
    ValueMap dict = mExitObject->getObject("spawn");
    
    
    // get values and make a Point
    float x = dict["x"].asFloat();
    float y = dict["y"].asFloat();
    float width = dict["width"].asFloat();
    float height = dict["height"].asFloat();
    
    mSpawnPoint = Point( x + width * 0.5, y + height * 0.5 );
    
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
    return false;
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
