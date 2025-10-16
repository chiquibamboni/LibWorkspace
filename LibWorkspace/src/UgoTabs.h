#pragma once

#include <QTabWidget>
#include <QWidget>
#include <QLabel>
#include <QMap>
#include <QVBoxLayout>
#include <QPixmap>

class UgoTabs : public QTabWidget
{
    Q_OBJECT

public:
    UgoTabs(QWidget* parent = nullptr);
    ~UgoTabs();

    void setTab(QString tabName);
    void setTabImage(const QString& tabName, const QIcon& icon);
    void clearTabImage(const QString& tabName);

private:
    QHash<QString, int> tabIndices;
    QMap<QString, QLabel*> tabLabels;
};

