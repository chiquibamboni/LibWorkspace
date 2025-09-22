#pragma once

#include <QtWidgets/QMainWindow>

#include "ComponentsTable.h"
#include "LibraryManager.h"

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
    QTableWidget* table;
    LibraryManager* libraryManager;
    ComponentsTable* componentsTable;
    QPushButton* refreshButton;
    QToolBar* toolBar;
    QListWidget* parametrsList;
    QMenuBar* menuBar;

public slots:
    void RequestWithSelectedItem(const QModelIndex& index);
    //void refreshButtonClicked();
    void setupConnections();
    void setupToolBar();
    void setupMenuBar();
};

