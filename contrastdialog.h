#ifndef CONTRASTDIALOG_H
#define CONTRASTDIALOG_H

#include <QDialog>
#include <QImage>
#include <QPainter>
#include <QButtonGroup>

namespace Ui {
class ContrastDialog;
}

class ContrastDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ContrastDialog(QWidget *parent = 0);
    ~ContrastDialog();

signals:
    void linearTransform(int fa, int fb, int ga, int gb);
    void piecewiseLinearTransform(int fa, int fb, int ga, int gb, int gmin, int gmax);
    void logarithmTransform(int a, int b, int c);
    void expotentTransform(float a, float b, float c);
    void histogramEqualization();


private:
    Ui::ContrastDialog *ui;
    QImage *histogram;
    QImage *funcgram;
    QImage *img;
    QButtonGroup *contrastButtonGroup;

private slots:
    void emitContrastTransform();
    void emitHistogramEqualization();
    void changeExpbSlider();
    void changeExpbSpinBox();

public slots:
    void drawHistogram(QImage *img);

private:
    void drawLinearFunction();
    void drawPiecewiseLinearFunction();

};

#endif // CONTRASTDIALOG_H
