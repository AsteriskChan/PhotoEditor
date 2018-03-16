#ifndef GEOMETRYDIALOG_H
#define GEOMETRYDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QImage>
#include "imageprocess.h"
#include <QButtonGroup>

namespace Ui {
class GeometryDialog;
}

class GeometryDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GeometryDialog(QWidget *parent = 0);
    ~GeometryDialog();
    void setImageProcess(ImageProcess *imageProcess);

private:
    Ui::GeometryDialog *ui;
    QLabel *imageLabel;
    QImage *img;
    ImageProcess *imageProcess;
    QButtonGroup *rotateButtonGroup;
    QButtonGroup *resizeButtonGroup;

    void showImage();

private slots:
    void resize();
    void rotate();
    void resizeAndRotate();

};

#endif // GEOMETRYDIALOG_H
