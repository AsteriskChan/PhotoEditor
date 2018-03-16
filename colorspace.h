#ifndef COLORSPACE_H
#define COLORSPACE_H
#include <opencv2/core/core.hpp>

namespace cs{

void BGR2HSI(float B, float G, float R, float &H, float  &S, float &I);
void HSI2BGR(float H,float S,float I,float &B,float &G,float &R);
void HSI2BGR2(float H,float S,float I,float &B,float &G,float &R);

void BGR2HSL(const uchar *bgr, float *hsl);
void BGR2HSL(uchar B, uchar G, uchar R, float &H, float &S, float &L);
float hueToRgb(float p, float q, float t);
void HSL2BGR(const float *hsl, uchar *bgr);
void HSL2BGR(float H, float S, float L, uchar &B, uchar &G, uchar &R);
}

#endif // COLORSPACE_H
