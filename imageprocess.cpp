#include "imageprocess.h"
#include <QDebug>
#include <iostream>
#include <math.h>
#include <algorithm>
#include "filter.h"


#define MAX2(a, b) ( (a) > (b) ) ?  (a) : (b)
#define MAX3(a,b,c) ( ( (a) > (b) ) ?  MAX2( a, c) : MAX2( b, c ) )

#define MIN2(a, b)  ( (a) < (b) ) ?  (a) : (b)
#define MIN3(a,b,c) ( ( (a) < (b) ) ?  MIN2( a, c ) : MIN2( b, c ) )

using namespace cs;

ImageProcess::ImageProcess()
{

}

ImageProcess::ImageProcess(QString filename)
{
    this->cimage = cv::imread((std::string)filename.toLocal8Bit(),cv::IMREAD_UNCHANGED);
    initialize();
}

ImageProcess::~ImageProcess()
{

}

QImage ImageProcess::cvMat2QImage(const cv::Mat& mat)
{
    // 8-bits unsigned, NO. OF CHANNELS = 1
    if(mat.type() == CV_8UC1)
    {
        QImage image(mat.cols, mat.rows, QImage::Format_Indexed8);
        // Set the color table (used to translate colour indexes to qRgb values)
        //QImage image(mat.cols,mat.rows,QImage::Format_Grayscale8);
        image.setColorCount(256);
        for(int i = 0; i < 256; i++)
        {
            image.setColor(i, qRgb(i, i, i));
        }
        // Copy input Mat
        uchar *pSrc = mat.data;
        for(int row = 0; row < mat.rows; row ++)
        {
            uchar *pDest = image.scanLine(row);
            memcpy(pDest, pSrc, mat.cols);
            pSrc += mat.step;
        }
        return image;
    }
    // 8-bits unsigned, NO. OF CHANNELS = 3
    else if(mat.type() == CV_8UC3)
    {
        // Copy input Mat
        const uchar *pSrc = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
        return image.rgbSwapped();
    }
    else if(mat.type() == CV_8UC4)
    {
        qDebug() << "CV_8UC4";
        // Copy input Mat
        const uchar *pSrc = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);
        return image.copy();
    }
    else
    {
        qDebug() << "ERROR: Mat could not be converted to QImage.";
        return QImage();
    }
}

/* just use opencv to read image and this function may be useless
cv::Mat ImageProcess::QImage2cvMat(QImage image)
{
    cv::Mat mat;
    qDebug() << "QImage format:"<<image.format()<<endl;;
    switch(image.format())
    {
    case QImage::Format_ARGB32:
    case QImage::Format_RGB32:
    case QImage::Format_ARGB32_Premultiplied:
        mat = cv::Mat(image.height(), image.width(), CV_8UC4, (void*)image.constBits(), image.bytesPerLine());
        break;
    case QImage::Format_RGB888:
        mat = cv::Mat(image.height(), image.width(), CV_8UC3, (void*)image.constBits(), image.bytesPerLine());
        cv::cvtColor(mat, mat, CV_BGR2RGB);
        break;
    case QImage::Format_Indexed8:
    case QImage::Format_Grayscale8:
        mat = cv::Mat(image.height(), image.width(), CV_8UC1, (void*)image.constBits(), image.bytesPerLine());
        break;
    }

    return mat;
}
*/



QImage ImageProcess::getRedChannel()
{
    //imageVec.push_back(channels[2]);
    splitChannel(curimage);
    curimage = BGRchannels[2].clone();
    version++;
    imageVec.insert(imageVec.begin()+version,curimage.clone());
    return cvMat2QImage(curimage);
}

QImage ImageProcess::getBlueChannel()
{
    //imageVec.push_back(channels[0]);
    splitChannel(curimage);
    curimage = BGRchannels[0].clone();

    version++;
    imageVec.insert(imageVec.begin()+version,curimage.clone());
    return cvMat2QImage(curimage);
}

QImage ImageProcess::getGreenChannel()
{
    //imageVec.push_back(channels[1]);
    splitChannel(curimage);
    curimage = BGRchannels[1].clone();

    version++;
    imageVec.insert(imageVec.begin()+version,curimage.clone());
    return cvMat2QImage(curimage);
}

QImage ImageProcess::getQImage()
{
    return cvMat2QImage(curimage);
}

QImage ImageProcess::getGrayImage()
{
    if (curimage.channels()!=3 && curimage.channels()!=4){
        return cvMat2QImage(curimage);
    }
    grayImage(curimage);
    version++;
    imageVec.insert(imageVec.begin()+version,curimage.clone());
    return cvMat2QImage(curimage);
}

QImage ImageProcess::otsuBinary()
{
    if (curimage.channels()!=1){
        return cvMat2QImage(curimage);
    }
    int t = otsuTreshold(curimage);
    binaryzation(curimage,t);
    version++;
    imageVec.insert(imageVec.begin()+version,curimage.clone());
    return cvMat2QImage(curimage);
}

QImage ImageProcess::twoThresholdBinary(int t1, int t2)
{
    curimage = imageVec.at(version).clone();
    binaryzation(curimage,t1,t2);
    return cvMat2QImage(curimage);
}

QImage ImageProcess::reverse()
{

    curimage = binaryComplement(curimage);
    version++;
    imageVec.insert(imageVec.begin()+version,curimage.clone());
    return cvMat2QImage(curimage);
}

QImage ImageProcess::resize(double scalex, double scaley , int mode)
{

    if (mode == 1)
    {
        nearestResize(curimage,scalex,scaley);
    }
    if (mode == 2)
    {
        bilinearResize(curimage,scalex,scaley);
    }
    return cvMat2QImage(curimage);
}

QImage ImageProcess::rotate(float angle, int mode)
{
    if (mode ==1)
    {
        nearestRotate(curimage,angle);
    }
    if (mode ==2)
    {
        bilinearRotate(curimage,angle);
    }
    return cvMat2QImage(curimage);
}

QImage ImageProcess::resizeAndRotate(double scalex, double scaley, float angle, int mode)
{
   curimage = imageVec.at(version).clone();
    switch(mode)
    {
    case 1:
        bilinearResize(curimage,scalex,scaley);
        bilinearRotate(curimage,angle);
        break;
    case 2:
        bilinearResize(curimage,scalex,scaley);
        nearestRotate(curimage, angle);
        break;
    case 3:
        nearestResize(curimage,scalex,scaley);
        bilinearRotate(curimage,angle);
        break;
    case 4:
        nearestResize(curimage,scalex,scaley);
        nearestRotate(curimage,angle);
        break;
    default:
        break;
    }
    return cvMat2QImage(curimage);
}


QImage ImageProcess::adjustHSL(float scaleH, float scaleS, float scaleL)
{
    curimage = imageVec.at(version).clone();
    adjustHSL(curimage,scaleH,scaleS,scaleL);
    return cvMat2QImage(curimage);
}

QImage ImageProcess::linearTransform(int fa, int fb, int ga, int gb)
{
    if (curimage.channels()!=1)
    {
        return cvMat2QImage(curimage);
    }
    curimage = imageVec.at(version).clone();
    fa = MIN2(fa,fb);
    fb = MAX2(fa,fb);
    //ga = MIN2(ga,gb);
    //gb = MAX2(ga,gb);
    linearTransform(curimage,fa,fb,ga,gb);
    return cvMat2QImage(curimage);
}

QImage ImageProcess::piecewiseLinearTransform(int fa, int fb, int ga, int gb, int gmin, int gmax)
{
    if (curimage.channels()!=1)
    {
        return cvMat2QImage(curimage);
    }
    curimage = imageVec.at(version).clone();
    fa = MIN2(fa,fb);
    fb = MAX2(fa,fb);
    //ga = MIN2(ga,gb);
    //gb = MAX2(ga,gb);
    piecewiseLinearTransform(curimage,fa,fb,ga,gb,gmin,gmax);
    return cvMat2QImage(curimage);
}

QImage ImageProcess::logarithmTransform(int a, int b, int c)
{
    if (curimage.channels()!=1)
    {
        return cvMat2QImage(curimage);
    }
    curimage = imageVec.at(version).clone();
    logarithmTransform(curimage,a,b,c);
    return cvMat2QImage(curimage);
}

QImage ImageProcess::expotentTransform(float a, float b, float c)
{
    if (curimage.channels()!=1)
    {
        return cvMat2QImage(curimage);
    }
    curimage = imageVec.at(version).clone();
    expotentTransform(curimage,a,b,c);
    return cvMat2QImage(curimage);
}

QImage ImageProcess::histogramEqualization()
{
    if (curimage.channels()!=1)
    {
        return cvMat2QImage(curimage);
    }
    curimage = imageVec.at(version).clone();
    histogramEqualization(curimage);
    return cvMat2QImage(curimage);
}

QImage ImageProcess::meanFilter(int anchorx, int anchory, int kernelW, int kernelH)
{
    meanFilter(curimage,anchorx,anchory,kernelW,kernelH);
    version++;
    imageVec.insert(imageVec.begin()+version,curimage.clone());
    return cvMat2QImage(curimage);
}

QImage ImageProcess::medianFilter(int anchorx, int anchory, int kernelW, int kernelH)
{
    medianFilter(curimage,anchorx,anchory,kernelW,kernelH);
    version++;
    imageVec.insert(imageVec.begin()+version,curimage.clone());
    return cvMat2QImage(curimage);
}

QImage ImageProcess::gaussianFilter(int kernelSize, int sigma)
{
    gaussianFilter(curimage,kernelSize,sigma);
    version++;
    imageVec.insert(imageVec.begin()+version,curimage.clone());
    return cvMat2QImage(curimage);
}

QImage ImageProcess::sobel()
{
    sobel(curimage);
    version++;
    imageVec.insert(imageVec.begin()+version,curimage.clone());
    return cvMat2QImage(curimage);
}

QImage ImageProcess::laplacian()
{
    laplacian(curimage);
    version++;
    imageVec.insert(imageVec.begin()+version,curimage.clone());
    return cvMat2QImage(curimage);
}

QImage ImageProcess::canny(int t1, int t2)
{
    if (t1 > t2)
        std::swap(t1,t2);
    canny(curimage,t1,t2);
    version++;
    imageVec.insert(imageVec.begin()+version,curimage.clone());
    return cvMat2QImage(curimage);
}

QImage ImageProcess::houghLine(double rho, double theta, int threshold)
{
    houghLine(curimage,rho,theta,threshold);
    version++;
    imageVec.insert(imageVec.begin()+version,curimage.clone());
    return cvMat2QImage(curimage);
}

QImage ImageProcess::houghCircle(int rmin, int rmax, int threshold)
{
    houghCircle(curimage,rmin,rmax,threshold);
    version++;
    imageVec.insert(imageVec.begin()+version,curimage.clone());
    return cvMat2QImage(curimage);
}

QImage ImageProcess::addWeighted(QString filename2, int x1, int y1, int x2, int y2, double alpha)
{
    cv::Mat image2 = cv::imread((std::string)filename2.toLocal8Bit(),cv::IMREAD_UNCHANGED);
    addWeighted(curimage,image2,x1,y1,x2,y2,alpha);
    version++;
    imageVec.insert(imageVec.begin()+version,curimage.clone());
    return cvMat2QImage(curimage);
}

QImage ImageProcess::add(QString filename2, int x1, int y1, int x2, int y2)
{
    cv::Mat image2 = cv::imread((std::string)filename2.toLocal8Bit(),cv::IMREAD_UNCHANGED);
    add(curimage,image2,x1,y1,x2,y2);
    version++;
    imageVec.insert(imageVec.begin()+version,curimage.clone());
    return cvMat2QImage(curimage);
}

QImage ImageProcess::subtract(QString filename2, int x1, int y1, int x2, int y2)
{
    cv::Mat image2 = cv::imread((std::string)filename2.toLocal8Bit(),cv::IMREAD_UNCHANGED);
    subtract(curimage,image2,x1,y1,x2,y2);
    version++;
    imageVec.insert(imageVec.begin()+version,curimage.clone());
    return cvMat2QImage(curimage);
}

QImage ImageProcess::multiply(QString filename2, int x1, int y1, int x2, int y2)
{
    cv::Mat image2 = cv::imread((std::string)filename2.toLocal8Bit(),cv::IMREAD_UNCHANGED);
    multiply(curimage,image2,x1,y1,x2,y2);
    version++;
    imageVec.insert(imageVec.begin()+version,curimage.clone());
    return cvMat2QImage(curimage);
}

QImage ImageProcess::cut(int x1, int y1, int x2, int y2)
{
    cut(curimage,x1,y1,x2,y2);
    version++;
    imageVec.insert(imageVec.begin()+version,curimage.clone());
    return cvMat2QImage(curimage);
}

QImage ImageProcess::erosion(int *se, int width, int height, int anchorx, int anchory)
{
    cv::Mat seMat = constructSE(se,width,height);
    erosion(curimage,seMat,anchorx,anchory);
    version++;
    imageVec.insert(imageVec.begin()+version,curimage.clone());
    return cvMat2QImage(curimage);
}

QImage ImageProcess::dilation(int *se, int width, int height, int anchorx, int anchory)
{
    cv::Mat seMat = constructSE(se,width,height);
    dilation(curimage,seMat,anchorx,anchory);
    version++;
    imageVec.insert(imageVec.begin()+version,curimage.clone());
    return cvMat2QImage(curimage);
}

QImage ImageProcess::opening(int *se, int width, int height, int anchorx, int anchory)
{
    cv::Mat seMat = constructSE(se,width,height);
    opening(curimage,seMat,anchorx,anchory);
    version++;
    imageVec.insert(imageVec.begin()+version,curimage.clone());
    return cvMat2QImage(curimage);
}

QImage ImageProcess::closing(int *se, int width, int height, int anchorx, int anchory)
{
    cv::Mat seMat = constructSE(se,width,height);
    closing(curimage,seMat,anchorx,anchory);
    version++;
    imageVec.insert(imageVec.begin()+version,curimage.clone());
    return cvMat2QImage(curimage);
}

QImage ImageProcess::hitOrMiss(int *se, int width, int height, int anchorx, int anchory)
{
    cv::Mat seMat = constructSE(se,width,height);
    hitOrMiss(curimage,seMat,anchorx,anchory);
    version++;
    imageVec.insert(imageVec.begin()+version,curimage.clone());
    return cvMat2QImage(curimage);
}

QImage ImageProcess::thin(int *se, int width, int height, int anchorx, int anchory)
{
    //cv::Mat seMat = constructSE(se,width,height);
    thin(curimage);
    version++;
    imageVec.insert(imageVec.begin()+version,curimage.clone());
    return cvMat2QImage(curimage);
}

QImage ImageProcess::thicken(int *se, int width, int height, int anchorx, int anchory)
{
    //cv::Mat seMat = constructSE(se,width,height);
    thicken(curimage);
    version++;
    imageVec.insert(imageVec.begin()+version,curimage.clone());
    return cvMat2QImage(curimage);
}

QImage ImageProcess::skeletonize(int *se, int width, int height, int anchorx, int anchory, int maxk)
{
    if (isSkeleton)
        return cvMat2QImage(curimage);
    cv::Mat seMat = constructSE(se,width,height);
    skeletonSubset.clear();
    skeletonize(curimage,seMat,anchorx,anchory,maxk,skeletonSubset);
    /*
    version++;
    imageVec.insert(imageVec.begin()+version,curimage.clone());
    */
    isSkeleton = true;
    return cvMat2QImage(curimage);

}

QImage ImageProcess::skeletonReconstruct(int *se, int width, int height, int anchorx, int anchory)
{
    if(!isSkeleton)
        return cvMat2QImage(curimage);
    cv::Mat seMat = constructSE(se,width,height);
    skeletonReconstruct(curimage,seMat,anchorx,anchory,skeletonSubset);
    /*
    version++;
    imageVec.insert(imageVec.begin()+version,curimage.clone());
    */
    skeletonSubset.clear();
    isSkeleton = false;
    return cvMat2QImage(curimage);

}

QImage ImageProcess::distanceTransform(int *se, int width, int height, int anchorx, int anchory)
{
    cv::Mat seMat = constructSE(se,width,height);
    distanceTransform(curimage,seMat,anchorx,anchory);
    version++;
    imageVec.insert(imageVec.begin()+version,curimage.clone());
    return cvMat2QImage(curimage);
}

cv::Mat ImageProcess::constructSE(int *se, int width, int height)
{
    cv::Mat res(height,width,CV_8UC1);
    for (int i=0; i<height;i++)
    {
        for (int j=0; j<width; j++)
        {
            res.at<uchar>(i,j) = se[i*width+j];
        }
    }
    return res;
}

void ImageProcess::setMorphologyMask(QString filename)
{
    morphologyMask = cv::imread((std::string)filename.toLocal8Bit(),cv::IMREAD_UNCHANGED);
    hasMask = true;
    return;
}

void ImageProcess::setMorphologyMask()
{
    morphologyMask = curimage.clone();
    hasMask = true;
    return;
}

QImage ImageProcess::geoDilation(int *se, int width, int height, int anchorx, int anchory)
{
    cv::Mat seMat = constructSE(se,width,height);
    if (!hasMask)
        return cvMat2QImage(curimage);
    if (isBinaryImage(curimage))
    {
        binaryGeoDilation(curimage,morphologyMask,seMat,anchorx,anchory);
    }
    else
    {
        grayGeoDilation(curimage,morphologyMask,seMat,anchorx,anchory);
    }

    version++;
    imageVec.insert(imageVec.begin()+version,curimage.clone());
    return cvMat2QImage(curimage);
}

QImage ImageProcess::geoErosion(int *se, int width, int height, int anchorx, int anchory)
{
    cv::Mat seMat = constructSE(se,width,height);
    if (!hasMask)
        return cvMat2QImage(curimage);
    if (isBinaryImage(curimage))
    {
        binaryGeoErosion(curimage,morphologyMask,seMat,anchorx,anchory);
    }
    else
    {
        grayGeoErosion(curimage,morphologyMask,seMat,anchorx,anchory);
    }

    version++;
    imageVec.insert(imageVec.begin()+version,curimage.clone());
    return cvMat2QImage(curimage);
}

QImage ImageProcess::watershed()
{
    watershed(curimage);
    version++;
    imageVec.insert(imageVec.begin()+version,curimage.clone());
    return cvMat2QImage(curimage);
}

QImage ImageProcess::undo()
{
    if(version>0)
    {
        version--;
    }
    curimage = imageVec.at(version).clone();
    //qDebug() << "version: "<<version<<endl;
    return cvMat2QImage(curimage);
}

void ImageProcess::save()
{
    version++;
    imageVec.insert(imageVec.begin()+version,curimage.clone());
    return;
}

QImage ImageProcess::initialize()
{
    imageVec.clear();
    imageVec.push_back(cimage.clone());
    version = 0;
    curimage = cimage.clone();
    isSkeleton = false;
    hasMask = false;
    return cvMat2QImage(curimage);
}

bool ImageProcess::isSetMask()
{
    return hasMask;
}

bool ImageProcess::isSetSkeleton()
{
    return isSkeleton;
}

/*------------------------------------ Algorithm ------------------------------------------*/

void ImageProcess::splitChannel(cv::Mat &image)
{
    qDebug()<< "channels:"<<image.channels() <<endl;  //get channel number
    if (image.channels()== 3 || image.channels()== 4){

        int size = image.rows * image.cols;
        uchar *blue = new uchar[size];
        uchar *green = new uchar[size];
        uchar *red = new uchar[size];

        int i=0;
        cv::MatIterator_<cv::Vec3b> it = image.begin<cv::Vec3b>();
        cv::MatIterator_<cv::Vec3b> end = image.end<cv::Vec3b>();
        for( ; it != end; ++it)
        {
            blue[i]=(*it)[0];
            green[i]=(*it)[1];
            red[i]=(*it)[2];
            i++;
        }
        cv::Mat blueChannel(image.rows,image.cols,CV_8UC1,blue);
        cv::Mat greenChannel(image.rows,image.cols,CV_8UC1,green);
        cv::Mat redChannel(image.rows,image.cols,CV_8UC1,red);
        blueChannel.copyTo(BGRchannels[0]);
        greenChannel.copyTo(BGRchannels[1]);
        redChannel.copyTo(BGRchannels[2]);

        delete blue;
        delete green;
        delete red;
    }
}

int ImageProcess::otsuTreshold(cv::Mat &image)
{
    if(image.channels()!=1)
    {
        return 0;
    }
    int T=0; //Otsu算法阈值
    double deltaMax=0; //类间方差中间值保存
    double w0=0; //前景像素点数所占比例
    double w1=0; //背景像素点数所占比例
    double u0=0; //前景平均灰度
    double u1=0; //背景平均灰度
    int totalNum = image.rows*image.cols;
    double grayCount[256]={0}; //灰度直方图，下标是灰度值，保存内容是灰度值对应的像素点总数
    cv::MatIterator_<uchar> it = image.begin<uchar>();
    cv::MatIterator_<uchar> end = image.end<uchar>();
    for( ; it != end; ++it)
    {
        grayCount[*it]++;
    }
    for(int i=0;i<255;i++)
    {
        //每次遍历之前初始化各变量
        w1=0;
        u1=0;
        w0=0;
        u0=0;
        //背景各分量值计算
        for(int j=0;j<=i;j++) //背景部分各值计算
        {
            w1+=grayCount[j];  //背景部分像素点总数
            u1+=j*grayCount[j]; //背景部分像素总灰度和
        }
        if(w1==0) //背景部分像素点数为0时退出
        {
            continue;
        }
        u1=u1/w1; //背景像素平均灰度
        w1=w1/totalNum; // 背景部分像素点数所占比例

        //前景各分量值计算
        for(int k=i+1;k<255;k++)
        {
            w0+=grayCount[k];  //前景部分像素点总数
            u0+=k*grayCount[k]; //前景部分像素总灰度和
        }
        if(w0==0) //前景部分像素点数为0时退出
        {
            continue;
        }
        u0=u0/w0; //前景像素平均灰度
        w0=w0/totalNum; // 前景部分像素点数所占比例


        //类间方差计算
        double deltaTmp=w0*w1*(u1-u0)*(u1-u0); //当前类间方差计算
        if(deltaMax<=deltaTmp)
        {
            deltaMax=deltaTmp;
            T=i;
        }
    }
    qDebug()<<"threshold: " << T <<endl;
    return T;
}

void ImageProcess::binaryzation(cv::Mat &image, int t)
{
    cv::MatIterator_<uchar> it = image.begin<uchar>();
    cv::MatIterator_<uchar> end = image.end<uchar>();
    for( ; it != end; ++it)
    {
        if(*it >t)
            *it = 255;
        else
            *it = 0;
    }
    return;
}

void ImageProcess::binaryzation(cv::Mat &image, int t1 , int t2)
{
    cv::MatIterator_<uchar> it = image.begin<uchar>();
    cv::MatIterator_<uchar> end = image.end<uchar>();
    int min , max;
    if (t1 >= t2){
        max = t1;
        min = t2;

    }
    else{
        max = t2;
        min = t1;
    }
    for( ; it != end; ++it)
    {
        if(*it >=min && *it <= max)
            *it = 255;
        else
            *it = 0;
    }
    return;
}

void ImageProcess::grayImage(cv::Mat &image)
{
    int size = image.rows * image.cols;
    uchar *gray = new uchar[size];
    int i=0;
    cv::MatIterator_<cv::Vec3b> it = image.begin<cv::Vec3b>();
    cv::MatIterator_<cv::Vec3b> end = image.end<cv::Vec3b>();
    for( ; it != end; ++it)
    {
        gray[i] = (1140 * ((*it)[0]) + 5870 * ((*it)[1]) + 2989 * ((*it)[2]) ) / 10000;
        i++;
    }
    cv::Mat grayImage(image.rows,image.cols,CV_8UC1,gray);
    grayImage.copyTo(image);
    return;
}

void ImageProcess::nearestResize(cv::Mat &image, double scalex, double scaley)
{
    int rescols = cvFloor(image.cols * scalex);
    int resrows = cvFloor(image.rows * scaley);
    if (rescols< 20 || resrows< 20)
    {
        return;
    }
    cv::Mat res(resrows,rescols, image.type());
    uchar* imgData = image.data;
    int imgStep = image.step;
    uchar* resData = res.data;
    int resStep = res.step;
    int channel = image.channels();
    for (int i = 0; i < res.cols; i++) //以目标图像为基准，映射到源图像中
    {
        int x = cvFloor(i / scalex);  //向下取整
        x = std::min(x, image.cols - 1); //防止超出源图像的边界
        for (int j = 0; j < res.rows; j++)
        {
            int y = cvFloor(j / scaley);
            y = std::min(y, image.rows - 1); //防止超出源图像的边界
            /*
            if(image.channels()==1)   //灰度图片
            {
                res.at<uchar>(j,i) = image.at<uchar>(sy,sx);
            }
            else    //彩色图片
            {
                res.at<cv::Vec3b>(j, i) = image.at<cv::Vec3b>(sy, sx);
            }
            */
            for(int k = 0; k < channel ; k++)
            {
                *(resData+ j*resStep + i*channel + k ) = *(imgData +y*imgStep + x*channel + k);
            }
        }
    }
    res.copyTo(image);
}

void ImageProcess::bilinearResize(cv::Mat &image, double scalex, double scaley)
{
    int rescols = cvFloor(image.cols * scalex);
    int resrows = cvFloor(image.rows * scaley);
    if (rescols< 20 || resrows< 20)
    {
        return;
    }
    cv::Mat res(resrows,rescols, image.type());
    uchar* imgData = image.data;
    int imgStep = image.step;
    uchar* resData = res.data;
    int resStep = res.step;
    int channel = image.channels();
    for (int i = 0; i < res.cols; i++)
    {
        float sx = (float)((i + 0.5) / scalex - 0.5);
        int x = cvFloor(sx);  //x为sx的整数部分（向下取整）
        float u= sx - x;  //u为sx的小数部分
        if( x<0 )
        {
            x = 0;
            u = 0;
        }
        if ( x>=image.cols - 1 )
        {
            x = image.cols - 2;
            u = 0;
        }
        for (int j = 0; j < res.rows; j++)
        {
            float sy = (float)((j + 0.5) /scaley - 0.5);
            int y = cvFloor(sy);//y为sy的整数部分（向下取整）
            float v = sy - y; //v更新为sy的小数部分
            if (y < 0)
            {
                y = 0;
                v = 0;
            }
            if (y >= image.rows - 1)
            {
                y = image.rows - 2;
                v = 0;
            }
            /*
            if(image.channels() == 1)
            {
                res.at<uchar>(j,i) = (1-u)*(1-v)*image.at<uchar>(y,x)+
                        (1-u)*v*image.at<uchar>(y+1,x) + u*(1-v)*image.at<uchar>(y,x+1)
                        +u*v*image.at<uchar>(y+1,x+1);
            }
            else
            {
                res.at<cv::Vec3b>(j,i)[0] = (1-u)*(1-v)*image.at<cv::Vec3b>(y,x)[0]+
                        (1-u)*v*image.at<cv::Vec3b>(y+1,x)[0] + u*(1-v)*image.at<cv::Vec3b>(y,x+1)[0]
                        +u*v*image.at<cv::Vec3b>(y+1,x+1)[0];
                res.at<cv::Vec3b>(j,i)[1] = (1-u)*(1-v)*image.at<cv::Vec3b>(y,x)[1]+
                        (1-u)*v*image.at<cv::Vec3b>(y+1,x)[1] + u*(1-v)*image.at<cv::Vec3b>(y,x+1)[1]
                        +u*v*image.at<cv::Vec3b>(y+1,x+1)[1];
                res.at<cv::Vec3b>(j,i)[2] = (1-u)*(1-v)*image.at<cv::Vec3b>(y,x)[2]+
                        (1-u)*v*image.at<cv::Vec3b>(y+1,x)[2] + u*(1-v)*image.at<cv::Vec3b>(y,x+1)[2]
                        +u*v*image.at<cv::Vec3b>(y+1,x+1)[2];
            }
            */
            for(int k=0; k<channel;k++)
            {
                *(resData+ j*resStep + i*channel + k ) = (1-u)*(1-v)*(*(imgData + y*imgStep + x*channel + k))
                        +(1-u)*v*(*(imgData + (y+1)*imgStep + x*channel + k))
                        + u*(1-v)*(*(imgData + y*imgStep + (x+1)*channel + k))
                        +u*v*(*(imgData +(y+1)*imgStep + (x+1)*channel + k));
            }

        }
    }
    res.copyTo(image);
}

void ImageProcess::nearestRotate(cv::Mat &image, float angle)
{
    float alpha=angle*CV_PI/180.0;
    float cosalpha=cos(alpha);
    float sinalpha=sin(alpha);
    int imgrows=image.rows;
    int imgcols=image.cols;

    int rescols= cvFloor(imgcols * fabs(cosalpha) + imgrows * fabs(sinalpha)+0.5);
    int resrows= cvFloor(imgrows * fabs(cosalpha) + imgcols * fabs(sinalpha)+0.5);
    //qDebug()<<"new :"<<resrows<<" " <<rescols<<endl;
    cv::Mat res(resrows,rescols,image.type());
    uchar* imgData = image.data;
    int imgStep = image.step;
    uchar* resData = res.data;
    int resStep = res.step;
    int channel = image.channels();
    for(int i=0;i<resrows;i++)
    {

        for(int j=0;j<rescols;j++)
        {
            int x = (int)((i-resrows/2)*cosalpha - (j-rescols/2)*sinalpha + 0.5);
            int y = (int)((i-resrows/2)*sinalpha + (j-rescols/2)*cosalpha + 0.5);
            x += image.rows / 2;
            y += image.cols / 2;
            if(x>=0 && x<imgrows && y>=0 && y<imgcols)
            {
                for (int k= 0; k<channel;k++ )
                {
                     *(resData+ i*resStep + j*channel + k ) = *(imgData +x*imgStep + y*channel + k);
                }
            }
            else
            {
                for (int k= 0; k<channel;k++ )
                {
                     *(resData+ i*resStep + j*channel + k ) = (uchar)0;
                }
            }
        }
    }
    res.copyTo(image);
}

void ImageProcess::bilinearRotate(cv::Mat &image, float angle)
{
    float alpha=angle*CV_PI/180.0;
    float cosalpha=cos(alpha);
    float sinalpha=sin(alpha);
    int imgrows=image.rows;
    int imgcols=image.cols;

    int rescols= cvFloor(imgcols * fabs(cosalpha) + imgrows * fabs(sinalpha)+0.5);
    int resrows= cvFloor(imgrows * fabs(cosalpha) + imgcols * fabs(sinalpha)+0.5);
    //qDebug()<<"new :"<<resrows<<" " <<rescols<<endl;
    cv::Mat res(resrows,rescols,image.type());
    uchar* imgData = image.data;
    int imgStep = image.step;
    uchar* resData = res.data;
    int resStep = res.step;
    int channel = image.channels();
    for(int i=0;i<resrows;i++)
    {

        for(int j=0;j<rescols;j++)
        {
            float sx = (i-resrows/2)*cosalpha - (j-rescols/2)*sinalpha + image.rows / 2;
            int x = cvFloor(sx);
            float u = sx - x;
            float sy = (i-resrows/2)*sinalpha + (j-rescols/2)*cosalpha + image.cols / 2;
            int y = cvFloor(sy);
            float v = sy - y;
            if ( x>=imgrows - 1 && x<imgrows )
            {
                x = imgrows - 2;
                u = 0;
            }
            if (y >= imgcols - 1 && y<imgcols)
            {
                y = imgcols - 2;
                v = 0;
            }
            if(x>=0 && x<imgrows && y>=0 && y<imgcols)
            {
                for (int k= 0; k<channel;k++ )
                {
                    *(resData+ i*resStep + j*channel + k ) = (1-u)*(1-v)*(*(imgData + x*imgStep + y*channel + k))
                            +(1-v)*u*(*(imgData + (x+1)*imgStep + y*channel + k))
                            + v*(1-u)*(*(imgData + x*imgStep + (y+1)*channel + k))
                            +u*v*(*(imgData +(x+1)*imgStep + (y+1)*channel + k));
                }
            }
            else
            {
                for (int k= 0; k<channel;k++ )
                {
                     *(resData+ i*resStep + j*channel + k ) = (uchar)0;
                }
            }
        }
    }
    res.copyTo(image);
}


void ImageProcess::adjustHSI(cv::Mat &image, float scaleH, float scaleS, float scaleI)
{
    if (image.channels()== 3 || image.channels()== 4){

        int size = image.rows * image.cols;
        cv::MatIterator_<cv::Vec3b> it = image.begin<cv::Vec3b>();
        cv::MatIterator_<cv::Vec3b> end = image.end<cv::Vec3b>();
        for(; it != end; ++it)
        {
            float B = (float)(*it)[0];
            float G = (float)(*it)[1];
            float R = (float)(*it)[2];
            float H,S,I;
            BGR2HSI(B,G,R,H,S,I);
            //adjust hue
            H = H + scaleH;
            //adjust saturation
            if (scaleS<0)
            {
                S = S * (1+scaleS);
            }
            else
            {
                S = S + (1-S)*scaleS;
                I = I + (1-I)*scaleS;
            }
            //adjust intensity
            if (scaleI<0)
            {
                I = I * (1+scaleI);
            }
            else
            {
                I = I + (1-I)*scaleI;
                S = S - S * scaleS;
            }
            if(H>1) H -= 1;
            if(H<0) H += 1;
            /*
            if(S>1) S = 1;
            if(S<0) S = 0;
            if(I>1) I = 1;
            if(I<0) I = 0;
            */

            HSI2BGR2(H,S,I,B,G,R);

            (*it)[0] = (uchar)B;
            (*it)[1] = (uchar)G;
            (*it)[2] = (uchar)R;
        }
    }

}

void ImageProcess::test()
{
    float R = 92, G = 144, B = 181;
    float H, S , I;
    BGR2HSI(B,G,R,H,S,I);
    qDebug()<<"H:"<<H<<endl;
    qDebug()<<"S:"<<S<<endl;
    qDebug()<<"I:"<<I<<endl;
    HSI2BGR(H,S,I,B,G,R);
    qDebug()<<"B:"<<B<<endl;
    qDebug()<<"G:"<<G<<endl;
    qDebug()<<"R:"<<R<<endl;

}


/*
 * scaleH [-180,180]
 * scaleS [-1,1]
 * scaleL [-1,1]
 */
void ImageProcess::adjustHSL(cv::Mat &image, float scaleH, float scaleS, float scaleL)
{
    if (image.channels()== 3 || image.channels()== 4){
        cv::MatIterator_<cv::Vec3b> it = image.begin<cv::Vec3b>();
        cv::MatIterator_<cv::Vec3b> end = image.end<cv::Vec3b>();
        for(; it != end; ++it)
        {
            uchar B = (*it)[0];
            uchar G = (*it)[1];
            uchar R = (*it)[2];
            float H,S,L;
            cs::BGR2HSL(B,G,R,H,S,L);
            //adjust hue
            H = (int)(H + scaleH)%360;
            if( H < 0 )
                H+=360;
            //adjust saturation
            if (scaleS<0)
            {
                S = S * (1+scaleS);
            }
            else
            {
                S = S + (1-S)*scaleS;
                L = L + (1-L)*scaleS;
            }
            //adjust intensity
            if (scaleL<0)
            {
                L = L * (1+scaleL);
            }
            else
            {
                L = L + (1-L)*scaleL;
                S = S - S * scaleS;
            }
            cs::HSL2BGR(H,S,L,B,G,R);

            (*it)[0] = B;
            (*it)[1] = G;
            (*it)[2] = R;
        }
    }
}

void ImageProcess::linearTransform(cv::Mat &image, int fa, int fb, int ga, int gb)
{
    cv::MatIterator_<uchar> it = image.begin<uchar>();
    cv::MatIterator_<uchar> end = image.end<uchar>();
    float k = (float)(gb-ga)/(float)(fb-fa);
    for( ; it != end; ++it)
    {
        int tmp = *it;
        if(tmp > fb)
            tmp = gb;
        else if (tmp < fa)
            tmp = ga;
        else
            tmp = k*(tmp - fa) + ga;
        *it = tmp;
    }
    return;
}

void ImageProcess::piecewiseLinearTransform(cv::Mat &image, int fa, int fb, int ga, int gb, int gmin, int gmax)
{
    cv::MatIterator_<uchar> it = image.begin<uchar>();
    cv::MatIterator_<uchar> end = image.end<uchar>();

    float k1 = (fa==0)? 0:(float)(ga-gmin)/(float)(fa-0);
    float k2 = (fb==fa)? 0:(float)(gb-ga)/(float)(fb-fa);
    float k3 = (fb==255)? 0:(float)(gmax-gb)/(float)(255-fb);
    for( ; it != end; ++it)
    {
        int tmp = *it;
        if(tmp < fa)
            tmp = k1 * tmp + gmin;
        else if (tmp > fb)
            tmp = k3 * (tmp - fb) + gb;
        else
            tmp = k2 * (tmp - fa) + ga;
        *it = tmp;
    }
    return;
}

void ImageProcess::logarithmTransform(cv::Mat &image, int a, int b, int c)
{
    cv::MatIterator_<uchar> it = image.begin<uchar>();
    cv::MatIterator_<uchar> end = image.end<uchar>();
    for( ; it != end; ++it)
    {
        float tmp = *it;
        tmp = a + c * std::log(b+tmp);
        *it = std::min((int)tmp,255);
    }
    return;
}

void ImageProcess::expotentTransform(cv::Mat &image, float a, float b, float c)
{
    cv::MatIterator_<uchar> it = image.begin<uchar>();
    cv::MatIterator_<uchar> end = image.end<uchar>();
    //float min = std::pow(b,c*(0-a));
    //float max = std::pow(b,c*(255-a));
    //qDebug()<<"min:"<<min<<"  max:"<<max<<endl;
    for( ; it != end; ++it)
    {
        float tmp = *it;
        tmp = c * std::pow(a+tmp,b);
        *it = std::min((int)tmp,255);
    }
    return;
}

void ImageProcess::histogramEqualization(cv::Mat &image)
{
    double hist[256]={0}; //灰度直方图
    double dhist[256] = {0.00};//直方图归一化图
    double Dhist[256] = {0.00};//直方图积分图，每一个像素点
    int totalNum = image.rows*image.cols;
    cv::MatIterator_<uchar> it = image.begin<uchar>();
    cv::MatIterator_<uchar> end = image.end<uchar>();
    for( ; it != end; ++it)
    {
        hist[*it]++;
    }
    for(int i = 0;i<256;i++)//遍历直方图，得到归一化直方图和积分图
    {
        dhist[i] = hist[i]/totalNum;//得到归一化图,既概率
        for(int j = 0;j<=i;j++)
        {
            Dhist[i] = Dhist[i] + dhist[j]; //得到积分图
        }
    }
    it = image.begin<uchar>();
    for( ; it != end; ++it)
    {
        *it = (int)(Dhist[*it]*255);
    }
    return;
}


void ImageProcess::meanFilter(cv::Mat &image, int anchorx, int anchory, int kernelW, int kernelH)
{
    /*
    int kleft = anchorx;
    int kright = kernelW - anchorx - 1;
    int ktop = anchory;
    int kbottom = kernelH - anchory - 1;
    */

    //revert kernel
    anchorx = kernelW - 1 - anchorx;
    anchory = kernelH - 1 - anchory;

    int kernelNum = kernelW * kernelH;
    cv::Mat res(image.rows,image.cols,image.type());
    uchar* imgData = image.data;
    int imgStep = image.step;
    uchar* resData = res.data;
    int resStep = res.step;
    int channel = image.channels();
    for (int i=0 ; i<image.rows;i++)
    {
        for(int j=0; j < image.cols; j++)
        {
            for(int k = 0 ; k < channel; k++)
            {
                int sum = 0;
                for (int ki=0 ; ki < kernelH; ki++)
                {
                    //相对于anchor坐标+anchor坐标
                    int x = ki - anchory + i;
                    x = std::max(0,x);
                    x = std::min(image.rows-1,x);
                    for (int kj = 0 ; kj < kernelW; kj++)
                    {
                        int y = kj - anchorx + j;
                        y = std::max(0,y);
                        y = std::min(image.cols-1 ,y);
                        sum += (*(imgData + x*imgStep + y*channel + k));
                    }
                }
                int mean = sum / kernelNum;
                *(resData+ i*resStep + j*channel + k ) = mean;
            }

        }
    }
    res.copyTo(image);
}

void ImageProcess::medianFilter(cv::Mat &image, int anchorx, int anchory, int kernelW, int kernelH)
{
    //revert kernel
    anchorx = kernelW - 1 - anchorx;
    anchory = kernelH - 1 - anchory;

    int kernelNum = kernelW * kernelH;
    cv::Mat res(image.rows,image.cols,image.type());
    uchar* imgData = image.data;
    int imgStep = image.step;
    uchar* resData = res.data;
    int resStep = res.step;
    int channel = image.channels();
    for (int i=0 ; i<image.rows;i++)
    {
        for(int j=0; j < image.cols; j++)
        {
            for(int k = 0 ; k < channel; k++)
            {
                int pixArray[kernelNum];
                int index = 0;
                for (int ki=0 ; ki < kernelH; ki++)
                {
                    //相对于anchor坐标+anchor坐标
                    int x = ki - anchory + i;
                    x = std::max(0,x);
                    x = std::min(image.rows-1,x);
                    for (int kj = 0 ; kj < kernelW; kj++)
                    {
                        int y = kj - anchorx + j;
                        y = std::max(0,y);
                        y = std::min(image.cols-1 ,y);
                        pixArray[index] = (*(imgData + x*imgStep + y*channel + k));
                        index ++ ;
                    }
                }
                *(resData+ i*resStep + j*channel + k ) = cvRound(findMedian(pixArray,kernelNum));
            }

        }
    }
    res.copyTo(image);
}

void ImageProcess::filter(cv::Mat &image, float *kernel, int kernelW, int kernelH)
{
    int anchorx = kernelW / 2;
    int anchory = kernelH / 2;
    cv::Mat res(image.rows,image.cols,image.type());
    uchar* imgData = image.data;
    int imgStep = image.step;
    uchar* resData = res.data;
    int resStep = res.step;
    int channel = image.channels();
    for (int i=0 ; i<image.rows;i++)
    {
        for(int j=0; j < image.cols; j++)
        {
            for(int k = 0 ; k < channel; k++)
            {
                float sum = 0;
                for (int ki=0 ; ki < kernelH; ki++)
                {
                    //相对于anchor坐标+anchor坐标
                    int x = ki - anchory + i;
                    x = std::max(0,x);
                    x = std::min(image.rows-1,x);
                    for (int kj = 0 ; kj < kernelW; kj++)
                    {
                        int y = kj - anchorx + j;
                        y = std::max(0,y);
                        y = std::min(image.cols-1 ,y);
                        sum += (*(imgData + x*imgStep + y*channel + k)) * kernel[ki*kernelW+kj];
                    }
                }
                sum = std::min(255,(int)sum);
                sum = std::max(0,(int)sum);
                *(resData+ i*resStep + j*channel + k ) = cvRound(sum);
            }

        }
    }
    res.copyTo(image);
}

void ImageProcess::getGaussianKernel(float *kernel, int size, float sigma)
{
    int index;
    float dx2, dy2;
    float sum = 0;
    for(int i=0; i< size; i++)
    {
        dx2 = pow(i - (size - 1)/2.0, 2);
        for(int j=0; j< size; j++)
        {
            dy2 = pow(j - (size - 1)/2.0, 2);
            index = i*size + j;
            kernel[index] = exp(-(dx2 + dy2)/2/pow(sigma, 2)) / (2*CV_PI*pow(sigma, 2));
            //printf("%f\n", kernel[index]);
            sum += kernel[index];
        }
    }

    for(int k=0; k< size*size; k++)
    {
        kernel[k] = kernel[k] / sum;
    }
}

void ImageProcess::gaussianFilter(cv::Mat &image, int kernelSize, float sigma)
{
    float *kernel = new float[kernelSize*kernelSize];
    getGaussianKernel(kernel,kernelSize,sigma);
    //因为对称不需要翻转卷积核

    int kernelW = kernelSize;
    int kernelH = kernelSize;
    int anchorx = kernelSize / 2;
    int anchory = kernelSize / 2;
    cv::Mat res(image.rows,image.cols,image.type());
    uchar* imgData = image.data;
    int imgStep = image.step;
    uchar* resData = res.data;
    int resStep = res.step;
    int channel = image.channels();
    for (int i=0 ; i<image.rows;i++)
    {
        for(int j=0; j < image.cols; j++)
        {
            for(int k = 0 ; k < channel; k++)
            {
                float sum = 0;
                for (int ki=0 ; ki < kernelH; ki++)
                {
                    //相对于anchor坐标+anchor坐标
                    int x = ki - anchory + i;
                    x = std::max(0,x);
                    x = std::min(image.rows-1,x);
                    for (int kj = 0 ; kj < kernelW; kj++)
                    {
                        int y = kj - anchorx + j;
                        y = std::max(0,y);
                        y = std::min(image.cols-1 ,y);
                        sum += (*(imgData + x*imgStep + y*channel + k)) * kernel[ki*kernelSize+kj];
                    }
                }
                sum = std::min(255,(int)sum);
                sum = std::max(0,(int)sum);
                *(resData+ i*resStep + j*channel + k ) = cvRound(sum);
            }

        }
    }
    res.copyTo(image);
    delete kernel;
}

void ImageProcess::sobel(cv::Mat &image)
{
    float Gx[9] = {-1,0,1,-2,0,2,-1,0,1};
    float Gy[9] = {-1,-2,-1,0,0,0,1,2,1};
    int kernelSize = 3;
    cv::Mat res(image.rows,image.cols,image.type());
    uchar* imgData = image.data;
    int imgStep = image.step;
    uchar* resData = res.data;
    int resStep = res.step;
    int channel = image.channels();
    int kernelW = kernelSize;
    int kernelH = kernelSize;
    int anchorx = kernelSize / 2;
    int anchory = kernelSize / 2;
    for (int i=0 ; i<image.rows;i++)
    {
        for(int j=0; j < image.cols; j++)
        {
            for(int k = 0 ; k < channel; k++)
            {
                int gx = 0;
                int gy = 0;
                for (int ki=0 ; ki < kernelH; ki++)
                {
                    //相对于anchor坐标+anchor坐标
                    int x = ki - anchory + i;
                    x = std::max(0,x);
                    x = std::min(image.rows-1,x);
                    for (int kj = 0 ; kj < kernelW; kj++)
                    {
                        int y = kj - anchorx + j;
                        y = std::max(0,y);
                        y = std::min(image.cols-1 ,y);
                        gx += (*(imgData + x*imgStep + y*channel + k)) * Gx[ki*kernelSize+kj];
                        gy += (*(imgData + x*imgStep + y*channel + k)) * Gy[ki*kernelSize+kj];
                    }
                }
                int sum = std::fabs(gx)+std::fabs(gy);
                sum = std::min(255,(int)sum);
                sum = std::max(0,(int)sum);
                *(resData+ i*resStep + j*channel + k ) = sum;
                //*(imgData + i*imgStep + j*channel + k) = std::fabs(*(imgGxData + i*imgStep + j*channel + k))+
                 //       std::fabs(*(imgGyData + i*imgStep + j*channel + k));
                //*(imgData + i*imgStep + j*channel + k) = sqrt(pow(*(imgGxData + i*imgStep + j*channel + k),2)+
                //                               pow(*(imgGyData + i*imgStep + j*channel + k),2));
            }

        }
    }
    res.copyTo(image);
}

void ImageProcess::laplacian(cv::Mat &image)
{
    float kernel[9] = {1,1,1,1,-8,1,1,1,1};
    filter(image,kernel,3,3);
}

void ImageProcess::canny(cv::Mat &image, int t1, int t2)
{
    float Gx[9] = {1,0,-1,2,0,-2,1,0,-1};
    float Gy[9] = {-1,-2,-1,0,0,0,1,2,1};
    int kernelSize = 3;
    float *Gtheta = new float[image.rows * image.cols];
    cv::Mat res(image.rows,image.cols,image.type());
    uchar* imgData = image.data;
    int imgStep = image.step;
    uchar* resData = res.data;
    int resStep = res.step;
    int channel = image.channels();
    int kernelW = kernelSize;
    int kernelH = kernelSize;
    int anchorx = kernelSize / 2;
    int anchory = kernelSize / 2;
    for (int i=0 ; i<image.rows;i++)
    {
        for(int j=0 ; j < image.cols; j++)
        {
            for(int k = 0 ; k < channel; k++)
            {
                int gx = 0;
                int gy = 0;
                for (int ki=0 ; ki < kernelH; ki++)
                {
                    //相对于anchor坐标+anchor坐标
                    int x = ki - anchory + i;
                    x = std::max(0,x);
                    x = std::min(image.rows-1,x);
                    for (int kj = 0 ; kj < kernelW; kj++)
                    {
                        int y = kj - anchorx + j;
                        y = std::max(0,y);
                        y = std::min(image.cols-1 ,y);
                        gx += (*(imgData + x*imgStep + y*channel + k)) * Gx[ki*kernelSize+kj];
                        gy += (*(imgData + x*imgStep + y*channel + k)) * Gy[ki*kernelSize+kj];
                    }
                }
                int sum = std::fabs(gx)+std::fabs(gy);
                sum = std::min(255,(int)sum);
                sum = std::max(0,(int)sum);
                *(resData+ i*resStep + j*channel + k ) = sum;
                float theta;
                if ( gy == 0 )
                    theta = 0;
                else if ( gx == 0 )
                    theta = 90;
                else
                    theta = std::atan2(gy,gx) * 180.0 / CV_PI;
                Gtheta[i*image.cols + j] = theta;
            }

        }
    }
    //非极大值抑制， theta近似到0，45，90，135
    //theta得出的坐标系与实际图像坐标系的y轴是相反的！
    int *flag = new int[image.rows * image.cols];    //0:非极大值 1:极大值  2:强边界点 3:非边界点 4:弱边界点  5:检测过

    for (int i=0 ; i<image.rows;i++)
    {
        for(int j=0; j < image.cols; j++)
        {
            for(int k = 0 ; k < channel; k++)
            {
                float theta = Gtheta[i*image.cols + j] ;
                int tmp1;
                int tmp2;
                int tmp = *(resData+ i*resStep + j*channel + k );
                if (i==0 || i== image.rows-1 || j==0 || j==image.cols-1)   //忽略边缘
                {
                    flag[i*image.cols + j] = 0;
                    *(imgData+ i*imgStep + j*channel + k ) = 0;
                    continue;
                }
                else if ( (theta>-22.5 && theta<= 22.5) || theta > 157.5 || theta <= -157.5) //0
                {
                    tmp1 = *(resData+ i*resStep + (j+1)*channel + k );
                    tmp2 = *(resData+ i*resStep + (j-1)*channel + k );
                }
                if((theta > 22.5 && theta <= 67.5) || (theta > -157.5 && theta <= -112.5) )   //45
                {
                    tmp1 = *(resData+ (i-1)*resStep + (j+1)*channel + k );
                    tmp2 = *(resData+ (i+1)*resStep + (j-1)*channel + k );
                }
                else if ((theta > 67.5 && theta <= 112.5) || (theta > -112.5 && theta <= -67.5) )  //90
                {
                    tmp1 = *(resData+ (i-1)*resStep + j*channel + k );
                    tmp2 = *(resData+ (i+1)*resStep + j*channel + k );

                }
                else if((theta > 112.5 && theta <= 157.5) || (theta > -67.5 && theta <= -22.5) ) //135
                {
                    tmp1 = *(resData+ (i-1)*resStep + (j-1)*channel + k );
                    tmp2 = *(resData+ (i+1)*resStep + (j+1)*channel + k );
                }
                if(tmp >= tmp1 && tmp >= tmp2)
                {
                    flag[i*image.cols + j] = 1;
                    *(imgData+ i*imgStep + j*channel + k ) = *(resData+ i*resStep + j*channel + k );
                }
                else
                {
                    flag[i*image.cols + j] = 0;
                    *(imgData+ i*imgStep + j*channel + k ) = 0;
                }

            }
        }
    }
    //双阈值边缘检测
    for (int i=0 ; i<image.rows;i++)
    {
        for(int j=0; j < image.cols; j++)
        {
            for(int k = 0 ; k < channel; k++)
            {
                int tmp = *(imgData+ i*imgStep + j*channel + k );
                if (tmp >= t2 && flag[i*image.cols+j] == 1)  //强边界点
                {
                    *(imgData+ i*imgStep + j*channel + k ) = 255;
                    flag[i*image.cols + j] = 2;
                }

                else if (tmp>=t1 && flag[i*image.cols+j] == 1)  //弱边界点
                {
                    flag[i*image.cols + j] = 4;
                }
                else //非边界点
                {
                    *(imgData+ i*imgStep + j*channel + k ) = 0;
                    flag[i*image.cols + j] = 3;
                }
            }
        }
    }
    for (int i=0 ; i<image.rows;i++)
    {
        for(int j=0; j < image.cols; j++)
        {
            for(int k = 0 ; k < channel; k++)
            {
                if(flag[i*image.cols+j] == 4 || flag[i*image.cols + j] == 2)
                    cannyTraceEdge(image,flag,i,j,t1,t2);
            }
         }
      }
    delete flag;
    delete Gtheta;
}

void ImageProcess::cannyTraceEdge(cv::Mat &image,int *flag, int i, int j, int t1, int t2)
{
    if (flag[i*image.cols + j] == 5 || flag[i*image.cols + j] == 3)
        return;
    uchar *imgData = image.data;
    int imgStep = image.step;
    int iNum[8] = {1,1,0,-1,-1,-1,0,1};
    int jNum[8] = {0,1,1,1,0,-1,-1,-1};
    for (int k = 0 ; k < 8 ; k++)
    {
        int tmpi = i + iNum[k];
        int tmpj = j + jNum[k];
        if (tmpi < 0 || tmpj < 0 || tmpi > image.rows-1 || tmpj > image.cols-1)
            continue;
        if (flag[tmpi*image.cols+tmpj] == 5)
            continue;
        int tmp = *(imgData+ tmpi*imgStep + tmpj);
        if( tmp >= t1 && flag[tmpi*image.cols+tmpj] == 4)
        {
            *(imgData+ tmpi*imgStep + tmpj) = 255;
            flag[tmpi*image.cols+tmpj] = 2;
            cannyTraceEdge(image,flag,tmpi,tmpj,t1,t2);

        }
    }
    flag[i*image.cols + j] = 5;

}

void ImageProcess::houghLine(cv::Mat &image, double rho, double theta, int threshold)
{
    cv::Mat res = image.clone();
    cv::cvtColor(res,res,CV_GRAY2BGR);
    double maxRho = std::sqrt(std::pow(image.rows - 1, 2) + std::pow(image.cols - 1, 2));
    int numRho = cvRound(maxRho / rho);
    int numAngle = cvRound(2 * CV_PI / theta);
    double *sinTable = new double[numAngle];
    double *cosTable = new double[numAngle];
    double angle = 0;
    for (int i = 0 ; i < numAngle; i++)
    {  
        sinTable[i] = std::sin(angle);
        cosTable[i] = std::cos(angle);
        angle += theta;
    }
    int accum[numRho][numAngle] = {0};
    for (int i=0 ; i<image.rows;i++)
    {
        for(int j=0; j < image.cols; j++)
        {
            int tmp = image.at<uchar>(i,j);
            if (tmp == 255 )
            {
                for (int k = 0 ; k < numAngle; k++)
                {
                    int d = cvRound((j * cosTable[k] + i * sinTable[k]) / rho );
                    if ( d > 0 )
                        accum[d][k]++;
                }
            }
        }
    }
    for (int d = 0 ; d < numRho; d++)
    {
        for (int t = 0; t < numAngle; t++)
        {
            if (accum[d][t] >= threshold)
            {
               //draw lines
                int tmprho = d * rho;
                double k, b;
                int x,y;
                if ( sinTable[t] == 0 )
                {
                    for ( y = 0; y < res.rows; y++)
                    {
                        res.at<cv::Vec3b>(y,tmprho) = cv::Vec3b(0,255,0);
                    }
                }
                else
                {
                    k = - cosTable[t] / sinTable[t];
                    b = tmprho / sinTable[t];
                    if (std::fabs(k) <= 1)
                    {
                        for (x=0;x<res.cols;x++)
                        {
                            y = cvRound(k*x+b);
                            if (y>=0&&y<res.rows)
                            {
                                res.at<cv::Vec3b>(y,x) = cv::Vec3b(0,255,0);
                            }
                        }
                    }
                    else
                    {
                        for(y=0;y<res.rows;y++)
                        {
                            x = cvRound(y/k - b/k);
                            if(x>=0 && x<res.cols)
                            {
                                res.at<cv::Vec3b>(y,x) = cv::Vec3b(0,255,0);
                            }
                        }
                    }
                }

            }
        }
    }
    res.copyTo(image);
}

void ImageProcess::houghCircle(cv::Mat &image, int rmin, int rmax, int threshold)
{
    cv::Mat res = image.clone();
    cv::cvtColor(res,res,CV_GRAY2BGR);
    int numR = rmax - rmin;
    int numAngle = 360;
    double *sinTable = new double[numAngle];
    double *cosTable = new double[numAngle];
    double angle = 0;
    for (int i = 0 ; i < numAngle; i++)
    {
        sinTable[i] = std::sin(i*CV_PI/180);
        cosTable[i] = std::cos(i*CV_PI/180);
    }
    //int accum[image.rows][image.cols][numR] = {0};
    int *accum = new int[image.rows * image.cols * numR];
    for (int y=0 ; y<image.rows;y++)
    {
        for (int x=0; x<image.cols;x++)
        {
            for (int r=rmin ; r<rmax; r++)
            {
                for (int angle = 0; angle < numAngle; angle ++)
                {
                    if(image.at<uchar>(y,x) == 255)
                    {
                        int a = cvRound(x - r * cosTable[angle]);
                        int b = cvRound(y - r * sinTable[angle]);
                        if ( a>0 && a<image.cols && b>0 && b < image.rows)
                        {
                            accum[b*(image.cols*numR)+a*numR + r-rmin]++;
                        }
                    }
                }
            }
        }
    }
    for (int b = 0 ; b<image.rows; b++ )
    {
        for (int a=0 ; a<image.cols; a++)
        {
            for (int r=rmin; r<rmax; r++)
            {
                if( accum[b*(image.cols*numR)+a*numR + r-rmin]>threshold)
                {
                    for(int angle = 0 ; angle < numAngle; angle++)
                    {
                        int x = cvRound(a + r * cosTable[angle]);
                        int y = cvRound(b + r * sinTable[angle]);
                        if ( x> 0&& x<image.cols && y>0 && y < image.rows)
                        {
                            res.at<cv::Vec3b>(y,x) = cv::Vec3b(0,255,0);
                        }
                    }
                }
            }
        }
    }
    res.copyTo(image);
}

void ImageProcess::addWeighted(cv::Mat &image1, cv::Mat &image2, int x1, int y1, int x2, int y2,double alpha)
{
    int width2 = x2 - x1;
    int height2 = y2 - y1;
    if (width2 != image2.cols &&  height2 != image2.rows)
    {
        double scalex2 = (double)width2 / image2.cols;
        double scaley2 = (double)height2 / image2.rows;
        bilinearResize(image2, scalex2, scaley2);
    }
    qDebug()<<"image2 width:" << image2.cols << " height: "<<image2.rows<<endl;
    uchar* img1Data = image1.data;
    int img1Step = image1.step;
    uchar* img2Data = image2.data;
    int img2Step = image2.step;
    int channel = std::min(image1.channels(),image2.channels());
    int img1Channel = image1.channels();
    int img2Channel = image2.channels();
    for (int x = x1 ; x < x2; x++)
    {
        for (int y = y1; y < y2; y++)
        {
            for(int k = 0; k <channel; k++)
            {
                double tmp =(1-alpha)* (*(img1Data+y*img1Step+x*img1Channel+k));
                tmp += alpha * (*(img2Data+(y-y1)*img2Step+(x-x1)*img2Channel+k));
                int tmpint = cvFloor(tmp);
                tmpint = (tmp>255)?255:tmpint;
                *(img1Data+y*img1Step+x*img1Channel+k) = tmpint;
            }
        }
    }

}

void ImageProcess::add(cv::Mat &image1, cv::Mat &image2, int x1, int y1, int x2, int y2)
{
    int width2 = x2 - x1 + 1 ;
    int height2 = y2 - y1 + 1;
    if (width2 != image2.cols &&  height2 != image2.rows)
    {
        double scalex2 = (double)width2 / image2.cols;
        double scaley2 = (double)height2 / image2.rows;
        bilinearResize(image2, scalex2, scaley2);
    }
    qDebug()<<"image2 width:" << image2.cols << " height: "<<image2.rows<<endl;
    uchar* img1Data = image1.data;
    int img1Step = image1.step;
    uchar* img2Data = image2.data;
    int img2Step = image2.step;
    int channel = std::min(image1.channels(),image2.channels());
    int img1Channel = image1.channels();
    int img2Channel = image2.channels();
    for (int x = x1 ; x <= x2; x++)
    {
        for (int y = y1; y <= y2; y++)
        {
            for(int k = 0; k <channel; k++)
            {
                double tmp =(*(img1Data+y*img1Step+x*img1Channel+k));
                tmp += (*(img2Data+(y-y1)*img2Step+(x-x1)*img2Channel+k));
                int tmpint = cvFloor(tmp);
                tmpint = (tmp>255)?255:tmpint;
                *(img1Data+y*img1Step+x*img1Channel+k) = tmpint;
            }
        }
    }
}

void ImageProcess::subtract(cv::Mat &image1, cv::Mat &image2, int x1, int y1, int x2, int y2)
{
    int width2 = x2 - x1 + 1 ;
    int height2 = y2 - y1 + 1;
    if (width2 != image2.cols &&  height2 != image2.rows)
    {
        double scalex2 = (double)width2 / image2.cols;
        double scaley2 = (double)height2 / image2.rows;
        bilinearResize(image2, scalex2, scaley2);
    }
    qDebug()<<"image2 width:" << image2.cols << " height: "<<image2.rows<<endl;
    uchar* img1Data = image1.data;
    int img1Step = image1.step;
    uchar* img2Data = image2.data;
    int img2Step = image2.step;
    int channel = std::min(image1.channels(),image2.channels());
    int img1Channel = image1.channels();
    int img2Channel = image2.channels();
    for (int x = x1 ; x <= x2; x++)
    {
        for (int y = y1; y <= y2; y++)
        {
            for(int k = 0; k <channel; k++)
            {
                int tmp =(*(img1Data+y*img1Step+x*img1Channel+k));
                tmp -= (*(img2Data+(y-y1)*img2Step+(x-x1)*img2Channel+k));
                int tmpint = cvFloor(tmp);
                tmpint = (tmp<0)?0:tmpint;
                *(img1Data+y*img1Step+x*img1Channel+k) = tmpint;
            }
        }
    }
}

void ImageProcess::multiply(cv::Mat &image1, cv::Mat &image2, int x1, int y1, int x2, int y2)
{
    int width2 = x2 - x1 + 1 ;
    int height2 = y2 - y1 + 1;
    if (width2 != image2.cols &&  height2 != image2.rows)
    {
        double scalex2 = (double)width2 / image2.cols;
        double scaley2 = (double)height2 / image2.rows;
        bilinearResize(image2, scalex2, scaley2);
    }
    uchar* img1Data = image1.data;
    int img1Step = image1.step;
    uchar* img2Data = image2.data;
    int img2Step = image2.step;
    int channel = std::min(image1.channels(),image2.channels());
    int img1Channel = image1.channels();
    int img2Channel = image2.channels();
    for (int x = x1 ; x <= x2; x++)
    {
        for (int y = y1; y <= y2; y++)
        {
            for(int k = 0; k <channel; k++)
            {
                int tmp =(*(img1Data+y*img1Step+x*img1Channel+k));
                tmp *= (*(img2Data+(y-y1)*img2Step+(x-x1)*img2Channel+k));
                tmp = tmp % 256;
                *(img1Data+y*img1Step+x*img1Channel+k) = tmp;
            }
        }
    }
}

void ImageProcess::cut(cv::Mat &image, int x1, int y1, int x2, int y2)
{
    int width2 = x2 - x1 + 1 ;
    int height2 = y2 - y1 + 1;
    cv::Mat res(height2,width2,image.type());
    uchar* imgData = image.data;
    int imgStep = image.step;
    uchar* resData = res.data;
    int resStep = res.step;
    int channel = image.channels();
    for (int x = x1 ; x <= x2; x++)
    {
        for (int y = y1; y <= y2; y++)
        {
            for(int k = 0; k <channel; k++)
            {
                *(resData+(y-y1)*resStep+(x-x1)*channel+k)=*(imgData+y*imgStep+x*channel+k);

            }
        }
    }
    res.copyTo(image);
}

void ImageProcess::dilation(cv::Mat &image, cv::Mat se, int anchorx, int anchory)
{
    //reverse se
    anchorx = se.cols - 1 - anchorx;
    anchory = se.rows - 1 - anchory;
    reverseMat(se);
    cv::Mat res = image.clone();
    for (int i=0 ; i<image.rows;i++)
    {
        for(int j=0; j < image.cols; j++)
        {
            int max = 0;
            for (int ki=0 ; ki < se.rows ; ki++)
            {
                //相对于anchor坐标+anchor坐标
                int x = ki - anchory + i;
                x = std::max(0,x);
                x = std::min(image.rows-1,x);
                for (int kj = 0 ; kj < se.cols ; kj++)
                {
                    int y = kj - anchorx + j;
                    y = std::max(0,y);
                    y = std::min(image.cols-1 ,y);
                    int tmp = image.at<uchar>(x,y);
                    if (tmp > max && se.at<uchar>(ki,kj)==255)
                        max = tmp;
                }
            }
            res.at<uchar>(i,j) = max;
        }
    }
    res.copyTo(image);
}

void ImageProcess::erosion(cv::Mat &image, cv::Mat se, int anchorx, int anchory)
{
    cv::Mat res = image.clone();
    for (int i=0 ; i<image.rows;i++)
    {
        for(int j=0; j < image.cols; j++)
        {
            int min = 255;
            for (int ki=0 ; ki < se.rows ; ki++)
            {
                //相对于anchor坐标+anchor坐标
                int x = ki - anchory + i;
                x = std::max(0,x);
                x = std::min(image.rows-1,x);
                for (int kj = 0 ; kj < se.cols ; kj++)
                {
                    int y = kj - anchorx + j;
                    y = std::max(0,y);
                    y = std::min(image.cols-1 ,y);
                    int tmp = image.at<uchar>(x,y);
                    if (tmp < min && se.at<uchar>(ki,kj)==255)
                        min = tmp;
                }
            }
            res.at<uchar>(i,j) = min;
        }
    }
    res.copyTo(image);
}

void ImageProcess::opening(cv::Mat &image, cv::Mat se, int anchorx, int anchory)
{
    erosion(image,se,anchorx,anchory);
    dilation(image,se,anchorx,anchory);
}

void ImageProcess::closing(cv::Mat &image, cv::Mat se, int anchorx, int anchory)
{
    dilation(image,se,anchorx,anchory);
    erosion(image,se,anchorx,anchory);
}

void ImageProcess::reverseMat(cv::Mat &image)
{
    cv::Mat res(image.rows,image.cols,image.type());
    uchar* imgData = image.data;
    int imgStep = image.step;
    uchar* resData = res.data;
    int resStep = res.step;
    int channel = image.channels();
    for (int i=0 ; i<res.rows;i++)
    {
        for(int j=0; j < res.cols; j++)
        {
            for(int k = 0 ; k < channel; k++)
            {
                *(resData + i*resStep + j*channel + k) = *(imgData + (image.rows -1 - i)*imgStep + (image.cols - 1 -j)*channel + k );
            }
        }
    }
    res.copyTo(image);
}

void printse(cv::Mat se)
{
    qDebug()<<"[";
    for(int i=0;i<se.rows;i++)
    {
        qDebug()<<"i:"<< i << " ";
        for (int j=0 ; j<se.cols;j++)
        {
            qDebug()<< se.at<uchar>(i,j);
        }
        qDebug()<<endl;
    }
    qDebug()<<"]"<<endl;
}
void ImageProcess::hitOrMiss(cv::Mat &image, cv::Mat se, int anchorx, int anchory)
{

    cv::Mat res1 = image.clone();
    erosion(res1,se,anchorx,anchory);
    //cv::imshow("res1",res1);
    cv::Mat res2 = binaryComplement(image);
    cv::Mat se2 = binaryComplement(se);
    erosion(res2,se2 ,anchorx,anchory);
    //cv::imshow("res2",res2);
    image = binaryIntersection(res1,res2);
    //erosion(image,se,anchorx,anchory);
}

void ImageProcess::thin(cv::Mat &image, cv::Mat se, int anchorx, int anchory)
{
    cv::Mat res1 = image.clone();
    hitOrMiss(res1,se,anchorx,anchory);
    res1 = binaryComplement(res1);
    cv::Mat res2 = image.clone();
    image = binaryIntersection(res1,res2);
    //qDebug()<<"thin"<<endl;
}

void ImageProcess::thin(cv::Mat &image)
{
    //0:background 255:foreground 128:not care
    uchar data1[9] = {0,0,0,128,255,128,255,255,255};
    cv::Mat se1(3,3,CV_8UC1, data1);
    thin(image,se1,1,1);
    uchar data2[9] = {128,0,0,255,255,0,255,255,128};
    cv::Mat se2(3,3,CV_8UC1, data2);
    thin(image,se2,1,1);
    uchar data3[9] = {255,128,0,255,255,0,255,128,0};
    cv::Mat se3(3,3,CV_8UC1, data3);
    thin(image,se3,1,1);
    uchar data4[9] = {255,255,128,255,255,0,128,0,0};
    cv::Mat se4(3,3,CV_8UC1, data4);
    thin(image,se4,1,1);
    uchar data5[9] = {255,255,255,128,255,128,0,0,0};
    cv::Mat se5(3,3,CV_8UC1, data5);
    thin(image,se5,1,1);
    uchar data6[9] = {128,255,255,0,255,255,0,0,128};
    cv::Mat se6(3,3,CV_8UC1, data6);
    thin(image,se6,1,1);
    uchar data7[9] = {0,128,255,0,255,255,0,128,255};
    cv::Mat se7(3,3,CV_8UC1, data7);
    thin(image,se7,1,1);
    uchar data8[9] = {0,0,128,0,255,255,128,255,255};
    cv::Mat se8(3,3,CV_8UC1, data8);
    thin(image,se8,1,1);
}

void ImageProcess::thicken(cv::Mat &image, cv::Mat se, int anchorx, int anchory)
{
    cv::Mat res1 = image.clone();
    hitOrMiss(res1,se,anchorx,anchory);
    cv::Mat res2 = image.clone();
    image = binaryUnion(res1,res2);
}

void ImageProcess::thicken(cv::Mat &image)
{
    //oposite operation to thin
    image = binaryComplement(image);
    thin(image);
    image = binaryComplement(image);
}

void ImageProcess::skeletonize(cv::Mat &image, cv::Mat se, int anchorx, int anchory, int maxk, std::vector<cv::Mat> &s)
{
    cv::Mat tmp = image.clone();
    cv::Mat res = cv::Mat::zeros(image.rows,image.cols,CV_8UC1);
    int k = 0;
    while(!isEmpty(tmp) && k < maxk)
    {
        tmp = image.clone();
        for (int i = 0 ; i < k ; i++)
        {
            erosion(tmp,se,anchorx,anchory);
        }
        cv::Mat res1 = tmp.clone();
        cv::Mat res2 = tmp.clone();
        opening(res2,se,anchorx,anchory);
        res2 = binaryComplement(res2);
        cv::Mat tmpres = binaryIntersection(res1,res2);
        s.push_back(tmpres);
        res = binaryUnion(res,tmpres);

        k++;
    }
    res.copyTo(image);
    qDebug()<<"k:"<<k<<endl;
}

void ImageProcess::skeletonReconstruct(cv::Mat &image, cv::Mat se, int anchorx, int anchory,std::vector<cv::Mat> &s)
{
    cv::Mat res = cv::Mat::zeros(image.rows,image.cols,CV_8UC1);
    int k;
    for ( k = 0 ; k < s.size(); k++)
    {
        cv::Mat tmpres = s[k];
        for (int i = 0; i<k;i++)
        {
            dilation(tmpres,se,anchorx,anchory);
        }
        res = binaryUnion(res,tmpres);
    }

    res.copyTo(image);
    qDebug()<<"k:"<<k<<endl;
}

cv::Mat ImageProcess::binaryComplement(cv::Mat image)
{
    cv::Mat res(image.rows,image.cols,image.type());

    for (int i=0 ; i<image.rows;i++)
    {
        for(int j=0; j < image.cols; j++)
        {
            uchar tmp = image.at<uchar>(i,j);
            if (tmp == 255)
                res.at<uchar>(i,j) = 0;
            else if (tmp == 0)
                res.at<uchar>(i,j) = 255;
            else
                res.at<uchar>(i,j) = 128;
        }
    }
    return res;
}

cv::Mat ImageProcess::binaryIntersection(cv::Mat image1, cv::Mat image2)
{
    if (image1.rows!=image2.rows || image1.cols != image2.cols)
    {
        qDebug()<<"binaryIntersection: size are not equal" <<endl;
    }
    cv::Mat res(image1.rows,image1.cols,image1.type());

    for (int i=0 ; i<res.rows;i++)
    {
        for(int j=0; j < res.cols; j++)
        {
            if(image1.at<uchar>(i,j) == 255 && image2.at<uchar>(i,j) == 255)
                res.at<uchar>(i,j) = 255;
            else
                res.at<uchar>(i,j) = 0;
        }
    }
    return res;
}

cv::Mat ImageProcess::binaryUnion(cv::Mat image1, cv::Mat image2)
{
    if (image1.rows!=image2.rows || image1.cols != image2.cols)
    {
        qDebug()<<"binaryIntersection: size are not equal" <<endl;
    }
    cv::Mat res(image1.rows,image1.cols,image1.type());

    for (int i=0 ; i<res.rows;i++)
    {
        for(int j=0; j < res.cols; j++)
        {
            if(image1.at<uchar>(i,j) == 255 || image2.at<uchar>(i,j) == 255)
                res.at<uchar>(i,j) = 255;
            else
                res.at<uchar>(i,j) = 0;
        }
    }
    return res;
}

cv::Mat ImageProcess::grayIntersection(cv::Mat image1, cv::Mat image2)
{
    if (image1.rows!=image2.rows || image1.cols != image2.cols)
    {
        qDebug()<<"binaryIntersection: size are not equal" <<endl;
    }
    cv::Mat res(image1.rows,image1.cols,image1.type());

    for (int i=0 ; i<res.rows;i++)
    {
        for(int j=0; j < res.cols; j++)
        {
            res.at<uchar>(i,j) = std::min(image1.at<uchar>(i,j),image2.at<uchar>(i,j));
        }
    }
    return res;
}

cv::Mat ImageProcess::grayUnion(cv::Mat image1, cv::Mat image2)
{
    if (image1.rows!=image2.rows || image1.cols != image2.cols)
    {
        qDebug()<<"binaryIntersection: size are not equal" <<endl;
    }
    cv::Mat res(image1.rows,image1.cols,image1.type());

    for (int i=0 ; i<res.rows;i++)
    {
        for(int j=0; j < res.cols; j++)
        {
            res.at<uchar>(i,j) = std::max(image1.at<uchar>(i,j),image2.at<uchar>(i,j));
        }
    }
    return res;
}

bool ImageProcess::isEmpty(cv::Mat image)
{
    bool flag = true;
    for (int i=0 ; i<image.rows;i++)
    {
        for(int j=0; j < image.cols; j++)
        {
            if(image.at<uchar>(i,j) > 0)
            {
                flag = false;
                return flag;
            }
        }
    }
    return flag;
}

bool ImageProcess::isBinaryImage(cv::Mat image)
{
    bool flag = true;
    for (int i=0 ; i<image.rows;i++)
    {
        for(int j=0; j < image.cols; j++)
        {
            if(image.at<uchar>(i,j) !=0 && image.at<uchar>(i,j)!=255)
            {
                flag = false;
                return flag;
            }
        }
    }
    return flag;
}

/*
 * Suitable SE for different distance metrics:
 * A square SE gives the chessboard distance transform
 * A cross shaped SE gives the city block distance transform
 * A disc shaped SE gives the Euclidean distance transform.
*/
void ImageProcess::distanceTransform(cv::Mat &image, cv::Mat se, int anchorx, int anchory)
{
    cv::Mat copy = image.clone();
    cv::Mat res(image.rows,image.cols,image.type());
    int resData[image.rows*image.cols]={0};
    int d = 1;
    while(!isEmpty(copy))
    {
        image = copy.clone();
        erosion(copy,se,anchorx,anchory);
        for (int i = 0; i< image.rows;i++)
        {
            for (int j = 0; j <image.cols; j++)
            {
                if(image.at<uchar>(i,j)!=copy.at<uchar>(i,j))
                {
                    resData[i*res.cols+j] = d;
                }
            }
        }
        d++;
    }
    //将res各个像素的值转换为0-255
    for (int i = 0; i< image.rows;i++)
    {
        for (int j = 0; j <image.cols; j++)
        {
            res.at<uchar>(i,j) = (int)((double)resData[i*res.cols+j]/(double)d * 255.0);

        }
    }
    res.copyTo(image);
}

void ImageProcess::binaryGeoErosion(cv::Mat &image, cv::Mat mask, cv::Mat se, int anchorx, int anchory)
{
    erosion(image,se,anchorx,anchory);
    image = binaryUnion(image,mask);
}

void ImageProcess::binaryGeoDilation(cv::Mat &image, cv::Mat mask, cv::Mat se, int anchorx, int anchory)
{
    dilation(image,se,anchorx,anchory);
    image = binaryIntersection(image,mask);
}

void ImageProcess::grayGeoErosion(cv::Mat &image, cv::Mat mask, cv::Mat se, int anchorx, int anchory)
{
    erosion(image,se,anchorx,anchory);
    image = grayUnion(image,mask);
}

void ImageProcess::grayGeoDilation(cv::Mat &image, cv::Mat mask, cv::Mat se, int anchorx, int anchory)
{
    dilation(image,se,anchorx,anchory);
    image = grayIntersection(image,mask);
}

bool pairCompare(const std::pair<cv::Point,int> &p1, const std::pair<cv::Point,int> &p2 )
{
    return p1.second < p2.second;
}

void ImageProcess::watershed(cv::Mat &image)
{
    //根据各点灰度值排序
    sobel(image);
    std::vector<std::pair<cv::Point,int>> pixels;
    for (int i = 0 ; i < image.rows; i++)
    {
        for (int j = 0 ; j < image.cols; j++)
        {
            int grad = image.at<uchar>(i,j);
            cv::Point p(i,j);
            std::pair<cv::Point,int> pa(p,grad);
            pixels.push_back(pa);
        }
    }
    sort(pixels.begin(),pixels.end(),pairCompare);
    int grad = pixels[0].second;
    int gradMax = pixels[pixels.size()-1].second;
    qDebug()<<"grad:"<<grad;
    qDebug()<<"gradMax"<<gradMax;
    //遍历灰度层
    int mask[image.rows*image.cols]={ -1 };   //mask:-2 init:-1 wshed:0 inqe:-3
    int iNum[8] = {1,1,0,-1,-1,-1,0,1};
    int jNum[8] = {0,1,1,1,0,-1,-1,-1};
    int index = 0;
    int curlab = 0;
    int heightIndex1 = 0;
    int heightIndex2 = 0;
    std::queue<cv::Point> pixelQueue;
    for(; grad<=gradMax ; grad++ )
    {
        //加入梯度值为grad的点
        bool flag = true;
        while(flag)
        {
            flag = false;
            if(pixels[index].second == grad)
            {
                pixelQueue.push(pixels[index].first);
                flag = true;
                index++;
            }
        }
        while(!pixelQueue.empty())
        {
            cv::Point p = pixelQueue.front();
            mask[p.x*image.cols + p.y] = 1;
            for (int k = 0 ; k < 8 ; k++)
            {
                int tmpi = p.x + iNum[k];
                int tmpj = p.y + jNum[k];
                if (tmpi < 0 || tmpj < 0 || tmpi > image.rows-1 || tmpj > image.cols-1)
                    continue;
            }
        }
    }

}


