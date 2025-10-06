#pragma once

#include <QtWidgets/QMainWindow>

#include "Library.h"
#include "Catalog.h"
#include "ParametersList.h"
#include "ParameterEditor.h"
#include "ThumbSelectDialog.h"
#include "UgoTabs.h"

#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
# include <QHBoxLayout>

class ComponentEditor : public QMainWindow
{
    Q_OBJECT

public:
    ComponentEditor(QList<Library>* libraries, QList<Catalog>* catalogs, QList<Parameters>*  parameters, QWidget* parent = nullptr);
    ~ComponentEditor();
    ParametersList* parametersListWidget;
    QList<QString> iconPaths;
    QList<QString> symbolPaths;

    ParametersList* currentParameterListWidget;
    QList<QString> iconsPaths;
    void updateParameterEditor(QString searchName);

    QLabel* iconDisplay;
    QList<QString> ugoDirsSymbolsPaths;
    QComboBox* modelsComboBox;

private:
    void setupUi();
    void setupConnections();

    QPushButton* selectIconBtn;

    ParameterEditor* parameterEditor;
    UgoTabs* ugoTabs;
 
    QList<Library>* librariesList;
    QList<Catalog>* catalogsList;
    QList<Parameters>* parametersList;

    Parameters* currentParameter;
    QList<QString>* tabs;
public slots:
    void selectModel(const QString& text);

private slots:
    void onItemDoubleClicked(QListWidgetItem* item);
    void onParameterChanged();
    void updateParameterLink();
};