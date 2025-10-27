#pragma once

#include <QtWidgets/QMainWindow>

#include "Library.h"
#include "Catalog.h"
#include "ParametersList.h"
#include "ParameterEditor.h"
#include "ThumbSelectDialog.h"
#include "UgoTabs.h"
#include "ValueValidator.h"

#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
# include <QHBoxLayout>

class ComponentEditor : public QMainWindow
{
    Q_OBJECT

public:
    ComponentEditor(QList<Library>* libraries, QList<Catalog>* catalogs, QList<Component>* components, QList<Parameters>*  parameters, QWidget* parent = nullptr);
    ~ComponentEditor();
    ParametersList* parametersListWidget;
    QList<QString> iconPaths;
    QList<QString> symbolPaths;

    ParametersList* currentParameterListWidget;
    QList<QString> iconsPaths;
    void updateParameterEditor(QString searchName);
    void clearUgo();
    void clearIcons();
    void clearWidget();

    QLabel* iconDisplay;
    QList<QString> ugoDirsSymbolsPaths;
    QComboBox* modelsComboBox;
    QPushButton* delButton;

    QString* newThumbName;

    ParameterEditor* parameterEditor;

private:
    void setupUi();
    void setupConnections();

    QPushButton* selectIconBtn;

    UgoTabs* ugoTabs;
 
    QList<Library>* librariesList;
    QList<Catalog>* catalogsList;
    QList<Component>* componentsList;
    QList<Parameters>* parametersList;

    Parameters* currentParameter;
    QList<QString>* tabs;
public slots:
    void selectModel(const QString& text);

private slots:
    void onItemDoubleClicked(QListWidgetItem* item);
    void onParameterChanged();
    void updateParameterLink();
    void addNewParameter();
    void delParameter();
};