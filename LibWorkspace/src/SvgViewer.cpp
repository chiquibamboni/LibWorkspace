#include "SvgViewer.h"
#include <QSvgWidget>
#include <QSvgRenderer>
#include <QResizeEvent>
#include <QVBoxLayout>

SvgViewer::SvgViewer(QSize& initialSize, QWidget* parent) : containerSize(initialSize), QWidget(parent)
{
    svgWidget = new QSvgWidget(this);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(svgWidget);
    setLayout(layout);

    setMinimumSize(containerSize);

    svgWidget->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    //svgWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void SvgViewer::loadSvg(const QString& svgPath)
{
    svgWidget->load(svgPath);
    fitSvgToContainer();
    update();
}

void SvgViewer::fitSvgToContainer()
{
    QSvgRenderer* renderer = svgWidget->renderer();
    if (!renderer || !renderer->isValid()) {
        return;
    }

    QSize svgSize = renderer->defaultSize();
    if (svgSize.isEmpty()) {
        return;
    }

    //if (svgSize.width() > svgSize.height())
    //{
    //    resize(containerSize.width(), containerSize.height() * 0.5);
    //}
    //else if (svgSize.width() < svgSize.height())
    //{
    //    resize(containerSize.width() * 0.75, containerSize.height());
    //}
    double widthRatio = (double)containerSize.width() / svgSize.width();
    double heightRatio = (double)containerSize.height() / svgSize.height();

    double scaleFactor = qMin(widthRatio, heightRatio);

    QSize newSize = svgSize * scaleFactor;

    svgWidget->setFixedSize(newSize);

    QVBoxLayout* layout = qobject_cast<QVBoxLayout*>(this->layout());
    if (layout) {
        layout->setAlignment(Qt::AlignCenter);
    }
}

void SvgViewer::setContainerSize(const QSize& newSize)
{
    containerSize = newSize;
    setFixedSize(containerSize);
    fitSvgToContainer();
}

void SvgViewer::clear()
{
    // Очищаем SVG виджет
    svgWidget->load(QString()); // Загружаем пустую строку для очистки

    // Сбрасываем размер к минимальному
    svgWidget->setFixedSize(containerSize);

    // Обновляем отображение
    update();
}