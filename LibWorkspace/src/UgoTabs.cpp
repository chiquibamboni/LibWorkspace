#include "UgoTabs.h"

UgoTabs::UgoTabs(QWidget* parent) :QTabWidget(parent)
{

}

UgoTabs::~UgoTabs()
{

}

void UgoTabs::setTab(QString tabName)
{
    if (tabWidgets.contains(tabName))
        return;

    QWidget* tab = new QWidget;
    tab->setFixedSize(initialSize);
    QVBoxLayout* layout = new QVBoxLayout(tab);

    SvgViewer* svgViewer = new SvgViewer(initialSize, tab);
    layout->addWidget(svgViewer, 0, Qt::AlignCenter);
    layout->setContentsMargins(0, 0, 0, 0);
    tab->setLayout(layout);

    addTab(tab, tabName);
    tabWidgets[tabName] = svgViewer;
}

void UgoTabs::setTabImage(QString tabName, QString svgFilePath)
{
    if (!tabWidgets.contains(tabName))
        return;

    SvgViewer* viewer = tabWidgets[tabName];
    viewer->loadSvg(svgFilePath);
}


void UgoTabs::clearTabImage(const QString& tabName)
{
    if (!tabWidgets.contains(tabName))
        return;

    SvgViewer* viewer = tabWidgets[tabName];
    //viewer->clear();
}