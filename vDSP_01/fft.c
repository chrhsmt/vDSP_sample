//
//  fft.c
//  vDSP_01
//
//  Created by chihiro on 2016/09/20.
//  Copyright © 2016年 chihiro hashimoto. All rights reserved.
//

#include "fft.h"

float *window;
float *windowedInput;

void FFT(float *wavedata, DSPSplitComplex *splitComplex, int fftSize, FFTSetup fftSetup) {
    
    printf("start FFT. \n");

    int fftHalfSize = fftSize / 2;
    vDSP_Length log2n = log2(fftSize);
    
    // setup fft
    printf("setup FFT. \n");
    bool shouldCleanSetup = false;
    if (fftSetup == NULL) {
        fftSetup = vDSP_create_fftsetup(log2n, kFFTRadix2);
        shouldCleanSetup = true;
    }
    
    // make window (fft size)
//    float *window = malloc(sizeof(float) * fftSize);
    // hanning window
//    printf("hanning window. \n");
//    vDSP_hann_window(window, fftSize, 0);
    // windowing
    printf("windowing. \n");
    vDSP_vmul(wavedata, 1, window, 1, windowedInput, 1, fftSize);
    
    // transform to COMPLEX
    printf("fransform complex. \n");
    vDSP_ctoz((COMPLEX *) windowedInput, 2, splitComplex, 1, fftHalfSize);
    // FFT forward
    printf("FFT forward. \n");
    // ここでsplitComplex->realpがNaNになるケースがある
    vDSP_fft_zrip(fftSetup, splitComplex, 1, log2n, FFT_FORWARD);
    
    // scalling
    float scale = 1.0 / (fftSize * 2);
    printf("scalling. \n");
    vDSP_vsmul(splitComplex->realp, 1, &scale, splitComplex->realp, 1, fftHalfSize);
    vDSP_vsmul(splitComplex->imagp, 1, &scale, splitComplex->imagp, 1, fftHalfSize);
    
    if (shouldCleanSetup) {
        vDSP_destroy_fftsetup(fftSetup);
    }
//    free(window);
    printf("finish FFT. \n");
}

void createWindow(int fftSize) {
    window = malloc(sizeof(float) * fftSize);
    windowedInput = (float *) malloc(fftSize);
    vDSP_hann_window(window, fftSize, 0);
}

void freeWindow() {
    free(window);
    free(windowedInput);
}

void FFT_free(DSPSplitComplex *splitComplex) {
    free(splitComplex->realp);
    free(splitComplex->imagp);
}