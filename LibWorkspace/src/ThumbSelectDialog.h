#pragma once

#include <QDialog>
#include <QListWidget>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QPushButton>
#include <QIcon>
#include <QDir>

class ThumbSelectDialog : public QDialog {
    Q_OBJECT
public:
    ThumbSelectDialog(const QString& iconsPath = QString(), QWidget* parent = nullptr);
    ~ThumbSelectDialog();

    QIcon selectedIcon() const;

private slots:
    void loadIcons(const QString& path);

private:
    QListWidget* listWidget;
    QSize iconSize = QSize(64, 64);
    QSize gridSize = QSize(100, 100);
    int space = 10;
    QSize dialogSize = QSize(350, 350);
};