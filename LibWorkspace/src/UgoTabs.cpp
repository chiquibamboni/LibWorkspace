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

void UgoTabs::setTabImage(const QString& tabName, const QString& UgoPath)
{
	if (tabLabels.contains(tabName)) {
		QLabel* label = tabLabels[tabName];
		QPixmap pixmap(UgoPath);
		label->setPixmap(pixmap.scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        label->setAlignment(Qt::AlignCenter);
        label->update();
	}
}