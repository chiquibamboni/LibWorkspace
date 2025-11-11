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
    ParametersList* currentParameterListWidget;
    QList<QString> iconPaths;
    QLabel* iconDisplay;
    QList<QString> ugoDirsSymbolsPaths;
    QComboBox* modelsComboBox;
    QPushButton* delButton;

    QString* newThumbName;

    ParameterEditor* parameterEditor;

    void updateParameterEditor(Parameters currentParam);
    Parameters serchParam(QString searchName, QList<Parameters> curParametersList);
    void clearUgo();
    void clearIcons();
    void clearWidget();

private:
    QPushButton* selectIconBtn;

    UgoTabs* ugoTabs;
 
    QList<Library>* librariesList;
    QList<Catalog>* catalogsList;
    QList<Component>* componentsList;
    QList<Parameters>* parametersList;

    Parameters* currentParameter;
    QList<QString>* tabs;

    void setupUi();
    void setupConnections();
    void onItemDoubleClickedCurList(QListWidgetItem* item);
public slots:
    void selectModel(const QString& text);

private slots:
    void onItemDoubleClicked(QListWidgetItem* item);
    void onParameterChanged();
    void updateParameterLink();
    void addNewParameter();
    void delParameter();
};