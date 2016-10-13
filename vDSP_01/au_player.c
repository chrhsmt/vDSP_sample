//
//  au_player.c
//  vDSP_01
//
//  Created by chihiro hashimoto on 2016/10/07.
//  Copyright © 2016年 chihiro hashimoto. All rights reserved.
//

#include "au_player.h"

void CreateAUGraph(AUGraphDesc * graphDesc) {
    
    NewAUGraph(&graphDesc->graph);
    
    // 出力デバイスのコンポーネント特性を設定
    AudioComponentDescription outputDevice = {0};
    outputDevice.componentType = kAudioUnitType_Output;
    outputDevice.componentSubType = kAudioUnitSubType_DefaultOutput;
    outputDevice.componentManufacturer = kAudioUnitManufacturer_Apple;
    
    // 出力デバイス・コンポーネントをグラフのノードに追加する
    AUNode outputNode;
    AUGraphAddNode(graphDesc->graph, &outputDevice, &outputNode);
    
    // 再生ファイルのコンポーネント特性を設定
    AudioComponentDescription inputDevice = {0};
    inputDevice.componentType = kAudioUnitType_Generator;
    inputDevice.componentSubType = kAudioUnitSubType_AudioFilePlayer;
    inputDevice.componentManufacturer = kAudioUnitManufacturer_Apple;
    
    // 入力ファイル・コンポーネントをグラフのノードに追加する
    AUNode fileNode;
    AUGraphAddNode(graphDesc->graph, &inputDevice, &fileNode);
    
    // オープン
    AUGraphOpen(graphDesc->graph);
    
    // 再生ファイルノード情報取得
    AUGraphNodeInfo(graphDesc->graph, fileNode, NULL, &graphDesc->fileAU);
    
    // ファイルノードと出力ノードを接続
    AUGraphConnectNodeInput(graphDesc->graph, fileNode, 0, outputNode, 0);
    
    // グラフ初期化
    AUGraphInitialize(graphDesc->graph);
    // グラフ情報を出力
    CAShow(graphDesc->graph);
}

Float64 CulcuratePlayTime(AUGraphDesc * graphDesc) {
    
    // 再生ファイルをAUに設定
    AudioUnitSetProperty(graphDesc->fileAU,
                         kAudioUnitProperty_ScheduledFileIDs,
                         kAudioUnitScope_Global,
                         0,
                         &graphDesc->playFile,
                         sizeof(graphDesc->playFile));
    
    // 再生ファイルのデータ・パケット数を取得する
    UInt64 nPackets;
    UInt32 propSize = sizeof(nPackets);
    AudioFileGetProperty(graphDesc->playFile,
                         kAudioFilePropertyAudioDataPacketCount,
                         &propSize,
                         &nPackets);
    
    // 再生ファイルAUにファイル再生領域の情報を設定
    ScheduledAudioFileRegion region;
    memset(&region.mTimeStamp, 0, sizeof(region.mTimeStamp));
    region.mTimeStamp.mFlags = kAudioTimeStampSampleTimeValid;
    region.mTimeStamp.mSampleTime = 0;
    region.mCompletionProc = NULL;
    region.mCompletionProcUserData = NULL;
    region.mAudioFile = graphDesc->playFile;
    region.mLoopCount = 0;
    region.mStartFrame = 0;
    region.mFramesToPlay = (UInt32) nPackets * graphDesc->fileFormat.mFramesPerPacket;
    
    AudioUnitSetProperty(graphDesc->fileAU,
                         kAudioUnitProperty_ScheduledFileRegion,
                         kAudioUnitScope_Global,
                         0,
                         &region,
                         sizeof(region));
    
    // 再生ファイルAUにファイル再生領域の情報を設定
    AudioTimeStamp startTime;
    memset(&startTime, 0, sizeof(startTime));
    startTime.mFlags = kAudioTimeStampSampleTimeValid;
    startTime.mSampleTime = -1;
    
    AudioUnitSetProperty(graphDesc->fileAU,
                         kAudioUnitProperty_ScheduleStartTimeStamp,
                         kAudioUnitScope_Global,
                         0,
                         &startTime,
                         sizeof(startTime));
    
    return (nPackets * graphDesc->fileFormat.mFramesPerPacket) / graphDesc->fileFormat.mSampleRate;
}

int play(int argc, const char * argv[]) {
    
    const char *filePath = NULL;
    AUGraphDesc graphDesc;
    
    if(argc != 2) {
        fprintf(stderr, "usage: プログラム名 ファイル名\n");
        return 1;
    }
    
    filePath = argv[1];
    CFURLRef audioFileURL = CFURLCreateFromFileSystemRepresentation(NULL, (const UInt8 *) filePath, strlen(filePath), FALSE);
    AudioFileOpenURL(audioFileURL, kAudioFileReadPermission, 0, &graphDesc.playFile);
    CFRelease(audioFileURL);
    
    UInt32 propSize = sizeof(graphDesc.fileFormat);
    OSStatus err = AudioFileGetProperty(
                                        graphDesc.playFile,
                                        kAudioFilePropertyDataFormat,
                                        &propSize,
                                        &graphDesc.fileFormat);
    if (err != noErr) {
        fprintf(stderr, "エラー：サポート対象外のフォーマットです\n");
        exit(1);
    }
    
    CreateAUGraph(&graphDesc);
 
    Float64 playTime = CulcuratePlayTime(&graphDesc);
    
    AUGraphStart(graphDesc.graph);
    printf("再生開始\n");
    printf("%s\n", filePath);
    printf("play time: %f秒\n", playTime);
    
    // sleep (μ秒)
    usleep((int)(playTime * 1000.0 * 1000.0));
    printf("再生終了\n");
 
cleanup:

    AUGraphStop(graphDesc.graph);
    AUGraphUninitialize(graphDesc.graph);
    AUGraphClose(graphDesc.graph);
    AudioFileClose(graphDesc.playFile);

    return noErr;
}