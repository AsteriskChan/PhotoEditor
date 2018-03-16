#include "contrastdialog.h"
#include "ui_contrastdialog.h"
#include <vector>

ContrastDialog::ContrastDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ContrastDialog)
{
    ui->setupUi(this);
    img = new QImage;
    histogram = new QImage;
    funcgram = new QImage;

    contrastButtonGroup = new QButtonGroup;
    contrastButtonGroup->addButton(ui->linearRadioButton);
    contrastButtonGroup->addButton(ui->piecewiseRadioButton);
    contrastButtonGroup->addButton(ui->logarithmRadioButton);
    ui->linearRadioButton->setChecked(true);
    connect(ui->faSlider,&QSlider::valueChanged,this,&ContrastDialog::emitContrastTransform);
    connect(ui->fbSlider,&QSlider::valueChanged,this,&ContrastDialog::emitContrastTransform);
    connect(ui->gaSlider,&QSlider::valueChanged,this,&ContrastDialog::emitContrastTransform);
    connect(ui->gbSlider,&QSlider::valueChanged,this,&ContrastDialog::emitContrastTransform);

    connect(ui->gminSlider,&QSlider::valueChanged,this,&ContrastDialog::emitContrastTransform);
    connect(ui->gmaxSlider,&QSlider::valueChanged,this,&ContrastDialog::emitContrastTransform);
    connect(ui->logaSlider,&QSlider::valueChanged,this,&ContrastDialog::emitContrastTransform);
    connect(ui->logbSlider,&QSlider::valueChanged,this,&ContrastDialog::emitContrastTransform);
    connect(ui->logcSlider,&QSlider::valueChanged,this,&ContrastDialog::emitContrastTransform);
    connect(ui->expaSlider,&QSlider::valueChanged,this,&ContrastDialog::emitContrastTransform);
    connect(ui->expbSlider,&QSlider::valueChanged,this,&ContrastDialog::emitContrastTransform);
    connect(ui->expcSlider,&QSlider::valueChanged,this,&ContrastDialog::emitContrastTransform);
    connect(contrastButtonGroup,QOverload<int, bool>::of(&QButtonGroup::buttonToggled),this,&ContrastDialog::emitContrastTransform);

    void (QSpinBox::*spinBoxSignal)(int) = &QSpinBox::valueChanged;
    void (QDoubleSpinBox::*doubleSpinBoxSignal)(double) = &QDoubleSpinBox::valueChanged;
    connect(ui->logaSpinBox, spinBoxSignal, ui->logaSlider, &QSlider::setValue);
    connect(ui->logaSlider, &QSlider::valueChanged, ui->logaSpinBox, &QSpinBox::setValue);
    connect(ui->logbSpinBox, spinBoxSignal, ui->logbSlider, &QSlider::setValue);
    connect(ui->logbSlider, &QSlider::valueChanged, ui->logbSpinBox, &QSpinBox::setValue);
    connect(ui->logcSpinBox, spinBoxSignal, ui->logcSlider, &QSlider::setValue);
    connect(ui->logcSlider, &QSlider::valueChanged, ui->logcSpinBox, &QSpinBox::setValue);
    connect(ui->expaSpinBox, spinBoxSignal, ui->expaSlider, &QSlider::setValue);
    connect(ui->expaSlider, &QSlider::valueChanged, ui->expaSpinBox, &QSpinBox::setValue);

    connect(ui->expbSpinBox, doubleSpinBoxSignal,this, &ContrastDialog::changeExpbSlider);
    connect(ui->expbSlider, &QSlider::valueChanged,this, &ContrastDialog::changeExpbSpinBox);

    connect(ui->expcSpinBox, spinBoxSignal, ui->expcSlider, &QSlider::setValue);
    connect(ui->expcSlider, &QSlider::valueChanged, ui->expcSpinBox, &QSpinBox::setValue);

    connect(ui->equalizationPushButton,&QPushButton::clicked,this,&ContrastDialog::emitHistogramEqualization);
}

ContrastDialog::~ContrastDialog()
{
    delete ui;
    /*
    delete img;
    delete histogram;
    delete funcgram;
    delete contrastButtonGroup;
    */
}


void ContrastDialog::emitContrastTransform()
{
    int fa = ui->faSlider->value();
    int fb = ui->fbSlider->value();
    int ga = ui->gaSlider->value();
    int gb = ui->gbSlider->value();
    int gmin = ui->gminSlider->value();
    int gmax = ui->gmaxSlider->value();
    int la = ui->logaSlider->value();
    int lb = ui->logbSlider->value();
    int lc = ui->logcSlider->value();
    int ea = ui->expaSlider->value();
    double eb = (double) ui->expbSlider->value() / 100.0;
    int ec = ui->expcSlider->value();
    if(ui->linearRadioButton->isChecked())
    {
        drawLinearFunction();
        emit linearTransform(fa,fb,ga,gb);
    }
    if(ui->piecewiseRadioButton->isChecked())
    {
        drawPiecewiseLinearFunction();
        emit piecewiseLinearTransform(fa,fb,ga,gb,gmin,gmax);
    }
    if(ui->logarithmRadioButton->isChecked())
    {
        emit logarithmTransform(la,lb,lc);
    }
    if(ui->expotentRadioButton->isChecked())
    {
        emit expotentTransform(ea,eb,ec);
    }
}

void ContrastDialog::emitHistogramEqualization()
{
    emit histogramEqualization();
}

void ContrastDialog::changeExpbSlider()
{
    double value = ui->expbSpinBox->value();
    int intValue = value * 100;
    ui->expbSlider->setValue(intValue);
}

void ContrastDialog::changeExpbSpinBox()
{
    int value = ui->expbSlider->value();
    double doubleValue = (double)value / 100.0;
    ui->expbSpinBox->setValue(doubleValue);
}

void ContrastDialog::drawHistogram(QImage *img)
{
    this->img = img;
    int height = ui->histogramLabel->height();
    int width = ui->histogramLabel->width();
    histogram = new QImage(width,height,QImage::Format_RGB888);
    QPainter p(histogram);
    p.setBrush(QBrush(QColor(121,121,121)));
    p.drawRect(0,0,width,height);

    p.setBrush(QBrush(QColor(255,255,255)));
    p.drawRect(0,0,width,height);

    //calculate histogram
    std::vector<int> histVec(256);
    for (int i=0;i<img->width();i++)
    {
        for (int j=0;j<img->height();j++)
        {
            QColor qc = img->pixelColor(i,j);
            int index = 255 - qc.black();
            histVec[index] ++;
        }
    }
    std::vector<int>::iterator biggest = std::max_element(std::begin(histVec), std::end(histVec));
    int maxCount = *biggest;


    p.translate(0,height);   //坐标原点设为左下角，y方向正方向仍为下
    p.drawLine(0,0,100,100);

    p.drawLine(10,-10,width-20,-10);// 横轴
    p.drawLine(10,-10,10,-height+10);//纵轴

    float xstep = float(width-30) / 256;
    float ystep = float(height-20) / maxCount;
    for (int i=0;i<256;i++)
    {
        //QColor color(i,255-i,0);
        p.setPen(Qt::black);
        p.setBrush(Qt::black);
        //p.setBrush(color);
        //p.setPen(color);
        //p.drawLine(QPointF(10+(i+0.5)*xstep,-10-ystep*count[i]),QPointF(10+(i+1.5)*xstep,-10-ystep*count[i+1]));
        p.drawRect(10+i*xstep,-10,xstep,-10-ystep*histVec[i]);


        if(i % 32 == 0||i==255)
        {
            p.drawText(QPointF(10+(i-0.5)*xstep,0),QString::number(i));
        }

    }
    ui->histogramLabel->setPixmap(QPixmap::fromImage(*histogram));
}

void ContrastDialog::drawLinearFunction()
{
    int fa = ui->faSlider->value();
    int fb = ui->fbSlider->value();
    int ga = ui->gaSlider->value();
    int gb = ui->gbSlider->value();
    int height = ui->functionLabel->height();
    int width = ui->functionLabel->width();
    funcgram = new QImage(width,height,QImage::Format_RGB888);
    QPainter p(funcgram);

    p.setBrush(QBrush(QColor(121,121,121)));
    p.drawRect(0,0,width,height);

    p.setBrush(QBrush(QColor(255,255,255)));
    p.drawRect(0,0,width,height);

    p.translate(0,height);   //坐标原点设为左下角，y方向正方向仍为下
    p.drawLine(0,0,100,100);

    p.drawLine(10,-10,width-20,-10);// 横轴
    p.drawLine(10,-10,10,-height+10);//纵轴

    float xstep = float(width-30) / 256;
    float ystep = float(height-20) / 256;
    p.drawLine(10,-10-ystep*ga,10+xstep*fa,-10-ystep*ga);
    p.drawLine(10+xstep*fa,-10-ystep*ga,10+xstep*fb,-10-ystep*gb);
    p.drawLine(10+xstep*fb,-10-ystep*gb,10+xstep*255,-10-ystep*gb);

    p.drawText(QPointF(0,0),QString::number(0));
    p.drawText(QPointF(10+xstep*fa,0),QString::number(fa));
    p.drawText(QPointF(10+xstep*fb,0),QString::number(fb));
    p.drawText(QPointF(0,-10-ystep*ga),QString::number(ga));
    p.drawText(QPointF(0,-10-ystep*gb),QString::number(gb));
    ui->functionLabel->setPixmap(QPixmap::fromImage(*funcgram));

}

void ContrastDialog::drawPiecewiseLinearFunction()
{
    int fa = ui->faSlider->value();
    int fb = ui->fbSlider->value();
    int ga = ui->gaSlider->value();
    int gb = ui->gbSlider->value();
    int gmin = ui->gminSlider->value();
    int gmax = ui->gmaxSlider->value();
    int height = ui->functionLabel->height();
    int width = ui->functionLabel->width();
    funcgram = new QImage(width,height,QImage::Format_RGB888);
    QPainter p(funcgram);

    p.setBrush(QBrush(QColor(121,121,121)));
    p.drawRect(0,0,width,height);

    p.setBrush(QBrush(QColor(255,255,255)));
    p.drawRect(0,0,width,height);

    p.translate(0,height);   //坐标原点设为左下角，y方向正方向仍为下
    p.drawLine(0,0,100,100);

    p.drawLine(10,-10,width-20,-10);// 横轴
    p.drawLine(10,-10,10,-height+10);//纵轴

    float xstep = float(width-30) / 256;
    float ystep = float(height-20) / 256;
    p.drawLine(10,-10-ystep*gmin,10+xstep*fa,-10-ystep*ga);
    p.drawLine(10+xstep*fa,-10-ystep*ga,10+xstep*fb,-10-ystep*gb);
    p.drawLine(10+xstep*fb,-10-ystep*gb,10+xstep*255,-10-ystep*gmax);

    p.drawText(QPointF(0,0),QString::number(0));
    p.drawText(QPointF(10+xstep*fa,0),QString::number(fa));
    p.drawText(QPointF(10+xstep*fb,0),QString::number(fb));
    p.drawText(QPointF(0,-10-ystep*gmin),QString::number(ga));
    p.drawText(QPointF(0,-10-ystep*ga),QString::number(ga));
    p.drawText(QPointF(0,-10-ystep*gb),QString::number(gb));
    p.drawText(QPointF(0,-10-ystep*gmax),QString::number(gb));
    ui->functionLabel->setPixmap(QPixmap::fromImage(*funcgram));
}



