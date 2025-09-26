#pragma once

#include <QtWidgets/QMainWindow>

#include "ParametersList.h"
#include "ParameterEditor.h"
#include "ThumbSelectDialog.h"

#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>

class ComponentEditor : public QMainWindow
{
    Q_OBJECT

public:
    ComponentEditor(QWidget* parent = nullptr);
    ~ComponentEditor();
    ParametersList* parametersList;
    QString iconsPath;

private:
    void setupUi();
    void setupConnections();

    QPushButton* selectIconBtn;
    QLabel* iconDisplay;
    QLabel* currentIconDisplay;

    ParameterEditor* parameterEditor;
 
    QList<Parameters>* parameters;

public slots:
    void onItemDoubleClicked(QListWidgetItem* item);
};