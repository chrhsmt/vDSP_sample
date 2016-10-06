//
//  recorder.c
//  vDSP_01
//
//  Created by chihiro on 2016/10/01.
//  Copyright © 2016年 chihiro hashimoto. All rights reserved.
//

#include "recorder.h"
#include "waveFormat1.h"
#include "fft.h"

#include <AudioUnit/AudioUnit.h>
#include <AudioToolbox/AudioToolbox.h>
#include <math.h>

#define FFTSIZE 2048
#define SAMPLE_RATE 44100.00
#define BITRATE 16
#define CHANNEL 1

//static FILE *fpout;
static int sampleCount = 0;

static OSStatus RecorderCallback(
                          void *inRefCon,
                          AudioUnitRenderActionFlags *ioActionFlags,
                          const AudioTimeStamp *inTimeStamp,
                          UInt32 inBusNumber,
                          UInt32 inNumberFrames,
                          AudioBufferList *ioData) {
    printf("start\n");
    OSStatus err = noErr;
    
    UInt32 numChannel = CHANNEL;
//    UInt32 dataSize = numChannel * ((UInt32) sizeof(Float64)) * inNumberFrames;
    UInt32 dataSize = numChannel * ((UInt32) sizeof(SInt16)) * inNumberFrames;
    void * mData = malloc((int) dataSize);
    AudioBuffer buffer;
    buffer.mNumberChannels = numChannel;
    buffer.mDataByteSize = dataSize;
    buffer.mData = mData;

    AudioBufferList bufferList;
    bufferList.mNumberBuffers = 1;
    bufferList.mBuffers[0] = buffer;
    
    AudioUnit * inputUnit = (AudioUnit *) inRefCon;
    err = AudioUnitRender(
                          *inputUnit,
                          ioActionFlags,
                          inTimeStamp,
                          inBusNumber,
                          inNumberFrames,
                          &bufferList);
    
    if (err == noErr)
    {
        SInt16 *data = bufferList.mBuffers->mData;

        int frames = (int) (bufferList.mBuffers->mDataByteSize / sizeof(SInt16));
        
        float *buffer = (float *) malloc(sizeof(float) * frames);
        
        float signal = 0;
        for (int i = 0; i < frames - 1; i++) {
            sampleCount += 1;
            signal = (float) data[i];
            if (signal >= 0) {
                signal=signal/32767;
            }else {
                signal=signal/32768;
            }
//            printf("%f\n", signal);
            
//            buffer[i] = signal;
            buffer[i] = SHRT_MAX * signal;
            printf("%f\n", buffer[i]);

//            // save wave file
//            if (sampleCount <= 44100 * 5.0) {
//                SInt16 ssample = (SInt16) (SHRT_MAX * signal);
//                fwrite(&ssample, sizeof(short), 1, fpout);
//            }
        }

        int fftHalfSize = FFTSIZE / 2;
        DSPSplitComplex splitComplex;
        
        // alloc complex
        splitComplex.realp = malloc(sizeof(float) * fftHalfSize);
        splitComplex.imagp = malloc(sizeof(float) * fftHalfSize);
        
        float *magnitude = malloc(sizeof(float) * fftHalfSize);
        for (int i = 0; i < fftHalfSize; i++) {
            magnitude[i] = 0.0f;
        }
        
        // do FFT
        FFT(buffer, &splitComplex, FFTSIZE);
        
        vDSP_zvabs(&splitComplex, 1, magnitude, 1, fftHalfSize);
        
        // fft 1サイズあたりの周波数
        float freqBins = (float) SAMPLE_RATE / FFTSIZE;
        
        // print magnitude
        // TODO: メモリ関係がうまくいかない
        for (int i = 0; i < fftHalfSize; i++) {
            if (freqBins * i > 18000 && freqBins * i < 22000 && !isnan(magnitude[i]) && magnitude[i] != -INFINITY)
            printf("%0.2f hz : magunitude = %f\n", freqBins * i, magnitude[i]);
        }
        FFT_free(&splitComplex);
        free(magnitude);
        free(buffer);

//        free(mData);
    }
    
    return err;
}


OSStatus setUpAudioHAL(AudioUnit * inputUnit) {
    
    OSStatus err = noErr;
    
    // AudioUnitを作成する
    AudioComponentDescription desc;
    
    desc.componentType = kAudioUnitType_Output;
    desc.componentSubType = kAudioUnitSubType_HALOutput;
    
    desc.componentManufacturer = kAudioUnitManufacturer_Apple;
    desc.componentFlags = 0;
    desc.componentFlagsMask = 0;
    
    AudioComponent comp = AudioComponentFindNext(NULL, &desc);
    if (comp == NULL)
    {
        return -1;
    }
    
    err = AudioComponentInstanceNew(comp, inputUnit);
    return err;
}

OSStatus setUpEnableIO(AudioUnit * inputUnit) {

    OSStatus err = noErr;
    
    // AudioUnitの入力を有効化、出力を無効化する。
    // デフォルトは出力有効設定
    UInt32 enableIO = 1;
    UInt32 disableIO = 0;

    err = AudioUnitSetProperty(
                               *inputUnit,
                               kAudioOutputUnitProperty_EnableIO,
                               kAudioUnitScope_Input,
                               1,
                               &enableIO,
                               (UInt32) sizeof(UInt32)
                               );
    
    if (err == noErr)
    {
        err = AudioUnitSetProperty(
                                   *inputUnit,
                                   kAudioOutputUnitProperty_EnableIO,
                                   kAudioUnitScope_Output,
                                   0,
                                   &disableIO,
                                   (UInt32) sizeof(UInt32)
                                   );
    }
    
    return err;
}

char * CFStringCopyUTF8String(CFStringRef aString) {
    if (aString == NULL) {
        return NULL;
    }
    
    CFIndex length = CFStringGetLength(aString);
    CFIndex maxSize =
    CFStringGetMaximumSizeForEncoding(length, kCFStringEncodingUTF8) + 1;
    char *buffer = (char *)malloc(maxSize);
    if (CFStringGetCString(aString, buffer, maxSize,
                           kCFStringEncodingUTF8)) {
        return buffer;
    }
    return NULL;
}

char * deviceName(AudioDeviceID devID)
{
    // 名前確認
    AudioObjectPropertyAddress address;
    address.mSelector = kAudioObjectPropertyName;
    address.mScope = kAudioObjectPropertyScopeGlobal;
    address.mElement = kAudioObjectPropertyElementMaster;
    
    CFStringRef name;
    UInt32 stringsize = (UInt32) sizeof(CFStringRef);
    
    AudioObjectGetPropertyData(devID, &address, 0, nil, &stringsize, &name);
    
    return CFStringCopyUTF8String(name);
    
}
    

OSStatus setUpMicInput(AudioUnit * inputUnit) {
    
    OSStatus err = noErr;
    
    // 入力デバイスを設定
    
    AudioDeviceID inputDeviceId;
    AudioObjectPropertyAddress address;
    address.mSelector = kAudioHardwarePropertyDefaultInputDevice;
    address.mScope = kAudioObjectPropertyScopeGlobal;
    address.mElement = kAudioObjectPropertyElementMaster;
    
    UInt32 size = (UInt32) sizeof(AudioDeviceID);
    
    err = AudioObjectGetPropertyData(
                                     (UInt32) kAudioObjectSystemObject,
                                     &address,
                                     0,
                                     nil,
                                     &size,
                                     &inputDeviceId);
    // デフォルトの入力デバイスを取得
    
    if (err == noErr)
    {
        err = AudioUnitSetProperty(
                                   *inputUnit,
                                   kAudioOutputUnitProperty_CurrentDevice,
                                   kAudioUnitScope_Global,
                                   0,
                                   &inputDeviceId,
                                   size);
        // AudioUnitにデバイスを設定
    }
    
    // 確認用
    printf("DeviceName: %s\n", (char *) deviceName(inputDeviceId));
//    print("BufferSize:",self.bufferSize(inputDeviceId))

    return err;
}

OSStatus setUpInputFormat(AudioUnit * inputUnit) {

    OSStatus err = noErr;

    // サンプリングレートやデータビット数、データフォーマットなどを設定
    AudioStreamBasicDescription audioFormat;
    audioFormat.mBitsPerChannel = BITRATE;
    audioFormat.mBytesPerFrame = 2;
    audioFormat.mBytesPerPacket = 2;
    audioFormat.mChannelsPerFrame = CHANNEL;
    audioFormat.mFormatFlags = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked;
    audioFormat.mFormatID = kAudioFormatLinearPCM;
    audioFormat.mFramesPerPacket = 1;
    audioFormat.mSampleRate = SAMPLE_RATE;
    
    UInt32 size = (UInt32) sizeof(AudioStreamBasicDescription);
    err = AudioUnitSetProperty(
                               *inputUnit,
                               kAudioUnitProperty_StreamFormat,
                               kAudioUnitScope_Output,
                               1,
                               &audioFormat,
                               size);

    return err;
}

OSStatus setUpCallback(AudioUnit * inputUnit) {
    
    OSStatus err = noErr;

    // サンプリング用コールバックを設定
    AURenderCallbackStruct input;
    input.inputProc = RecorderCallback;
    input.inputProcRefCon = inputUnit;
    
    UInt32 size = (UInt32)(sizeof(AURenderCallbackStruct));
    err = AudioUnitSetProperty(
                               *inputUnit,
                               kAudioOutputUnitProperty_SetInputCallback,
                               kAudioUnitScope_Global,
                               0,
                               &input,
                               size);
    
    return err;
}



int record() {

    AudioUnit inputUnit;
    
    if (setUpAudioHAL(&inputUnit) != noErr)
    {
        return -1;
    }

    if (setUpEnableIO(&inputUnit) != noErr)
    {
        return -1;
    }
    
    if (setUpMicInput(&inputUnit) != noErr)
    {
        return -1;
    }
    
    if (setUpInputFormat(&inputUnit) != noErr)
    {
        return -1;
    }
    
    if (setUpCallback(&inputUnit) != noErr)
    {
        printf("err : setUpCallback");
        return -1;
    }

    if (AudioUnitInitialize(inputUnit) != noErr)
    {
        printf("err : AudioUnitInitialize");
        return -1;
    }
    
    if (AudioOutputUnitStart(inputUnit) != noErr)
    {
        printf("err : AudioOutputUnitStart");
        return -1;
    }

//    const char *filePath = "tone.wav";
//    int numSamples;
//    int fs = 44100;
//    int numBits = PCM16;
//    int dataBytes = 0;
//    float duration = 5.0;
//    
//    WAVEFORMAT *header;
//    
//    numSamples = (int) (duration * fs);
//    dataBytes = numSamples * sizeof(short);
//    
//    header = (WAVEFORMAT *) malloc(sizeof(WAVEFORMAT));
//    waveSetHeader(header, fs, 1, numBits, dataBytes);
//    fpout = fopen(filePath, "wb");
//    fwrite(header, 1, sizeof(WAVEFORMAT), fpout);
//    
////    current = start;
////    for(int i = 0; i < numSamples; i++) {
//////        fsample = current * sin(angleIncrement * i);
//////        current *= factor;
////        ssample = (short) (SHRT_MAX * fsample);
////        fwrite(&ssample, sizeof(short), 1, fpout);
////    }
////    printf("%d サンプルをファイル %s に書く\n", numSamples, filePath);
    

    int in;
    do
        in = fgetc(stdin);
    while (in != EOF && in != '\n');
    
//    free(header);
//    fclose(fpout);

    AudioUnitUninitialize(inputUnit);
    AudioComponentInstanceDispose(inputUnit);
    return 0;
    
}

