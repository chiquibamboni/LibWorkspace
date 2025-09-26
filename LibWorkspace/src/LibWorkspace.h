#pragma once

#include <QtWidgets/QMainWindow>

#include "ComponentsTable.h"
#include "LibraryManager.h"
#include "ParametersList.h"
#include "ParameterEditor.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QKeyEvent>
#include <QPushButton>
#include <QToolBar> 
#include <QLabel>
#include <QListWidget>
#include <QMenuBar>

class LibWorkspace : public QMainWindow
{
    Q_OBJECT

public:
    LibWorkspace(QWidget *parent = nullptr);
    ~LibWorkspace();

private:
    void setupUI();
    void setupConnections();
    void setupToolBar();
    void setupMenuBar();
    Parameters parseTooltip(const QString &tooltip);
    LibraryManager* libraryManager;
    ComponentsTable* componentsTable;
    ParametersList* parametersList;
    ParameterEditor* parameterEditor;
   // QPushButton* refreshButton;
    QToolBar* toolBar;
    QMenuBar* menuBar;

    QList<Library>* libraries;
    QList<Catalog>* catalogs;
    QList<Parameters>* parameters;

public slots:
    void RequestWithSelectedItem(const QModelIndex& index);
    //void refreshButtonClicked();
    void onItemDoubleClicked(QListWidgetItem* item);
};

