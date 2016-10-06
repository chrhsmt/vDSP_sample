//
//  toneGen.c
//  audiopg
//
//  Created by chihiro on 2016/10/01.
//  Copyright © 2016年 chihiro hashimoto. All rights reserved.
//

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include "toneGen.h"
#include "waveFormat1.h"

void generate() {
    
    const char *filePath = "tone.wav";
    FILE *fpout;
    short ssample;
    int numSamples;
    int fs = 44100;
    int numBits = PCM16;
    int dataBytes = 0;
    float duration = 5.0;
    float frequency = 1000.0;
    double fsample;
    double pi = 4 * atan(1.0);
    double start = 1.0;
    double end = 1.0e-4;
    
    double current;
    double factor;
    double angleIncrement;
    WAVEFORMAT *header;
    
    numSamples = (int) (duration * fs);
    dataBytes = numSamples * sizeof(short);
    angleIncrement = 2 * pi * frequency / fs;
    factor = pow(end/start, 1.0/numSamples);
    
    header = (WAVEFORMAT *) malloc(sizeof(WAVEFORMAT));
    waveSetHeader(header, fs, 1, numBits, dataBytes);
    fpout = fopen(filePath, "wb");
    fwrite(header, 1, sizeof(WAVEFORMAT), fpout);
    
    current = start;
    for(int i = 0; i < numSamples; i++) {
        fsample = current * sin(angleIncrement * i);
        current *= factor;
        ssample = (short) (SHRT_MAX * fsample);
        fwrite(&ssample, sizeof(short), 1, fpout);
    }
    printf("%d サンプルをファイル %s に書く\n", numSamples, filePath);
    
    free(header);
    fclose(fpout);
}

void waveSetHeader(WAVEFORMAT *header, int fs, int channels, int nBits, int dataBytes) {
    header->riffID = *(FOURCC *) RIFF_ID;
    header->riffSize = dataBytes + sizeof(WAVEFORMAT) - 8;
    header->typeID = *(FOURCC *) WAVE_ID;
    header->fmtID = *(FOURCC *) FMT_ID;
    header->fmtSize = 16;
    header->formatTag = 1;
    header->numChannels = (WORD) channels;
    header->samplesPerSec = fs;
    header->avgBytesPerSec = (DWORD)(fs * channels * nBits / 8);
    header->dataFrameSize = (WORD) (channels * nBits / 8);
    header->bitesPerSample = (WORD) nBits;
    header->dataID = *(FOURCC *) DATA_ID;
    header->dataSize = dataBytes;
    return;
}