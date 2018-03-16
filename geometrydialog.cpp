#include "geometrydialog.h"
#include "ui_geometrydialog.h"
#include <QSpinBox>
#include <QSlider>

GeometryDialog::GeometryDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GeometryDialog)
{
    ui->setupUi(this);
    imageLabel = new QLabel;
    img=new QImage;
    imageProcess = NULL;
    imageLabel->setAlignment(Qt::AlignTop);
    imageLabel->setAlignment(Qt::AlignLeft);
    ui->scrollArea->setWidget(imageLabel);

    rotateButtonGroup = new QButtonGroup;
    resizeButtonGroup = new QButtonGroup;
    rotateButtonGroup->addButton(ui->nearestRotateRadioButton,1);
    rotateButtonGroup->addButton(ui->linearRotateRadioButton,2);
    resizeButtonGroup->addButton(ui->nearestResizeRadioButton,1);
    resizeButtonGroup->addButton(ui->linearResizeRadioButton,2);
    ui->linearRotateRadioButton->setChecked(true);
    ui->linearResizeRadioButton->setChecked(true);

    void (QSpinBox::*spinBoxSignal)(int) = &QSpinBox::valueChanged;
    connect(ui->rotateSpinBox, spinBoxSignal, ui->rotateSlider, &QSlider::setValue);
    connect(ui->rotateSlider, &QSlider::valueChanged, ui->rotateSpinBox, &QSpinBox::setValue);
    connect(ui->resizeSpinBox, spinBoxSignal, ui->resizeSlider, &QSlider::setValue);
    connect(ui->resizeSlider, &QSlider::valueChanged, ui->resizeSpinBox, &QSpinBox::setValue);
    connect(ui->rotateSlider,&QSlider::valueChanged,this,&GeometryDialog::resizeAndRotate);
    connect(ui->resizeSlider,&QSlider::valueChanged,this,&GeometryDialog::resizeAndRotate);
    connect(rotateButtonGroup,QOverload<int, bool>::of(&QButtonGroup::buttonToggled),this,&resizeAndRotate);
    connect(resizeButtonGroup,QOverload<int, bool>::of(&QButtonGroup::buttonToggled),this,&resizeAndRotate);
}

GeometryDialog::~GeometryDialog()
{
    delete ui;
    /*
    delete imageLabel;
    delete img;
    delete imageProcess;
    delete rotateButtonGroup;
    delete resizeButtonGroup;
    */
}

void GeometryDialog::setImageProcess(ImageProcess *imageProcess)
{
    this->imageProcess = imageProcess;
    *img = imageProcess->getQImage();
    showImage();
}

void GeometryDialog::showImage()
{
    imageLabel->setPixmap(QPixmap::fromImage(*img));
}

void GeometryDialog::resize()
{
    int id = resizeButtonGroup->checkedId();
    int value = ui->resizeSlider->value();
    double scale = (double)value / 100.0;
    imageProcess->resize(scale,scale,id);
    showImage();
}

void GeometryDialog::rotate()
{
    int id = rotateButtonGroup->checkedId();
    int value = ui->rotateSlider->value();
    imageProcess->rotate((float)value,id);
    showImage();
}

void GeometryDialog::resizeAndRotate()
{
    int id1 = resizeButtonGroup->checkedId();
    int id2 = rotateButtonGroup->checkedId();
    int value1 = ui->resizeSlider->value();
    double scale = (double)value1 / 100.0;
    int value2 = ui->rotateSlider->value();
    float angle = (float)value2;
    int mode;
    if (id1==1 && id2==1)
        mode = 1;
    else if (id1==1 && id2==2)
        mode = 2;
    else if (id1==2 && id2==1)
        mode = 3;
    else if (id1==2 && id2==2)
        mode = 4;
    *img = imageProcess->resizeAndRotate(scale,scale,angle,mode);
    showImage();
}


