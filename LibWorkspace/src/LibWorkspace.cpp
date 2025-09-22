#include "LibWorkspace.h"
#include <qheaderview.h>
#include <QSplitter>
#include <QString>
#include <QMessageBox>
#include <QMenu>


LibWorkspace::LibWorkspace(QWidget *parent)
    : QMainWindow(parent)
{
    setupUI();
    setupConnections();

    //первое отправление запроса
    libraryManager->request();
}

LibWorkspace::~LibWorkspace()
{}

void LibWorkspace::setupUI()
{
    QSize  iconSize(64, 64);

    libraryManager = new LibraryManager();
    libraryManager->setIconSize(iconSize);

    componentsTable = new ComponentsTable();
    componentsTable->setIconSize(iconSize);

    componentEditor = new ComponentEditor();

    //QLabel* label3 = new QLabel("ComponentEditor");
    //QLabel* label4 = new QLabel("ParametrsList");

    parametrsList = new QListWidget();

    QStringList items;
    items << "par 1" << "par 2" << "par 3";

    parametrsList->addItems(items);

    /*auto buttonLayout = new QHBoxLayout();*/

    //refreshButton = new QPushButton(QIcon("icons/refresh.svg"), "", this);
    //buttonLayout->addWidget(refreshButton);

    setupMenuBar();
    setupToolBar();

    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QHBoxLayout* mainLayout = new QHBoxLayout(centralWidget);
    QSplitter* splitter = new QSplitter(Qt::Vertical);

    splitter->addWidget(libraryManager);
    splitter->addWidget(componentsTable);

    mainLayout->addWidget(splitter);
    mainLayout->addWidget(componentEditor);
    mainLayout->addWidget(parametrsList);
}

void LibWorkspace::setupMenuBar()
{
    menuBar = new QMenuBar(this);
    setMenuBar(menuBar);

    QMenu* fileMenu = menuBar->addMenu(QStringLiteral(u"Файл"));

    QAction* newAction = new QAction(QStringLiteral(u"Создать"), this);
    newAction->setShortcut(QKeySequence::New);
    fileMenu->addAction(newAction);

    QAction* openAction = new QAction(QStringLiteral(u"Открыть"), this);
    openAction->setShortcut(QKeySequence::Open);
    fileMenu->addAction(openAction);

    fileMenu->addSeparator(); 

    QMenu* editMenu = menuBar->addMenu(QStringLiteral(u"Добавить"));

    QAction* libAction = new QAction(QStringLiteral(u"Библиотеку"), this);
    editMenu->addAction(libAction);

    QAction* catAction = new QAction(QStringLiteral(u"Каталог"), this);
    editMenu->addAction(catAction);

    QAction* compAction = new QAction(QStringLiteral(u"Компонент"), this);
    editMenu->addAction(compAction);

    fileMenu->addSeparator();

    QMenu* helpMenu = menuBar->addMenu(QStringLiteral(u"Помощь"));
}

void LibWorkspace::setupToolBar()
{
    toolBar = new QToolBar(this);
    toolBar->setIconSize(QSize(32, 32));
    toolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);
    addToolBar(Qt::TopToolBarArea, toolBar);

    QAction* newAction = new QAction(QIcon("icons/plus.svg"), QStringLiteral(u"Добавить"), this);
    QAction* deleteAction = new QAction(QIcon("icons/cross.svg"), QStringLiteral(u"Удалить"), this);
    QAction* downAction = new QAction(QIcon("icons/arrow-down.svg"), QStringLiteral(u"Вниз"), this);
    QAction* upAction = new QAction(QIcon("icons/arrow-up.svg"), QStringLiteral(u"Вверх"), this);

    toolBar->addAction(newAction);
    toolBar->addAction(deleteAction);
    toolBar->addAction(downAction);
    toolBar->addAction(upAction);
}

void LibWorkspace::setupConnections()
{
    connect(libraryManager, &QTreeView::clicked, this, &LibWorkspace::RequestWithSelectedItem);
    connect(libraryManager, &QTreeView::expanded, this, &LibWorkspace::RequestWithSelectedItem);

    //привязка сигналов для кнопок
    //connect(refreshButton, &QPushButton::clicked, this, &LibWorkspace::refreshButtonClicked);
}

void LibWorkspace::RequestWithSelectedItem(const QModelIndex& index)
{
    QString selectedItem = libraryManager->model->data(index, Qt::DisplayRole).toString();
    if (!index.isValid()) {
        return;
    }
    for (auto& lib : *libraryManager->libraries) {
        if (lib.name == selectedItem) {
            QString fullPath = "./Libraries/" + lib.dir;
            libraryManager->currentPath = fullPath;
            libraryManager->currentLibrary = &lib;
            libraryManager->request();
            componentsTable->setRowCount(0);
            return;
        }
    }
    for (auto& catalog : *libraryManager->catalogs)
    {
        if (catalog.name == selectedItem) {
            libraryManager->currentCatalog = &catalog;
            //Обновление данных компонентов таблицы
            if (!libraryManager->currentCatalog->components.isEmpty())
            {
                componentsTable->updateComponents(libraryManager->currentCatalog->components);
                return;
            }
            componentsTable->setRowCount(0);
            return;
        }
    }
}

//void LibWorkspace::refreshButtonClicked()
//{
//    libraryManager->request();
//    componentsTable->setRowCount(0);
//}

