#include "UgoTabs.h"

UgoTabs::UgoTabs(QWidget* parent) :QTabWidget(parent)
{

}

UgoTabs::~UgoTabs()
{

}

void UgoTabs::setTab(QString ugoPath, QString tabName)
{
	QWidget* tab = new QWidget;
	QVBoxLayout* layout = new QVBoxLayout(tab);
	QLabel* label = new QLabel;
	QPixmap pixmap(ugoPath);
	label->setPixmap(pixmap.scaled(400, 300, Qt::KeepAspectRatio, Qt::SmoothTransformation));
	layout->addWidget(label);
	addTab(tab, tabName);
}