#pragma once

#include <QtWidgets/QMainWindow>

#include "ComponentsTable.h"
#include "LibraryManager.h"
#include "ParametersList.h"
#include "ComponentEditor.h"
#include "FullTableDialog.h"
#include "NewComponentDialog.h"
#include "DeleteDialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QKeyEvent>
#include <QPushButton>
#include <QToolBar> 
#include <QLabel>
#include <QListWidget>
#include <QMenuBar>
#include <QStatusBar>

class LibWorkspace : public QMainWindow
{
    Q_OBJECT

public:
    LibWorkspace(QWidget *parent = nullptr);
    ~LibWorkspace();

private:
    void setupFields();
    void setupUI();
    void setupConnections();
    void setupToolBar();
    void setupMenuBar();
    LibraryManager* libraryManager;
    ComponentsTable* componentsTable;
    ComponentEditor* componentEditor;
    QToolBar* toolBar;
    QMenuBar* menuBar;
    QAction* newAction;
    QAction* deleteAction;
    QAction* refreshAction;
    NewComponentDialog* dialog;
    QAction* showFullTableAction;
    QStatusBar* statusBar; // Добавляем статус бар

    void createNewComponent(QString name, QString library, QString directory, QString category);

    Library* currentLibrary;
    Catalog* currentCatalog;
    Component* currentComponent;

    QList<Library>* libraries;
    QList<Catalog>* catalogs;
    QList<Component>* components;
    QList<Parameters>* parameters;


    QPushButton* resetButton;

    bool clearDirectory(const QString& dirPath);
    bool copyDirectoryContents(const QString& sourceDirPath, const QString& targetDirPath);


private slots:
    void RequestWithSelectedItem(const QModelIndex& index);
    void SelectComponent(const QModelIndex& index);
    void resetButtonClicked();
    void onShowFullTable();
    void openNewComponentDialog();
    void openDeleteDialog();
    void refresh();
};

