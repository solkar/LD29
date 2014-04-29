//
//  MapTransition.h
//  LD29
//
//  Created by Karlos Zafra on 29.4.2014.
//
//

#ifndef __LD29__MapTransition__
#define __LD29__MapTransition__
#include <iostream>
#include "cocos2d.h"

USING_NS_CC;

class MapTransition
: public cocos2d::Layer
{
public:
    CREATE_FUNC( MapTransition);
    static cocos2d::Scene* createScene();
    virtual bool init();
private:
    void goBack();
};
#endif /* defined(__LD29__MapTransition__) */
