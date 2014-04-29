//
//  FSM.cpp
//  SamuraiSlash
//
//  Created by Karlos Zafra on 18.4.2014.
//
//

#include "FSM.h"


void FSM::update( float delta )
{
    FsmState currentState = getCurrentState();
    
    if( currentState != nullptr)
    {
        currentState( delta );
    }

}

//void FSM::setState(std::function<void()> callback)
//{
//    m_pActiveState = callback;
//}

FsmState FSM::popState()
{
    FsmState state = m_stack.back();
    m_stack.pop_back();
    
    return state;
}
void FSM::pushState( FsmState newState)
{
    m_stack.push_back( newState );
}
FsmState FSM::getCurrentState()
{
    return m_stack.size() > 0 ? m_stack.back() : nullptr;
}




