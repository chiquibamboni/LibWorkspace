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
    ComponentEditor(QList<Parameters>*  parameters, QWidget* parent = nullptr);
    ~ComponentEditor();
    ParametersList* parametersListWidget;
    QString iconsPath;

private:
    void setupUi();
    void setupConnections();

    QPushButton* selectIconBtn;
    QLabel* iconDisplay;
    QLabel* currentIconDisplay;
    QComboBox* modelComboBox;
    QStringList* modelsList;

    ParameterEditor* parameterEditor;
 
    QList<Parameters>* parametersList;

    Parameters* currentParameter;

private slots:
    void onItemDoubleClicked(QListWidgetItem* item);
    void onParameterChanged(); // Слот для обработки изменений
    void updateParameterLink();
};