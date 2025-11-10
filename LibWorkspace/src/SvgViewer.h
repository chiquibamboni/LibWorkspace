#pragma once

#include <QWidget>
#include <QSvgWidget>
#include <QSvgRenderer>
#include <QResizeEvent>
#include <QPainter>


class QSvgWidget;

class SvgViewer : public QWidget
{
    Q_OBJECT

public:
    SvgViewer(QSize& initialSize, QWidget* parent = nullptr);

    void loadSvg(const QString& svgPath);
    void setContainerSize(const QSize& newSize);
    void clear();

private:
    void fitSvgToContainer();
    //void resizeEvent(QResizeEvent* event) override;

    QSvgWidget* svgWidget;
    QSize containerSize;
    QString currentSvgPath;
};
