#pragma once

#include <QtWidgets/QMainWindow>

#include "ComponentsTable.h"
#include "LibraryManager.h"
#include "ParametersList.h"
#include "ComponentEditor.h"

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
    void setupPath();
    void setupUI();
    void setupConnections();
    void setupToolBar();
    void setupMenuBar();
    LibraryManager* libraryManager;
    ComponentsTable* componentsTable;
    ComponentEditor* componentEditor;
    QToolBar* toolBar;
    QMenuBar* menuBar;

    Library* currentLibrary;
    Catalog* currentCatalog;

    QList<Library>* libraries;
    QList<Catalog>* catalogs;
    QList<Parameters>* parameters;

public slots:
    void RequestWithSelectedItem(const QModelIndex& index);
    void SelectComponent(const QModelIndex& index);
};

