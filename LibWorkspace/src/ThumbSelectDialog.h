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
    ThumbSelectDialog(QWidget* parent = nullptr, const QString& iconsPath = QString());
    ~ThumbSelectDialog();

    QIcon selectedIcon() const;

private slots:
    void loadIcons(const QString& path);

private:
    QListWidget* listWidget;
};