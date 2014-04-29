//
//  MapTransition.cpp
//  LD29
//
//  Created by Karlos Zafra on 29.4.2014.
//
//

#include "MapTransition.h"

cocos2d::Scene* MapTransition::createScene()
{
    
    auto scene = Scene::create();
    
    
    // 'layer' is an autorelease object
    auto layer = MapTransition::create();
        scene->addChild(layer);
    
    auto background = cocos2d::Sprite::create("transicion.png");
    background->setAnchorPoint( Point( 0 , 0));
    layer->addChild(background);
    
    
    //    ccbReader->release();
    
    return scene;
}

bool MapTransition::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    
    this->runAction( Sequence::create(
                                       DelayTime::create(0.5),
                                       CallFunc::create( CC_CALLBACK_0( MapTransition::goBack, this)),
                                       NULL));
    return true;
    
}

void MapTransition::goBack()
{
    Director::getInstance()->popScene();
}
