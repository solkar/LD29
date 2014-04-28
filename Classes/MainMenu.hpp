//
//  MainMenu.hpp
//  LD29
//
//  Created by Karlos Zafra on 25.4.2014.
//
//

#ifndef __LD29__MainMenu__
#define __LD29__MainMenu__

#include <iostream>
#include "cocos2d.h"
#include "extensions/cocos-ext.h"
#include "cocosbuilder/CocosBuilder.h"


class MainMenuLoader;

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
class MainMenu
: public cocos2d::Layer
, public cocosbuilder::CCBSelectorResolver
, public cocosbuilder::CCBMemberVariableAssigner
, public cocosbuilder::NodeLoaderListener
{
public:
    
    static cocos2d::Scene* createScene();
    
    CCB_STATIC_NEW_AUTORELEASE_OBJECT_WITH_INIT_METHOD(MainMenu, create);
    
    MainMenu();
    virtual ~MainMenu();
    
    
    virtual cocos2d::SEL_MenuHandler onResolveCCBCCMenuItemSelector(cocos2d::Ref * pTarget, const char * pSelectorName);
    virtual cocos2d::extension::Control::Handler onResolveCCBCCControlSelector(cocos2d::Ref * pTarget, const char * pSelectorName);
    virtual bool onAssignCCBMemberVariable(cocos2d::Ref * pTarget, const char * pMemberVariableName, cocos2d::Node * node);
    virtual void onNodeLoaded(cocos2d::Node * node, cocosbuilder::NodeLoader * nodeLoader);
    
    
private:
    
    void onPlayButton(Ref * sender, cocos2d::extension::Control::EventType pControlEvent);
    void onPlayClick(Ref * sender);
    
    virtual void update( float dt );
    
    
    
};

class MainMenuLoader : public cocosbuilder::LayerLoader {
public:
    CCB_STATIC_NEW_AUTORELEASE_OBJECT_METHOD(MainMenuLoader, loader);
    
protected:
    CCB_VIRTUAL_NEW_AUTORELEASE_CREATECCNODE_METHOD(MainMenu);
};
#endif /* defined(__LD29__MainMenu__) */
