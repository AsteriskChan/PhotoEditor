#ifndef QSSLOADER_H
#define QSSLOADER_H

#include <QFile>
#include <QApplication>

class QSSLoader
{
public:
    QSSLoader();
    static void setStyle(const QString &style);
};

#endif // QSSLOADER_H
