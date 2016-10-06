//
//  main.c
//  vDSP_01
//
//  Created by chihiro on 2016/09/18.
//  Copyright © 2016年 chihiro hashimoto. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "sndfile.h"

#include "fft.h"
#include "recorder.h"


#define PI 3.14159265358979323846264338 // PI
#define SAMPLE_RATE 44100
#define LENGTH 4 // 4 second
#define AMPLITUDE 1.0
#define FREQUENCY 440 // Hz
#define RESOLUTION 1000
#define FFTSIZE 2048

#include <Accelerate/Accelerate.h>

int main(int argc, const char * argv[]) {

    int status = 0;
    
//    printf("Start main.\n");
//
//    // sound file pointer
//    SNDFILE *fp;
//    // sound infomation
//    SF_INFO sfinfo;
//    // alloc mem
//    memset(&sfinfo, 0, sizeof(sfinfo));
//
//    // settings
//    sfinfo.samplerate = SAMPLE_RATE;
//    sfinfo.frames = SAMPLE_RATE * LENGTH;
//    sfinfo.channels = 2;
//    sfinfo.format = (SF_FORMAT_WAV | SF_FORMAT_PCM_16);
//
//    // memory for sin wave
//    float *buffer;
//    float *buffer2;
//    buffer = malloc(2 * SAMPLE_RATE * LENGTH * sizeof(float)); // 2 channels * SAMPLE_RATE * 4 second * float size
//    buffer2 = malloc(2 * SAMPLE_RATE * LENGTH * sizeof(float)); // 2 channels * SAMPLE_RATE * 4 second * float size
//    
//    if (!(fp = sf_open("/Users/chihiro/Desktop/Sample.wav", SFM_WRITE, &sfinfo))) {
//        printf("Error : Could not open output file.\n");
//        return 1;
//    }
//    
//    // sine wave
//    for (int i = 0; i < SAMPLE_RATE * LENGTH; i++) {
//        // 440hz
//        buffer[2 * i] = AMPLITUDE * sin((float) FREQUENCY / SAMPLE_RATE * 2 * PI * i);
//        // 880hz
//        buffer[2 * i + 1] = AMPLITUDE * sin((float) FREQUENCY * 2 / SAMPLE_RATE * 2 * PI * i);
//    }
//
//    int fftHalfSize = FFTSIZE / 2;
//    DSPSplitComplex splitComplex;
//
//    // alloc complex
//    splitComplex.realp = malloc(sizeof(float) * fftHalfSize);
//    splitComplex.imagp = malloc(sizeof(float) * fftHalfSize);
//    
//    float *magnitude = malloc(sizeof(float) * fftHalfSize);
//
//    // do FFT
//    FFT(buffer, &splitComplex, FFTSIZE);
//
//    vDSP_zvabs(&splitComplex, 1, magnitude, 1, fftHalfSize);
//
//    // fft 1サイズあたりの周波数
//    float freqBins = (float) SAMPLE_RATE / FFTSIZE;
//
//    // print magnitude
//    for (int i = 0; i < fftHalfSize; i++) {
//        printf("%0.2f hz : magunitude = %f\n", freqBins * i, magnitude[i]);
//    }
//    FFT_free(&splitComplex);
//
//    
//    int i, j;
//
////    // 矩形波
////    for(i=0;i<SAMPLE_RATE*LENGTH;i++){ // STEREO
////        float wav = 0;
////        for(j=0;j<RESOLUTION;j++){
////            wav += sin((float)FREQUENCY*(j*2+1) / SAMPLE_RATE * 2 * PI *i)/(j*2+1);
////        }
////        buffer[2*i] = AMPLITUDE * wav;
////        buffer[2*i+1] = AMPLITUDE * wav;
////    }
//
//    // ノコギリ波
////    for(i=0;i<SAMPLE_RATE*LENGTH;i++){ // STEREO
////        float wav = 0;
////        for(j=0;j<RESOLUTION;j++){
////            wav += sin((float)FREQUENCY*(j+1) / SAMPLE_RATE * 2 * PI * i)/(j+1);
////        }
////        buffer[2*i] = AMPLITUDE * (2/PI) * wav;
////        buffer[2*i+1] = AMPLITUDE * (2/PI) * wav;
////    }
//
//    // 重ね合わせ
////    for(i=0;i<SAMPLE_RATE*LENGTH;i++){ // STEREO
////        
////        float wav = 0;
////        float wav2 = 0;
////        for(j=0;j<RESOLUTION;j++){
////            
////            wav += sin((float)FREQUENCY*(j*2+1) / SAMPLE_RATE * 2 * PI * i)/(j*2+1);
////            wav2 += sin((float)FREQUENCY*(j+1) / SAMPLE_RATE * 2 * PI * i)/(j+1);
////            
////        }
////        
////        
////        buffer[2*i] = AMPLITUDE * wav;
////        buffer[2*i+1] = AMPLITUDE * wav;
////        buffer2[2*i] = AMPLITUDE * (2/PI) * wav2;
////        buffer2[2*i+1] = AMPLITUDE * (2/PI) * wav2;
////        
////    }
////    
////    vDSP_vadd(buffer,1,buffer2,1,buffer,1,2*SAMPLE_RATE*LENGTH);
////    
////    float division = 2;
////    vDSP_vsdiv(buffer, 1, &division, buffer, 1, 2*SAMPLE_RATE*LENGTH);
//
//    // write
//    if (sf_write_float(fp, buffer, sfinfo.channels * SAMPLE_RATE * LENGTH) != sfinfo.channels * SAMPLE_RATE * LENGTH) {
//        puts(sf_strerror(fp));
//    };
//
//    // free
//    free(buffer);
//
//    // close
//    sf_close(fp);

    status = record();
    
    return status;
}
