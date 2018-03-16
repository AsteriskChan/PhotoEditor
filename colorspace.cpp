#include "colorspace.h"

#define MAX2(a, b) ( (a) > (b) ) ?  (a) : (b)
#define MAX3(a,b,c) ( ( (a) > (b) ) ?  MAX2( a, c) : MAX2( b, c ) )

#define MIN2(a, b)  ( (a) < (b) ) ?  (a) : (b)
#define MIN3(a,b,c) ( ( (a) < (b) ) ?  MIN2( a, c ) : MIN2( b, c ) )

namespace cs{

/*
 * Convert RGB to HSI
 *         hue,  value range [0, 1]
 *         saturation,  value range [0, 1]
 *         intensity,   value range [0, 1]
 */
void BGR2HSI(float B, float G, float R, float &H, float &S, float &I)
{
    B = B / 255.0;
    G = G / 255.0;
    R = R / 255.0;
    if (B == G && B == R)
    {
        H = 0;
    }
    else
    {
        float theta = std::acos((0.5*(R-G+R-B))/sqrt(pow(R-G,2)+(R-B)*(G-B)));
        H = (B<=G)? theta: 2*CV_PI -theta;
    }

    I = (float)(R+G+B)/3.0;
    S = 1 - 3 * MIN3(R,G,B)/(R+G+B);
    H = H /(2*CV_PI);
}

void HSI2BGR(float H, float S, float I, float &B, float &G, float &R)
{
    H = H * 2 *CV_PI;
    if (H>=0 && H<2*CV_PI/3)
    {
        B = I * (1 - S);
        R = I * (1 + (S*std::cos(H) / std::cos(CV_PI/3 - H)));
        G = 3 * I - (R + B);
    }
    else if( H>=2*CV_PI/3 && H<4*CV_PI/3)
    {
        H = H - 2*CV_PI/3;
        R = I * (1 - S);
        G = I * (1 + (S*std::cos(H) / std::cos(CV_PI/3 - H)));
        B = 3 * I - (R + G);
    }
    else
    {
        H = H - 4*CV_PI/3;
        G = I * (1 - S);
        B = I * (1 + (S*std::cos(H) / std::cos(CV_PI/3 - H)));
        R = 3 * I - (G + B);
    }
    B = B * 255;
    G = G * 255;
    R = R * 255;

}

/*
 * optimize based on a paper
 * http://d.wanfangdata.com.cn/Periodical/dzzl201606024
 */
void HSI2BGR2(float H, float S, float I, float &B, float &G, float &R)
{
    H = H * 2 *CV_PI;
    if(H>=0 && H <= CV_PI/3)
    {
        R = I * (1 + (S*std::cos(H) / std::cos(CV_PI/3 - H)));
        if(R>1)
        {
            R = 1;
            S = (1-I)*std::cos(CV_PI-H)/(I * std::cos(H));
        }
        B = I * (1 - S);
        G = 3 * I - (R + B);
    }
    else if (H < 2*CV_PI/3)
    {
        B = I * (1 - S);
        R = I * (1 + (S*std::cos(H) / std::cos(CV_PI/3 - H)));
        G = 3 * I - (R + B);
        if ( G > 1)
        {
            G = 1;
            S = (1-I)*std::cos(CV_PI-H)/(I * (std::cos(CV_PI/3-H)-std::cos(H)));
            B = I * (1 - S);
        }
    }
    else if (H < CV_PI)
    {
        H = H - 2*CV_PI/3;
        G = I * (1 + (S*std::cos(H) / std::cos(CV_PI/3 - H)));
        if ( G > 1)
        {
            G = 1;
            S = (1-I)*std::cos(CV_PI-H)/(I * std::cos(H));
        }
        R = I * (1 - S);
        B = 3 * I - (R + G);
    }
    else if (H< 4*CV_PI/3)
    {
        H = H - 2*CV_PI/3;
        R = I * (1 - S);
        G = I * (1 + (S*std::cos(H) / std::cos(CV_PI/3 - H)));
        B = 3 * I - (R + G);
        if (B>1)
        {
            B = 1;
            S = (1-I)*std::cos(CV_PI-H)/(I * (std::cos(CV_PI/3-H)-std::cos(H)));
            R = I * (1 - S);
        }
    }
    else if (H < 5*CV_PI/3)
    {
        H = H - 4*CV_PI/3;
        B = I * (1 + (S*std::cos(H) / std::cos(CV_PI/3 - H)));
        if (B > 1)
        {
            B = 1;
            S = (1-I)*std::cos(CV_PI-H)/(I * std::cos(H));
        }
        G = I * (1 - S);
        R = 3 * I - (G + B);
    }
    else
    {
        H = H - 4*CV_PI/3;
        G = I * (1 - S);
        B = I * (1 + (S*std::cos(H) / std::cos(CV_PI/3 - H)));
        R = 3 * I - (G + B);
        if ( R > 1)
        {
            R = 1;
            S =  (1-I)*std::cos(CV_PI-H)/(I * std::cos(H));
            G = I * (1 - S);
        }

    }
    B = B * 255;
    G = G * 255;
    R = R * 255;
}

/*
 * Convert RGB to HSL
 *         hue,  value range [0, 360]
 *         saturation,  value range [0, 1]
 *         lightness,   value range [0, 1]
 */
void BGR2HSL(const uchar *bgr, float *hsl)
{
    float r1, g1, b1, cmax, cmin, delta;
    r1 = bgr[2] / 255.0;
    g1 = bgr[1] / 255.0;
    b1 = bgr[0] / 255.0;
    cmax = MAX3(r1, g1, b1);
    cmin = MIN3(r1, g1, b1);
    delta = cmax - cmin;
    hsl[2] = (cmax + cmin) / 2;
    if ( delta == 0 ) {
        hsl[1] = 0;
        hsl[0] = 0;
    } else {
        if ( hsl[2] <= 0.5 )
            hsl[1] = delta /(cmax + cmin);
        else
            hsl[1] = delta / (2 - (cmax + cmin));

        if (cmax == r1) {
            if ( g1 >= b1 )
                hsl[0] = int( 60 * (g1 - b1) / delta + 0.5);
            else
                hsl[0] = int( 60 * (g1 - b1) / delta + 360 + 0.5);
        } else if (cmax == g1) {
            hsl[0] = int( 60 * (b1 - r1) / delta + 120 + 0.5);
        } else { // cmax == b1
            hsl[0] = int( 60 * (r1 - g1) / delta + 240 + 0.5);
        }
    }
}

void BGR2HSL(uchar B, uchar G, uchar R, float &H, float &S, float &L)
{
    float hsl[3];
    uchar bgr[3];
    bgr[0] = B;
    bgr[1] = G;
    bgr[2] = R;
    BGR2HSL(bgr, hsl);
    H = hsl[0];
    S = hsl[1];
    L = hsl[2];
}

float hueToRgb(float p, float q, float t)
{
    if (t < 0.0 )
        t += 1.0 ;
    if (t > 1.0 )
        t -= 1.0;
    if (t < 1.0 / 6.0 )
        return p + (q - p) * 6.0 * t;
    if (t < 1.0 / 2.0 )
        return q;
    if (t < 2.0 /3.0 )
        return p + (q - p) * (2.0/3.0 - t) * 6.0;
    return p;
}

void HSL2BGR(const float *hsl, uchar *bgr)
{
    float r1, g1, b1;
    float h = hsl[0] / 360.0;

    if ( hsl[1] == 0 ) {
        r1 = g1 = b1 = hsl[2]; // gray
    } else {
        float q = hsl[2] < 0.5 ? hsl[2] * (1 + hsl[1]) : hsl[2] + hsl[1] - hsl[2] * hsl[1];
        float p = 2 * hsl[2] - q;
        r1 = hueToRgb(p, q, h + 1.0 / 3.0);
        g1 = hueToRgb(p, q, h);
        b1 = hueToRgb(p, q, h - 1.0 / 3.0);
    }

    bgr[0] = int(b1 * 255);
    bgr[1] = int(g1 * 255);
    bgr[2] = int(r1 * 255);
}

void HSL2BGR(float H, float S, float L, uchar &B, uchar &G, uchar &R)
{
    float hsl[3];
    uchar bgr[3];
    hsl[0] = H;
    hsl[1] = S;
    hsl[2] = L;
    HSL2BGR(hsl, bgr);
    B = bgr[0];
    G = bgr[1];
    R = bgr[2];
}
}
