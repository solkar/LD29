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

#define GID_ROCK 12 // Second tileset
#define GID_EMPTY 0 

#endif
