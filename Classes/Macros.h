//
//  Macros.h
//  LD29
//
//  Created by Karlos Zafra on 25.4.2014.
//
//

#ifndef LD29_Macros_h
#define LD29_Macros_h

#define random_range(low,high) (arc4random()%(high-low+1))+low
#define frandom (float)arc4random()/UINT64_C(0x100000000)
#define frandom_range(low,high) ((high-low)*frandom)+low

#define NO_MUSIC

// Meta GID
#define GID_METAEXIT 1
#define GID_METASWITCH 2
#define GID_METACOLLIDABLE 3

#define GID_ROCK 12 // Second tileset
#define GID_EMPTY 0 
#define GID_KEYL2 23
#define GID_L2_OPENDOOR 60

#define GID_SWITCHOFF 49
#define GID_SWITCHON 50
#define GID_CHAOSDOOR_L_OPEN 62

//#define SKIP_MAINMENU

#define PLAYER_SPEED 0.3f // time to complete one tile shift

//#define HASKEY_ALWAYS_TRUE

#endif
