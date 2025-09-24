#pragma once

#include <QtWidgets/QMainWindow>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QGroupBox>
#include <QFormLayout>
#include <QCheckBox>

class ComponentEditor : public QMainWindow
{
    Q_OBJECT

public:
    ComponentEditor(QWidget* parent = nullptr);
    ~ComponentEditor();

private:
    void setupUi();
    static const QStringList TYPE_ITEMS;
    static const QStringList FEATURE_ITEMS;
    static const QStringList UNIT_ITEMS;
};