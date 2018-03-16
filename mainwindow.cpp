#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QScrollBar>
#include <QFileDialog>
#include <QString>
#include <QMessageBox>
#include <QScreen>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{



    imageLabel = new QLabel;
    statuLabel = new QLabel;
    img=new QImage;
    imageProcess = NULL;
    channelButtonGroup = new QButtonGroup;
    contrastDialog = new ContrastDialog;
    imageLabel->setAlignment(Qt::AlignTop);
    imageLabel->setAlignment(Qt::AlignLeft);
    imageLabel->setMouseTracking(true);

    ui->setupUi(this);
    ui->statusBar->addWidget(statuLabel);

    /*set channel button group
    channelButtonGroup->addButton(ui->redRadio,0);
    channelButtonGroup->addButton(ui->greenRadio,1);
    channelButtonGroup->addButton(ui->blueRadio,2);
    channelButtonGroup->addButton(ui->originRadio,3);
    ui->originRadio->setVisible(false);
    */

    connect(ui->openAction, &QAction::triggered, this, &MainWindow::open);
    connect(ui->undoAction, &QAction::triggered, this, &MainWindow::undo);
    connect(ui->saveAsAction,&QAction::triggered,this,&MainWindow::saveAs);
    connect(ui->initPushButton,&QPushButton::clicked,this,&MainWindow::initialize);
    //connect(channelButtonGroup,QOverload<int, bool>::of(&QButtonGroup::buttonToggled),this,&MainWindow::selectChannel);

    //channel
    connect(ui->redPushButton,&QPushButton::clicked,this,&MainWindow::redImage);
    connect(ui->greenPushButton,&QPushButton::clicked,this,&MainWindow::greenImage);
    connect(ui->bluePushButton,&QPushButton::clicked,this,&MainWindow::blueImage);

    //binary
    connect(ui->otsuPushButton,&QPushButton::clicked, this ,&MainWindow::otsuBinary);
    connect(ui->grayPushButton,&QPushButton::clicked,this,&MainWindow::grayImage);

    //2 threshold slider
    void (QSpinBox::*spinBoxSignal)(int) = &QSpinBox::valueChanged;
    connect(ui->thresholdSpinBox1, spinBoxSignal, ui->thresholdSlider1, &QSlider::setValue);
    connect(ui->thresholdSlider1, &QSlider::valueChanged, ui->thresholdSpinBox1, &QSpinBox::setValue);
    connect(ui->thresholdSpinBox2,  spinBoxSignal, ui->thresholdSlider2, &QSlider::setValue);
    connect(ui->thresholdSlider2, &QSlider::valueChanged, ui->thresholdSpinBox2, &QSpinBox::setValue);
    connect(ui->thresholdSlider1,&QSlider::valueChanged,this,&MainWindow::twoThresholdBinary);
    connect(ui->thresholdSlider2,&QSlider::valueChanged,this,&MainWindow::twoThresholdBinary);
    connect(ui->binaryPushButton,&QPushButton::clicked,this,&MainWindow::save);
    connect(ui->reversePushButton,&QPushButton::clicked,this,&MainWindow::reverse);

    connect(ui->fitWindowButton,&QPushButton::clicked,this,&MainWindow::fitWindow);

    connect(ui->geometryButton,&QPushButton::clicked,this,&MainWindow::openGeometryDialog);

    connect(ui->hueSlider,&QSlider::valueChanged,this,&MainWindow::adjustHSL);
    connect(ui->saturationSlider,&QSlider::valueChanged,this,&MainWindow::adjustHSL);
    connect(ui->lightnessSlider,&QSlider::valueChanged,this,&MainWindow::adjustHSL);
    connect(ui->hslPushButton, &QPushButton::clicked,this,&MainWindow::save);
    contrastDialog = new ContrastDialog(this);
    //contrastDialog->setAttribute(Qt::WA_DeleteOnClose);
    connect(ui->contrastButton,&QPushButton::clicked,this,&MainWindow::openContrastDialog);

    connect(contrastDialog,&ContrastDialog::linearTransform,this,&MainWindow::linearTransform);
    connect(this,&MainWindow::drawHistogram,contrastDialog,&ContrastDialog::drawHistogram);
    connect(contrastDialog,&ContrastDialog::piecewiseLinearTransform,this,&MainWindow::piecewiseLinearTransform);
    connect(contrastDialog,&ContrastDialog::logarithmTransform,this,&MainWindow::logarithmTransform);
    connect(contrastDialog,&ContrastDialog::expotentTransform,this,&MainWindow::expotentTransform);
    connect(contrastDialog,&ContrastDialog::histogramEqualization,this,&MainWindow::histogramEqualization);
    connect(contrastDialog,&ContrastDialog::accepted,this,&MainWindow::save);

    connect(ui->meanFilterPushButton,&QPushButton::clicked,this,&MainWindow::meanFilter);
    connect(ui->medianFilterPushButton,&QPushButton::clicked,this,&MainWindow::medianFilter);
    connect(ui->gaussianFilterPushButton,&QPushButton::clicked,this,&MainWindow::gaussianFilter);

    connect(ui->sobelPushButton,&QPushButton::clicked,this,&MainWindow::sobel);
    connect(ui->laplacianPushButton,&QPushButton::clicked,this,&MainWindow::laplacian);
    connect(ui->cannyPushButton,&QPushButton::clicked,this,&MainWindow::canny);

    connect(ui->houghLinePushButton,&QPushButton::clicked,this,&MainWindow::houghLine);
    connect(ui->houghCirclePushButton,&QPushButton::clicked,this,&MainWindow::houghCircle);

    connect(ui->biPagePushButton,&QPushButton::clicked,this,&MainWindow::pageTurning);
    connect(ui->morPagePushButton,&QPushButton::clicked,this,&MainWindow::pageTurning);
    connect(ui->filterPagePushButton,&QPushButton::clicked,this,&MainWindow::pageTurning);
    connect(ui->colorPagePushButton,&QPushButton::clicked,this,&MainWindow::pageTurning);

    connect(ui->addPushButton,&QPushButton::clicked,this,&MainWindow::add);
    connect(ui->addWeightedPushButton,&QPushButton::clicked,this,&MainWindow::addWeighted);
    connect(ui->subPushButton,&QPushButton::clicked,this,&MainWindow::subtract);
    connect(ui->mulPushButton,&QPushButton::clicked,this,&MainWindow::multiply);
    connect(ui->cutPushButton,&QPushButton::clicked,this,&MainWindow::cut);

    connect(ui->seWidthSpinBox,spinBoxSignal,this,&MainWindow::changeSETable);
    connect(ui->seHeightSpinBox,spinBoxSignal,this,&MainWindow::changeSETable);
    changeSETable();  //initialize

    connect(ui->erosionPushButton,&QPushButton::clicked,this,&MainWindow::erosion);
    connect(ui->dilationPushButton,&QPushButton::clicked,this,&MainWindow::dilation);
    connect(ui->openingPushButton,&QPushButton::clicked,this,&MainWindow::opening);
    connect(ui->closingPushButton,&QPushButton::clicked,this,&MainWindow::closing);
    connect(ui->hitOrMissPushButton,&QPushButton::clicked,this,&MainWindow::hitOrMiss);
    connect(ui->thinPushButton,&QPushButton::clicked,this,&MainWindow::thin);
    connect(ui->thickenPushButton,&QPushButton::clicked,this,&MainWindow::thicken);
    connect(ui->skeletonPushButton,&QPushButton::clicked,this,&MainWindow::skeletonize);
    connect(ui->skeletonRePushButton,&QPushButton::clicked,this,&MainWindow::skeletonReconstruct);
    connect(ui->disTransPushButton,&QPushButton::clicked,this,&MainWindow::distanceTransform);
    connect(ui->setMaskPushButton,&QPushButton::clicked,this,&MainWindow::setMorphologyMask);
    connect(ui->geoDilationPushButton,&QPushButton::clicked,this,&MainWindow::geoDilation);
    connect(ui->geoErosionPushButton,&QPushButton::clicked,this,&MainWindow::geoErosion);
    //connect(ui->waterShedPushButton,&QPushButton::clicked,this,&MainWindow::watershed);
}

MainWindow::~MainWindow()
{
    delete ui;
    /*
    delete imageLabel;
    delete statuLabel;
    delete img;
    delete channelButtonGroup;
    delete imageProcess;
    delete geometryDialog;
    delete contrastDialog;
    */
}


void MainWindow::open()
{
    QString filename;
    filename=QFileDialog::getOpenFileName(this,
                                          tr("Choose Image"),
                                          "",
                                          tr("Images (*.png *.bmp *.jpg *.tif *.jpeg)"));
    if(filename.isEmpty())
    {
         return;
    }
    imageProcess= new ImageProcess(filename);
    *img = imageProcess->getQImage();
    ui->scrollArea->setWidget(imageLabel);
    showImage(img);

}

void MainWindow::saveAs()
{
    QString filename = QFileDialog::getSaveFileName(this,
        tr("Save Image"),
        "",
        tr("*.bmp;; *.png;; *.jpg;; *.tif;; *.GIF")); //选择路径
    if(filename.isEmpty())
    {
        return;
    }
    else
    {
        if(! ( img->save(filename) ) ) //保存图像
        {
            QMessageBox::information(this,
                tr("Failed to save the image"),
                tr("Failed to save the image!"));
            return;
        }
    }
}

/* ---------------------------------------- private ---------------------------------------*/
void MainWindow::showImage(QImage *img){

    if (img->format()==QImage::Format_Indexed8)    //gray image or binary image
    {
        ui->redPushButton->setDisabled(true);
        ui->greenPushButton->setDisabled(true);
        ui->bluePushButton->setDisabled(true);
        ui->hslPushButton->setDisabled(true);

        ui->meanFilterPushButton->setDisabled(false);
        ui->medianFilterPushButton->setDisabled(false);
        ui->gaussianFilterPushButton->setDisabled(false);
        ui->laplacianPushButton->setDisabled(false);
        ui->cannyPushButton->setDisabled(false);
        ui->sobelPushButton->setDisabled(false);

        ui->erosionPushButton->setDisabled(false);
        ui->dilationPushButton->setDisabled(false);
        ui->openingPushButton->setDisabled(false);
        ui->closingPushButton->setDisabled(false);
        ui->setMaskPushButton->setDisabled(false);
        ui->binaryPushButton->setDisabled(false);
        ui->thresholdSlider1->setDisabled(false);
        ui->thresholdSlider2->setDisabled(false);
        ui->contrastButton->setDisabled(false);
        if(imageProcess->isSetMask())
        {
            ui->geoDilationPushButton->setDisabled(false);
            ui->geoErosionPushButton->setDisabled(false);
        }
        else
        {
            ui->geoDilationPushButton->setDisabled(true);
            ui->geoErosionPushButton->setDisabled(true);
        }
        //ui->setMaskPushButton->setDisabled(false);

        if (isBinaryImage(*img))    //binary
        {
            ui->otsuPushButton->setDisabled(true);
            ui->reversePushButton->setDisabled(false);

            ui->houghCirclePushButton->setDisabled(false);
            ui->houghLinePushButton->setDisabled(false);
            ui->thickenPushButton->setDisabled(false);
            ui->thinPushButton->setDisabled(false);
            ui->disTransPushButton->setDisabled(false);
            ui->hitOrMissPushButton->setDisabled(false);
            if(imageProcess->isSetSkeleton())
            {
                ui->skeletonPushButton->setDisabled(true);
                ui->skeletonRePushButton->setDisabled(false);
            }
            else
            {
                ui->skeletonPushButton->setDisabled(false);
                ui->skeletonRePushButton->setDisabled(true);
            }

        }
        else    //gray
        {

            ui->otsuPushButton->setDisabled(false);
            ui->reversePushButton->setDisabled(true);

            ui->houghCirclePushButton->setDisabled(true);
            ui->houghLinePushButton->setDisabled(true);
            ui->skeletonPushButton->setDisabled(true);
            ui->skeletonRePushButton->setDisabled(true);
            ui->thickenPushButton->setDisabled(true);
            ui->thinPushButton->setDisabled(true);
            ui->disTransPushButton->setDisabled(true);
            ui->hitOrMissPushButton->setDisabled(true);
        }

    }
    else     //color image
    {
        ui->redPushButton->setDisabled(false);
        ui->greenPushButton->setDisabled(false);
        ui->bluePushButton->setDisabled(false);
        ui->hslPushButton->setDisabled(false);

        ui->contrastButton->setDisabled(true);
        ui->reversePushButton->setDisabled(true);
        ui->binaryPushButton->setDisabled(true);
        ui->otsuPushButton->setDisabled(true);
        ui->reversePushButton->setDisabled(true);
        ui->thresholdSlider1->setDisabled(true);
        ui->thresholdSlider2->setDisabled(true);
        ui->houghCirclePushButton->setDisabled(true);
        ui->houghLinePushButton->setDisabled(true);
        ui->meanFilterPushButton->setDisabled(true);
        ui->medianFilterPushButton->setDisabled(true);
        ui->gaussianFilterPushButton->setDisabled(true);
        ui->laplacianPushButton->setDisabled(true);
        ui->cannyPushButton->setDisabled(true);
        ui->sobelPushButton->setDisabled(true);
        ui->erosionPushButton->setDisabled(true);
        ui->dilationPushButton->setDisabled(true);
        ui->openingPushButton->setDisabled(true);
        ui->closingPushButton->setDisabled(true);
        ui->setMaskPushButton->setDisabled(true);
        ui->geoDilationPushButton->setDisabled(true);
        ui->geoErosionPushButton->setDisabled(true);
        ui->skeletonPushButton->setDisabled(true);
        ui->skeletonRePushButton->setDisabled(true);
        ui->thickenPushButton->setDisabled(true);
        ui->thinPushButton->setDisabled(true);
        ui->disTransPushButton->setDisabled(true);
        ui->hitOrMissPushButton->setDisabled(true);
    }

    imageLabel->setPixmap(QPixmap::fromImage(*img));
    int width = img->width();
    int height = img->height();
    ui->x1SpinBox->setMaximum(width-1);
    ui->x2SpinBox->setMaximum(width-1);
    ui->y1SpinBox->setMaximum(height-1);
    ui->y2SpinBox->setMaximum(height-1);
    ui->x2SpinBox->setValue(width-1);
    ui->y2SpinBox->setValue(height-1);
}

int* MainWindow::getSE()
{
    int width = ui->seTableWidget->columnCount();
    int height = ui->seTableWidget->rowCount();
    int *se = new int[width*height];
    for(int i = 0 ; i < height; i++)
    {
        for (int j = 0 ; j < width; j++)
        {
            int value = ui->seTableWidget->item(i,j)->text().toInt();
            if (value == 1)
                value = 255;
            se[i*width+j]=value;
        }
    }
    return se;
}

bool MainWindow::isBinaryImage(QImage img)
{
    bool flag = true;
    for(int i = 0 ; i < img.width();i++)
    {
        for (int j = 0 ; j <img.height();j++)
        {
            QColor qc = img.pixelColor(i,j);
            if (qc.black()!= 0 && qc.black()!= 255)
            {
                flag = false;
                return flag;
            }
        }
    }
    return flag;
}

/* ----------------------------------------- slots -----------------------------------------*/
/*
 * get x,y and RGB
 */
void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    QPoint realPos = event->pos();
    QPoint imageLabelPos = realPos-(ui->centralWidget->pos()+ui->scrollArea->pos()); //由于图片显示在左上角，相当于scrollarea pos
    int scrollx = ui->scrollArea->horizontalScrollBar()->value();
    int scrolly = ui->scrollArea->verticalScrollBar()->value();
    int imagex = imageLabelPos.x()+scrollx;
    int imagey = imageLabelPos.y()+scrolly;

    //判断鼠标位于ImageLabel内
    if(imagex>=0 && imagey>=0
       && imagex < img->width()
       && imagey < img->height()
       && imageLabelPos.x()<ui->scrollArea->width()
       && imageLabelPos.y()<ui->scrollArea->height())
    {
        QColor qc = img->pixelColor(imagex,imagey);
        //gray image
        if(img->isGrayscale())
        {
            QString text("x:%1, y:%2, gray:%3");
            text = text.arg(imagex).arg(imagey).arg(255-qc.black());
            statuLabel->setText(text);
        }
        else if(img->format()== QImage::Format_Indexed8)
        {
            QString text("x:%1, y:%2, gray:%3");
            text = text.arg(imagex).arg(imagey).arg(255-qc.black());
            statuLabel->setText(text);
        }
        //color image
        else
        {
            QString text("x:%1, y:%2, R:%3, G:%4, B:%5");
            text = text.arg(imagex).arg(imagey).arg(qc.red()).arg(qc.green()).arg(qc.blue());
            statuLabel->setText(text);
        }

    }
    //QPoint point = evn->pos();
    //qDebug()<<event->pos().x()<<" "<<event->pos().y()<<endl;

}

void MainWindow::undo()
{
    if (imageProcess==NULL)
    {
        return;
    }
    *img = imageProcess->undo();
    showImage(img);
}

void MainWindow::save()
{
    imageProcess->save();
    *img = imageProcess->getQImage();
    showImage(img);
}

void MainWindow::initialize()
{
    if (imageProcess==NULL)
    {
        return;
    }
    *img = imageProcess->initialize();
    showImage(img);
}


void MainWindow::fitWindow()
{
    if (imageProcess==NULL)
    {
        return;
    }
    double iw = img->width();
    double ih = img->height();
    double sw = ui->scrollArea->width();
    double sh = ui->scrollArea->height();
    qDebug()<<"scroll area:"<<ui->scrollArea->width() << "  " <<ui->scrollArea->height()<<endl;
    qDebug()<<"image:"<<img->width()<<"   "<<img->height();
    double scalex;
    double scaley;
    if (iw<=sw && ih <= sh)
    {
        scalex = 1;
        scaley = 1;
    }
    else if ( iw/ih > sw/sh)
    {
        scalex = sw / iw;
        scaley = scalex;
    }
    else
    {
        scaley = sh / ih;
        scalex = scaley;
    }

    qDebug()<<"scalex:"<<scalex<<" scaley:"<<scaley<<endl;
    *img = imageProcess->resize(scalex,scaley,2);
    showImage(img);
}

void MainWindow::pageTurning()
{
    /*
    int i = ui->stackedWidget->currentIndex();
    int n = ui->stackedWidget->count();
    ui->stackedWidget->setCurrentIndex((i+1)%n);
    */
    QString buttonName = sender()->objectName();
    if (buttonName.compare("colorPagePushButton") == 0)
    {
        ui->stackedWidget->setCurrentIndex(0);
    }
    if (buttonName.compare("biPagePushButton") == 0)
    {
        ui->stackedWidget->setCurrentIndex(1);
    }
    if (buttonName.compare("filterPagePushButton") == 0)
    {
        ui->stackedWidget->setCurrentIndex(2);
    }
    if (buttonName.compare("morPagePushButton") == 0)
    {
        ui->stackedWidget->setCurrentIndex(3);
    }

}


void MainWindow::openGeometryDialog()
{
    if (imageProcess == NULL)
    {
        return;
    }
    geometryDialog = new GeometryDialog(this);
    geometryDialog->setImageProcess(imageProcess);
    if(geometryDialog->exec() == QDialog::Accepted)
    {
        save();
    }
}

void MainWindow::openContrastDialog()
{
    if (imageProcess == NULL)
    {
        return;
    }

    contrastDialog->open();

    emit drawHistogram(img); //emit signals to draw histogram
}

void MainWindow::adjustHSL()
{
    if (imageProcess == NULL)
    {
        return;
    }
    float scaleH = ui->hueSlider->value();
    float scaleS = ui->saturationSlider->value() / 100.0;
    float scaleI = ui->lightnessSlider->value() / 100.0;
    *img = imageProcess->adjustHSL(scaleH,scaleS,scaleI);
    showImage(img);
}

void MainWindow::linearTransform(int fa, int fb, int ga, int gb)
{
    if (imageProcess == NULL)
    {
        return;
    }
    *img = imageProcess->linearTransform(fa,fb,ga,gb);
    emit drawHistogram(img);
    showImage(img);
}

void MainWindow::piecewiseLinearTransform(int fa, int fb, int ga, int gb, int gmin, int gmax)
{
    if (imageProcess == NULL)
    {
        return;
    }
    *img = imageProcess->piecewiseLinearTransform(fa,fb,ga,gb,gmin,gmax);
    emit drawHistogram(img);
    showImage(img);
}

void MainWindow::logarithmTransform(int a, int b, int c)
{
    if (imageProcess == NULL)
    {
        return;
    }
    *img = imageProcess->logarithmTransform(a,b,c);
    emit drawHistogram(img);
    showImage(img);
}

void MainWindow::expotentTransform(float a, float b, float c)
{
    if (imageProcess == NULL)
    {
        return;
    }
    *img = imageProcess->expotentTransform(a,b,c);
    emit drawHistogram(img);
    showImage(img);
}

void MainWindow::histogramEqualization()
{
    if (imageProcess == NULL)
    {
        return;
    }
    *img = imageProcess->histogramEqualization();
    emit drawHistogram(img);
    showImage(img);
}

void MainWindow::meanFilter()
{
    if (imageProcess == NULL)
    {
        return;
    }
    int anchorx = ui->anchorXSpinBox->value();
    int anchory = ui->anchorYSpinBox->value();
    int kernelW = ui->kernelWidthSpinBox->value();
    int kernelH = ui->kernelHeightSpinBox->value();
    *img = imageProcess->meanFilter(anchorx, anchory, kernelW, kernelH);
    showImage(img);
}

void MainWindow::medianFilter()
{
    if (imageProcess == NULL)
    {
        return;
    }
    int anchorx = ui->anchorXSpinBox->value();
    int anchory = ui->anchorYSpinBox->value();
    int kernelW = ui->kernelWidthSpinBox->value();
    int kernelH = ui->kernelHeightSpinBox->value();
    *img = imageProcess->medianFilter(anchorx, anchory, kernelW, kernelH);
    showImage(img);
}

void MainWindow::gaussianFilter()
{
    if (imageProcess == NULL)
    {
        return;
    }
    int kernelW = ui->kernelWidthSpinBox->value();
    int kernelH = ui->kernelHeightSpinBox->value();
    float sigma = ui->sigmaSpinBox->value();
    *img = imageProcess->gaussianFilter(kernelH , sigma);
    showImage(img);
}

void MainWindow::sobel()
{
    if (imageProcess == NULL)
    {
        return;
    }
    *img = imageProcess->sobel();
    showImage(img);
}

void MainWindow::laplacian()
{
    if (imageProcess == NULL)
    {
        return;
    }
    *img = imageProcess->laplacian();
    showImage(img);
}

void MainWindow::canny()
{
    if (imageProcess == NULL)
    {
        return;
    }
    int t1 = ui->cannyThresholdSpinBox1->value();
    int t2 = ui->cannyThresholdSpinBox2->value();
    *img = imageProcess->canny(t1,t2);
    showImage(img);
}

void MainWindow::houghLine()
{
    if (imageProcess == NULL)
    {
        return;
    }
    double rho = ui->houghRhoSpinBox->value();
    double theta = ui->houghThetaSpinBox->value() * 3.14 / 180;
    int threshold = ui->houghLineTSpinBox->value();
    *img = imageProcess->houghLine(rho,theta,threshold);
    showImage(img);
}

void MainWindow::houghCircle()
{
    if (imageProcess == NULL)
    {
        return;
    }
    int rmin = ui->houghRminSpinBox->value();
    int rmax = ui->houghRmaxSpinBox->value();
    int threshold = ui->houghCircleTSpinBox->value();
    *img = imageProcess->houghCircle(rmin,rmax,threshold);
    showImage(img);
}

void MainWindow::add()
{
    if (imageProcess == NULL)
    {
        return;
    }
    QString filename2;
    filename2=QFileDialog::getOpenFileName(this,
                                          tr("Choose Image"),
                                          "",
                                          tr("Images (*.png *.bmp *.jpg *.tif *.jpeg)"));
    if(filename2.isEmpty())
    {
         return;
    }
    int x1 = ui->x1SpinBox->value();
    int y1 = ui->y1SpinBox->value();
    int x2 = ui->x2SpinBox->value();
    int y2 = ui->y2SpinBox->value();
    *img = imageProcess->add(filename2,x1,y1,x2,y2);
    showImage(img);

}

void MainWindow::addWeighted()
{
    if (imageProcess == NULL)
    {
        return;
    }
    QString filename2;
    filename2=QFileDialog::getOpenFileName(this,
                                          tr("Choose Image"),
                                          "",
                                          tr("Images (*.png *.bmp *.jpg *.tif *.jpeg)"));
    if(filename2.isEmpty())
    {
         return;
    }
    int x1 = ui->x1SpinBox->value();
    int y1 = ui->y1SpinBox->value();
    int x2 = ui->x2SpinBox->value();
    int y2 = ui->y2SpinBox->value();
    double alpha = ui->alphaSpinBox->value();
    *img = imageProcess->addWeighted(filename2,x1,y1,x2,y2,alpha);
    showImage(img);
}

void MainWindow::subtract()
{
    if (imageProcess == NULL)
    {
        return;
    }
    QString filename2;
    filename2=QFileDialog::getOpenFileName(this,
                                          tr("Choose Image"),
                                          "",
                                          tr("Images (*.png *.bmp *.jpg *.tif *.jpeg)"));
    if(filename2.isEmpty())
    {
         return;
    }
    int x1 = ui->x1SpinBox->value();
    int y1 = ui->y1SpinBox->value();
    int x2 = ui->x2SpinBox->value();
    int y2 = ui->y2SpinBox->value();
    *img = imageProcess->subtract(filename2,x1,y1,x2,y2);
    showImage(img);
}

void MainWindow::multiply()
{
    if (imageProcess == NULL)
    {
        return;
    }
    QString filename2;
    filename2=QFileDialog::getOpenFileName(this,
                                          tr("Choose Image"),
                                          "",
                                          tr("Images (*.png *.bmp *.jpg *.tif *.jpeg)"));
    if(filename2.isEmpty())
    {
         return;
    }
    int x1 = ui->x1SpinBox->value();
    int y1 = ui->y1SpinBox->value();
    int x2 = ui->x2SpinBox->value();
    int y2 = ui->y2SpinBox->value();
    *img = imageProcess->multiply(filename2,x1,y1,x2,y2);
    showImage(img);

}

void MainWindow::cut()
{
    if (imageProcess == NULL)
    {
        return;
    }
    int x1 = ui->x1SpinBox->value();
    int y1 = ui->y1SpinBox->value();
    int x2 = ui->x2SpinBox->value();
    int y2 = ui->y2SpinBox->value();
    *img = imageProcess->cut(x1,y1,x2,y2);
    showImage(img);
}

void MainWindow::dilation()
{
    if (imageProcess == NULL)
    {
        return;
    }
    int *se = getSE();
    int width = ui->seTableWidget->columnCount();
    int height = ui->seTableWidget->rowCount();
    int anchorx = ui->seAnchorXSpinBox->value();
    int anchory = ui->seAnchorYSpinBox->value();
    *img = imageProcess->dilation(se,width,height,anchorx,anchory);
    showImage(img);

}

void MainWindow::erosion()
{
    if (imageProcess == NULL)
    {
        return;
    }
    int *se = getSE();
    int width = ui->seTableWidget->columnCount();
    int height = ui->seTableWidget->rowCount();
    int anchorx = ui->seAnchorXSpinBox->value();
    int anchory = ui->seAnchorYSpinBox->value();
    *img = imageProcess->erosion(se,width,height,anchorx,anchory);
    showImage(img);
}

void MainWindow::opening()
{
    if (imageProcess == NULL)
    {
        return;
    }
    int *se = getSE();
    int width = ui->seTableWidget->columnCount();
    int height = ui->seTableWidget->rowCount();
    int anchorx = ui->seAnchorXSpinBox->value();
    int anchory = ui->seAnchorYSpinBox->value();
    *img = imageProcess->opening(se,width,height,anchorx,anchory);
    showImage(img);
}

void MainWindow::closing()
{
    if (imageProcess == NULL)
    {
        return;
    }
    int *se = getSE();
    int width = ui->seTableWidget->columnCount();
    int height = ui->seTableWidget->rowCount();
    int anchorx = ui->seAnchorXSpinBox->value();
    int anchory = ui->seAnchorYSpinBox->value();
    *img = imageProcess->closing(se,width,height,anchorx,anchory);
    showImage(img);
}

void MainWindow::hitOrMiss()
{
    if (imageProcess == NULL)
    {
        return;
    }
    int *se = getSE();
    int width = ui->seTableWidget->columnCount();
    int height = ui->seTableWidget->rowCount();
    int anchorx = ui->seAnchorXSpinBox->value();
    int anchory = ui->seAnchorYSpinBox->value();
    *img = imageProcess->hitOrMiss(se,width,height,anchorx,anchory);
    showImage(img);
}

void MainWindow::thin()
{
    if (imageProcess == NULL)
    {
        return;
    }
    int *se = getSE();
    int width = ui->seTableWidget->columnCount();
    int height = ui->seTableWidget->rowCount();
    int anchorx = ui->seAnchorXSpinBox->value();
    int anchory = ui->seAnchorYSpinBox->value();
    *img = imageProcess->thin(se,width,height,anchorx,anchory);
    showImage(img);
}

void MainWindow::thicken()
{
    if (imageProcess == NULL)
    {
        return;
    }
    int *se = getSE();
    int width = ui->seTableWidget->columnCount();
    int height = ui->seTableWidget->rowCount();
    int anchorx = ui->seAnchorXSpinBox->value();
    int anchory = ui->seAnchorYSpinBox->value();
    *img = imageProcess->thicken(se,width,height,anchorx,anchory);
    showImage(img);
}

void MainWindow::skeletonize()
{
    if (imageProcess == NULL)
    {
        return;
    }
    int *se = getSE();
    int width = ui->seTableWidget->columnCount();
    int height = ui->seTableWidget->rowCount();
    int anchorx = ui->seAnchorXSpinBox->value();
    int anchory = ui->seAnchorYSpinBox->value();
    int maxk = ui->skeletonMaxKSpinBox->value();
    *img = imageProcess->skeletonize(se,width,height,anchorx,anchory,maxk);
    showImage(img);
}

void MainWindow::skeletonReconstruct()
{
    if (imageProcess == NULL)
    {
        return;
    }
    int *se = getSE();
    int width = ui->seTableWidget->columnCount();
    int height = ui->seTableWidget->rowCount();
    int anchorx = ui->seAnchorXSpinBox->value();
    int anchory = ui->seAnchorYSpinBox->value();

    *img = imageProcess->skeletonReconstruct(se,width,height,anchorx,anchory);
    showImage(img);
}

void MainWindow::distanceTransform()
{
    if (imageProcess == NULL)
    {
        return;
    }
    int *se = getSE();
    int width = ui->seTableWidget->columnCount();
    int height = ui->seTableWidget->rowCount();
    int anchorx = ui->seAnchorXSpinBox->value();
    int anchory = ui->seAnchorYSpinBox->value();
    *img = imageProcess->distanceTransform(se,width,height,anchorx,anchory);
    showImage(img);
}

void MainWindow::changeSETable()
{
    int width = ui->seWidthSpinBox->value();
    int height = ui->seHeightSpinBox->value();
    ui->seTableWidget->setColumnCount(width);
    ui->seTableWidget->setRowCount(height);
    for(int i = 0 ; i < height; i++)
    {
        for (int j = 0 ; j < width; j++)
        {
            ui->seTableWidget->setItem(i,j,new QTableWidgetItem("1"));
        }
    }
    ui->seAnchorXSpinBox->setMaximum(width-1);
    ui->seAnchorYSpinBox->setMaximum(height-1);
}

void MainWindow::setMorphologyMask()
{
    if (imageProcess==NULL)
    {
        return;
    }
    imageProcess->setMorphologyMask();
}

void MainWindow::geoDilation()
{
    if (imageProcess == NULL)
    {
        return;
    }
    int *se = getSE();
    int width = ui->seTableWidget->columnCount();
    int height = ui->seTableWidget->rowCount();
    int anchorx = ui->seAnchorXSpinBox->value();
    int anchory = ui->seAnchorYSpinBox->value();
    *img = imageProcess->geoDilation(se,width,height,anchorx,anchory);
    showImage(img);
}

void MainWindow::geoErosion()
{
    if (imageProcess == NULL)
    {
        return;
    }
    int *se = getSE();
    int width = ui->seTableWidget->columnCount();
    int height = ui->seTableWidget->rowCount();
    int anchorx = ui->seAnchorXSpinBox->value();
    int anchory = ui->seAnchorYSpinBox->value();
    *img = imageProcess->geoErosion(se,width,height,anchorx,anchory);
    showImage(img);
}

void MainWindow::watershed()
{
    if (imageProcess == NULL)
    {
        return;
    }
    *img = imageProcess->watershed();
    showImage(img);
}

void MainWindow::reverse()
{
    if (imageProcess == NULL)
    {
        return;
    }
    *img = imageProcess->reverse();
    showImage(img);
}

void MainWindow::selectChannel(int id , bool checked)
{
    if (imageProcess==NULL)
    {
        return;
    }
    //int color = channelButtonGroup->checkedId();
    switch(id)
    {
    case 0:
        *img = imageProcess->getRedChannel();
        break;
    case 1:
        *img = imageProcess->getGreenChannel();
        break;
    case 2:
        *img = imageProcess->getBlueChannel();
        break;
    default:
        *img = imageProcess->getQImage();
        break;
    }
    showImage(img);

}

void MainWindow::otsuBinary()
{
    if (imageProcess==NULL)
    {
        return;
    }
    *img = imageProcess->otsuBinary();
    showImage(img);
}

void MainWindow::twoThresholdBinary()
{
    if (imageProcess==NULL)
    {
        return;
    }
    int t1 = ui->thresholdSlider1->value();
    int t2 = ui->thresholdSlider2->value();
    *img = imageProcess->twoThresholdBinary(t1,t2);
    showImage(img);
}


void MainWindow::grayImage()
{
    if (imageProcess==NULL)
    {
        return;
    }
    *img = imageProcess->getGrayImage();
    showImage(img);
}

void MainWindow::redImage()
{
    if (imageProcess==NULL)
    {
        return;
    }
    *img = imageProcess->getRedChannel();
    showImage(img);
}

void MainWindow::greenImage()
{
    if (imageProcess==NULL)
    {
        return;
    }
    *img = imageProcess->getGreenChannel();
    showImage(img);
}

void MainWindow::blueImage()
{
    if (imageProcess==NULL)
    {
        return;
    }
    *img = imageProcess->getBlueChannel();
    showImage(img);
}
