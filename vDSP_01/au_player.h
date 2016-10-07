//
//  au_player.h
//  vDSP_01
//
//  Created by chihiro hashimoto on 2016/10/07.
//  Copyright © 2016年 chihiro hashimoto. All rights reserved.
//

#ifndef au_player_h
#define au_player_h

#include <stdio.h>
#include <AudioToolbox/AudioToolbox.h>

typedef struct AUGraphDesc {
    AudioStreamBasicDescription fileFormat;
    AudioFileID                 playFile;
    AUGraph                     graph;
    AudioUnit                   fileAU;
} AUGraphDesc;


int play(int argc, char * argv[]);

#endif /* au_player_h */
