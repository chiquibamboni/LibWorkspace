#pragma once

#include <QTabWidget>
#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QPixmap>

class UgoTabs : public QTabWidget
{
    Q_OBJECT

public:
    UgoTabs(QWidget* parent = nullptr);
    ~UgoTabs();

    void setTab(QString ugoPath, QString tabName);
private:

};

