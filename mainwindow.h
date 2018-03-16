#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QAction>
#include <QMouseEvent>
#include <QButtonGroup>
#include "imageprocess.h"
#include "geometrydialog.h"
#include "contrastdialog.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QLabel *imageLabel;
    QLabel *statuLabel;
    QImage *img;
    QButtonGroup *channelButtonGroup;
    ImageProcess *imageProcess;
    GeometryDialog *geometryDialog;
    ContrastDialog *contrastDialog;
    void showImage (QImage *img);
    int* getSE();
    bool isBinaryImage(QImage img);

protected:
    void mouseMoveEvent(QMouseEvent *event);

signals:
    void drawHistogram(QImage *img);

private slots:
    void open();
    void saveAs();
    void selectChannel(int id, bool checked);
    void redImage();
    void greenImage();
    void blueImage();
    void otsuBinary();
    void twoThresholdBinary();
    void grayImage();
    void undo();
    void save();
    void initialize();
    void fitWindow();
    void pageTurning();
    void openGeometryDialog();
    void openContrastDialog();
    void adjustHSL();
    void linearTransform(int fa, int fb, int ga, int gb);
    void piecewiseLinearTransform(int fa, int fb, int ga, int gb, int gmin, int gmax);
    void logarithmTransform(int a, int b,int c);
    void expotentTransform(float a, float b, float c);
    void histogramEqualization();
    void meanFilter();
    void medianFilter();
    void gaussianFilter();
    void sobel();
    void laplacian();
    void canny();
    void houghLine();
    void houghCircle();
    void add();
    void addWeighted();
    void subtract();
    void multiply();
    void cut();
    void dilation();
    void erosion();
    void opening();
    void closing();
    void hitOrMiss();
    void thin();
    void thicken();
    void skeletonize();
    void skeletonReconstruct();
    void distanceTransform();
    void changeSETable();
    void setMorphologyMask();
    void geoDilation();
    void geoErosion();
    void watershed();
    void reverse();
};

#endif // MAINWINDOW_H
