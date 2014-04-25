//
//  MainMenu.cpp
//  LD29
//
//  Created by Karlos Zafra on 25.4.2014.
//
//

#include "MainMenu.hpp"

//#include "GameManager.h"
#include "MainScene.hpp"

USING_NS_CC;
USING_NS_CC_EXT;
using namespace cocosbuilder;

Scene* MainMenu::createScene()
{
    
    auto scene = Scene::create();
    
    
    /* Create an autorelease NodeLoaderLibrary. */
    NodeLoaderLibrary * ccNodeLoaderLibrary = NodeLoaderLibrary::newDefaultNodeLoaderLibrary();
    ccNodeLoaderLibrary->registerNodeLoader("MainMenu", MainMenuLoader::loader());
    
    /* Create an autorelease CCBReader. */
    cocosbuilder::CCBReader * ccbReader = new cocosbuilder::CCBReader(ccNodeLoaderLibrary);
    ccbReader->autorelease();
    
    /* Read a ccbi file. */
    auto node = ccbReader->readNodeGraphFromFile("MainMenu.ccbi");
    
    
    if(node != NULL) {
        scene->addChild(node);
    }
    
//    ccbReader->release();
    
    return scene;
}


MainMenu::MainMenu()
{}

MainMenu::~MainMenu()
{

}


void MainMenu::onNodeLoaded(cocos2d::Node * node,  cocosbuilder::NodeLoader * nodeLoader) {
    
//    this->scheduleUpdate();
}


SEL_MenuHandler MainMenu::onResolveCCBCCMenuItemSelector(Ref * pTarget, const char * pSelectorName) {
//    CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this,"onMenuItemClick",MainMenu::onMenuItemClick);
    return NULL;
}

Control::Handler MainMenu::onResolveCCBCCControlSelector(Ref * pTarget, const char * pSelectorName) {
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(this, "onPlayButton", MainMenu::onPlayButton);
    return NULL;
}

bool MainMenu::onAssignCCBMemberVariable(Ref * pTarget, const char * pMemberVariableName, Node * pNode) {
//    CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "mComponent", Node *, this->mNode);
    return false;
}


//void MainMenu::onMenuItemClick(Ref * sender){
//
//    GameManager::getInstance()->getMissionFsm()->setWaitAck( true );
//}

void MainMenu::onPlayButton(Ref * sender, Control::EventType pControlEvent)
{
    Director::getInstance()->replaceScene(MainScene::createScene());
}

#pragma mark - Loop
void MainMenu::update( float dt )
{
    
//    GameManager::getInstance()->getMissionFsm()->update( dt );
    
}
