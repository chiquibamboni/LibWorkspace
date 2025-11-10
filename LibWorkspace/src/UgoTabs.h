#pragma once

#include <QTabWidget>
#include <QWidget>
#include <QLabel>
#include <QMap>
#include <QVBoxLayout>
#include <QPixmap>
#include "SvgViewer.h"

class UgoTabs : public QTabWidget
{
    Q_OBJECT

public:
    UgoTabs(QWidget* parent = nullptr);
    ~UgoTabs();

    void setTab(QString tabName);
    void setTabImage(QString tabName, QString svgFilePath);
    void clearTabImage(const QString& tabName);

private:
    QSize initialSize = QSize(200,200);
    QMap<QString, SvgViewer*> tabWidgets;
};

