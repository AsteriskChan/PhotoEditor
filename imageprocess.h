#ifndef IMAGEPROCESS_H
#define IMAGEPROCESS_H
#include <vector>
#include <map>
#include <queue>
#include <QImage>
#include <QString>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "colorspace.h"

using namespace std;

class ImageProcess
{
public:
    ImageProcess();
    ImageProcess(QString filename);
    ~ImageProcess();

    QImage getRedChannel();
    QImage getGreenChannel();
    QImage getBlueChannel();
    QImage getQImage();
    QImage getGrayImage();
    QImage otsuBinary();
    QImage twoThresholdBinary(int t1, int t2);
    QImage reverse();
    QImage resize(double scalex, double scaley, int mode);
    QImage rotate(float angle , int mode);
    QImage resizeAndRotate(double scalex, double scaley, float angle, int mode);
    QImage adjustHSL(float scaleH , float scaleS, float scaleL);
    QImage linearTransform(int fa, int fb, int ga, int gb);
    QImage piecewiseLinearTransform(int fa, int fb, int ga, int gb, int gmin, int gmax);
    QImage logarithmTransform(int a, int b, int c);
    QImage expotentTransform(float a, float b, float c);
    QImage histogramEqualization();
    QImage meanFilter(int anchorx, int anchory, int kernelW, int kernelH);
    QImage medianFilter(int anchorx, int anchory, int kernelW, int kernelH);
    QImage gaussianFilter(int kernelSize, int sigma);
    QImage sobel();
    QImage laplacian();
    QImage canny(int t1, int t2);
    QImage houghLine(double rho, double theta, int threshold);
    QImage houghCircle(int rmin, int rmax, int threshold);
    QImage addWeighted(QString filename2, int x1, int y1, int x2, int y2, double alpha);
    QImage add(QString filename2, int x1, int y1, int x2, int y2);
    QImage subtract(QString filename2, int x1, int y1, int x2, int y2);
    QImage multiply(QString filename2, int x1, int y1, int x2, int y2);
    QImage cut(int x1, int y1, int x2, int y2);
    QImage erosion(int *se, int width, int height, int anchorx, int anchory);
    QImage dilation(int *se, int width, int height, int anchorx, int anchory);
    QImage opening(int *se, int width, int height, int anchorx, int anchory);
    QImage closing(int *se, int width, int height, int anchorx, int anchory);
    QImage hitOrMiss(int *se, int width, int height, int anchorx, int anchory);
    QImage thin(int *se, int width, int height, int anchorx, int anchory);
    QImage thicken(int *se, int width, int height, int anchorx, int anchory);
    QImage skeletonize(int *se, int width, int height, int anchorx, int anchory,int maxk);
    QImage skeletonReconstruct(int *se, int width, int height, int anchorx, int anchory);
    QImage distanceTransform(int *se, int width, int height, int anchorx, int anchory);
    cv::Mat constructSE(int *se, int width, int height);
    void setMorphologyMask(QString filename);
    void setMorphologyMask();
    QImage geoDilation(int *se, int width, int height, int anchorx, int anchory);
    QImage geoErosion(int *se, int width, int height, int anchorx, int anchory);
    QImage watershed();
    QImage undo();
    void save();
    QImage initialize();
    bool isSetMask();
    bool isSetSkeleton();

private:
    cv::Mat cimage;
    cv::Mat BGRchannels[3];
    float HSLchannels[3];
    cv::Mat curimage;
    std::vector<cv::Mat> imageVec;
    int version;
    std::vector<cv::Mat> skeletonSubset;
    bool isSkeleton;
    cv::Mat morphologyMask;
    bool hasMask;
    //cv::Mat *blueChannel;
    //cv::Mat *greenChannel;
    //cv::Mat *redChannel;
    void splitChannel(cv::Mat &image);
    QImage cvMat2QImage(const cv::Mat& mat);
    cv::Mat QImage2cvMat(QImage image);
    int otsuTreshold(cv::Mat &image);
    void binaryzation(cv::Mat &image, int threshold); //one threshold
    void binaryzation(cv::Mat &image, int t1, int t2); //two threshold
    void grayImage(cv::Mat &image);
    void nearestResize(cv::Mat &image, double scalex, double scaley);
    void bilinearResize(cv::Mat &image,double scalex, double scaley);
    void nearestRotate(cv::Mat &image, float angle);
    void bilinearRotate(cv::Mat &image, float angle);

    void adjustHSI(cv::Mat &image, float scaleH, float scaleS , float scaleI);
    void test();
    void adjustHSL(cv::Mat &image, float scaleH, float scaleS , float scaleL);

    void linearTransform(cv::Mat &image, int fa, int fb , int ga, int gb);
    void piecewiseLinearTransform(cv::Mat &image, int fa, int fb, int ga, int gb, int gmin, int gmax);
    void logarithmTransform(cv::Mat &image, int a, int b, int c);
    void expotentTransform(cv::Mat &image, float a, float b, float c);
    void histogramEqualization(cv::Mat &image);
    void meanFilter(cv::Mat &image, int anchorx, int anchory , int kernelW, int kernelH);
    void medianFilter(cv::Mat &image, int anchorx, int anchory , int kernelW, int kernelH);
    void filter(cv::Mat &image, float *kernel, int kernelW, int kernelH);
    void getGaussianKernel(float *kernel,int size, float sigma);
    void gaussianFilter(cv::Mat &image, int kernelSize, float sigma);
    void sobel(cv::Mat &image);
    void laplacian(cv::Mat &image);
    void canny(cv::Mat &image, int t1 , int t2);
    void cannyTraceEdge(cv::Mat &image, int *flag, int i, int j, int t1, int t2);
    void houghLine(cv::Mat &image, double rho, double theta, int threshold);
    void houghCircle(cv::Mat &image, int rmin, int rmax, int threshold);
    void addWeighted(cv::Mat &image1, cv::Mat &image2, int x1, int y1, int x2, int y2, double alpha);
    void add(cv::Mat &image1, cv::Mat &image2, int x1, int y1, int x2, int y2);
    void subtract(cv::Mat &image1, cv::Mat &image2, int x1, int y1, int x2, int y2);
    void multiply(cv::Mat &image1, cv::Mat &image2, int x1, int y1, int x2, int y2);
    void cut(cv::Mat &image,int x1, int y1, int x2, int y2);
    void dilation(cv::Mat &image, cv::Mat se, int anchorx, int anchory);
    void erosion(cv::Mat &image, cv::Mat se, int anchorx, int anchory);
    void opening(cv::Mat &image, cv::Mat se, int anchorx, int anchory);
    void closing(cv::Mat &image, cv::Mat se, int anchorx, int anchory);
    void reverseMat(cv::Mat &image);
    void hitOrMiss(cv::Mat &image, cv::Mat se, int anchorx, int anchory);
    void thin(cv::Mat &image, cv::Mat se, int anchorx, int anchory);
    void thin(cv::Mat &image);
    void thicken(cv::Mat &image, cv::Mat se, int anchorx, int anchory);
    void thicken(cv::Mat &image);
    void skeletonize(cv::Mat &image, cv::Mat se, int anchorx, int anchory , int maxk, std::vector<cv::Mat> &s);
    void skeletonReconstruct(cv::Mat &image, cv::Mat se, int anchorx, int anchory , std::vector<cv::Mat> &s );
    cv::Mat binaryComplement(cv::Mat image);
    cv::Mat binaryIntersection(cv::Mat image1, cv::Mat image2);
    cv::Mat binaryUnion(cv::Mat image1, cv::Mat image2);
    cv::Mat grayIntersection(cv::Mat image1, cv::Mat image2);
    cv::Mat grayUnion(cv::Mat image1, cv::Mat image2);
    bool isEmpty(cv::Mat image);
    bool isBinaryImage(cv::Mat image);
    void distanceTransform(cv::Mat &image, cv::Mat se, int anchorx, int anchory);
    void binaryGeoErosion(cv::Mat &image, cv::Mat mask, cv::Mat se, int anchorx, int anchory);
    void binaryGeoDilation(cv::Mat &image, cv::Mat mask, cv::Mat se, int anchorx, int anchory);
    void grayGeoErosion(cv::Mat &image, cv::Mat mask, cv::Mat se, int anchorx, int anchory);
    void grayGeoDilation(cv::Mat &image, cv::Mat mask, cv::Mat se, int anchorx, int anchory);
    void watershed(cv::Mat &image);
};


#endif // IMAGEPROCESS_H
