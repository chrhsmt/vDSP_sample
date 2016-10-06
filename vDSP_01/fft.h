//
//  fft.h
//  vDSP_01
//
//  Created by chihiro on 2016/09/20.
//  Copyright © 2016年 chihiro hashimoto. All rights reserved.
//

#ifndef fft_h
#define fft_h

#include <Accelerate/Accelerate.h>

#include <stdio.h>

#endif /* fft_h */


void FFT(float *wavedata, DSPSplitComplex *splitComplex, int fftSize);

void FFT_free(DSPSplitComplex *splitComplex);