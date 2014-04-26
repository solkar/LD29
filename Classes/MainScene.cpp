
//  MainScene.cpp
//  LD29
//
//  Created by Karlos Zafra on 25.4.2014.
//
//

#include "MainScene.hpp"
#include "HudLayer.hpp"

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
    
    // load init map
    this->loadMap( "ego-level1" );
    

//    this->scheduleUpdate();
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



#pragma mark - Loop
void MainScene::update( float dt )
{
    
//    GameManager::getInstance()->getMissionFsm()->update( dt );
    
}

#pragma mark - Map
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
    mTileSize = mTileMap->getTileSize().width;
    
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

