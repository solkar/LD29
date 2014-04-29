//
//  FSM.h
//  SamuraiSlash
//
//  Created by Karlos Zafra on 18.4.2014.
//
//

#ifndef __SamuraiSlash__FSM__
#define __SamuraiSlash__FSM__

#include <iostream>
#include <functional>
#include <vector>


typedef std::function<void( float )> FsmState;

class FSM {
public:


    void update( float delta);
    
//    void setState(std::function<void()>);
    FsmState popState();
    void pushState( FsmState );
    FsmState getCurrentState();
    
private:
    FsmState m_pActiveState;
    
    std::vector<FsmState> m_stack;
   

};

#endif /* defined(__SamuraiSlash__FSM__) */
