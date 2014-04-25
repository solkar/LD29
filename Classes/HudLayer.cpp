//
//  HudLayer.cpp
//  LD29
//
//  Created by Karlos Zafra on 25.4.2014.
//
//

#include "HudLayer.hpp"

//#include "GameManager.h"
#include "MainScene.hpp"

USING_NS_CC;
USING_NS_CC_EXT;
using namespace cocosbuilder;

Scene* HudLayer::createScene()
{
    
    auto scene = Scene::create();
    
    
    /* Create an autorelease NodeLoaderLibrary. */
    NodeLoaderLibrary * ccNodeLoaderLibrary = NodeLoaderLibrary::newDefaultNodeLoaderLibrary();
    ccNodeLoaderLibrary->registerNodeLoader("HudLayer", HudLayerLoader::loader());
    
    /* Create an autorelease CCBReader. */
    cocosbuilder::CCBReader * ccbReader = new cocosbuilder::CCBReader(ccNodeLoaderLibrary);
    ccbReader->autorelease();
    
    /* Read a ccbi file. */
    auto node = ccbReader->readNodeGraphFromFile("HudLayer.ccbi");
    
    
    if(node != NULL) {
        scene->addChild(node);
    }
    
//    ccbReader->release();
    
    return scene;
}


HudLayer::HudLayer()
{}

HudLayer::~HudLayer()
{

}


void HudLayer::onNodeLoaded(cocos2d::Node * node,  cocosbuilder::NodeLoader * nodeLoader) {
    
//    this->scheduleUpdate();
}


SEL_MenuHandler HudLayer::onResolveCCBCCMenuItemSelector(Ref * pTarget, const char * pSelectorName) {
//    CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this,"onMenuItemClick",HudLayer::onMenuItemClick);
    return NULL;
}

Control::Handler HudLayer::onResolveCCBCCControlSelector(Ref * pTarget, const char * pSelectorName) {
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(this, "onPlayButton", HudLayer::onPlayButton);
    return NULL;
}

bool HudLayer::onAssignCCBMemberVariable(Ref * pTarget, const char * pMemberVariableName, Node * pNode) {
//    CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "mComponent", Node *, this->mNode);
    return false;
}


//void HudLayer::onMenuItemClick(Ref * sender){
//
//    GameManager::getInstance()->getMissionFsm()->setWaitAck( true );
//}

void HudLayer::onPlayButton(Ref * sender, Control::EventType pControlEvent)
{
    Director::getInstance()->replaceScene(MainScene::createScene());
}

#pragma mark - Loop
void HudLayer::update( float dt )
{
    
//    GameManager::getInstance()->getMissionFsm()->update( dt );
    
}
