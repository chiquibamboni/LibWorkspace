#include "UgoTabs.h"
#include <QDebug>

UgoTabs::UgoTabs(QWidget* parent) :QTabWidget(parent)
{

}

UgoTabs::~UgoTabs()
{

}

void UgoTabs::setTab(QString tabName)
{
    QWidget* tab = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout(tab);
    QLabel* label = new QLabel;
    layout->addWidget(label);
    addTab(tab, tabName);

    tabLabels[tabName] = label;
}

void UgoTabs::setTabImage(const QString& tabName, const QIcon& icon)
{
	if (tabLabels.contains(tabName)) {
		QLabel* iconDisplay = tabLabels[tabName];
        QPixmap mainPixmap = icon.pixmap(iconDisplay->size());
        iconDisplay->setPixmap(mainPixmap);
	}
}

void UgoTabs::clearTabImage(const QString& tabName)
{
    if (tabLabels.contains(tabName)) {
        QLabel* label = tabLabels[tabName];
        label->clear();
    }
}