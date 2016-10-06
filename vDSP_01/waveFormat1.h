//
//  waveFormat1.h
//  audiopg
//
//  Created by chihiro on 2016/10/01.
//  Copyright © 2016年 chihiro hashimoto. All rights reserved.
//

#ifndef waveFormat1_h
#define waveFormat1_h

#include <stdio.h>
#include <stdlib.h>

/* 量子化ビット数 */
#define PCM16 (16)

/* 符号付き整数値の最大値 */
#define MAX16BIT (32768.0)

/* WORD型の定義 */
typedef unsigned short WORD; /* 16bit符号なし整数型 */
typedef unsigned int DWORD;  /* 32bit符号なし整数型 */
typedef DWORD FOURCC;        /* 4文字コードの整数型  */

typedef struct waveformat_pcm {
    DWORD riffID;       /* RIFF チャンクID */
    DWORD riffSize;     /* ファイルサイズ - 8 bytes*/
    DWORD typeID;       /* RIFFタイプID : WAVE */
    DWORD fmtID;        /* fmt サブチャンクID */
    DWORD fmtSize;      /* fmt サブチャンクサイズ */
    WORD  formatTag;    /* サウンドフォーマットコード */
    WORD  numChannels;  /* チャンネル数 */
    DWORD samplesPerSec;/* 標本化周波数: fs(HZ) */
    DWORD avgBytesPerSec;/* 転送レート: dataFrameSize */
    WORD  dataFrameSize;/* フレームサイズ: numChannels * bitPerSample / 8 (Bytes) */
    WORD  bitesPerSample;/* サンプル量子化ビット数(16) */
    DWORD dataID;       /* data サブチャンクID */
    DWORD dataSize;     /* サウンドデータサイズ */
} WAVEFORMAT;

/* チャンクID文字コード */
static char RIFF_ID[4] = {'R', 'I', 'F', 'F'};
static char WAVE_ID[4] = {'W', 'A', 'V', 'E'};
static char FMT_ID[4]  = {'f', 'm', 't', ' '};
static char DATA_ID[4] = {'d', 'a', 't', 'a'};



#endif /* waveFormat1_h */