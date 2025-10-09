#include "LibWorkspace.h"
#include <qheaderview.h>
#include <QSplitter>
#include <QString>
#include <QMessageBox>
#include <QMenu>
#include <QDir>


LibWorkspace::LibWorkspace(QWidget* parent)
    : QMainWindow(parent)
{
    setupUI();
    setupConnections();

    //первое отправление запроса
    libraryManager->request();

    setupFields();
    componentsTable->setRowCount(0);
}

LibWorkspace::~LibWorkspace()
{
}

void LibWorkspace::setupFields()
{
    //Заполнение библиотек
    for (auto& lib : *libraries) {
        QString fullPath = "./Libraries/" + lib.dir;
        libraryManager->currentPath = fullPath;
        libraryManager->currentLibrary = &lib;
        libraryManager->request();

        //добавление пути для параметров в parametersListWidget
        QString location = "./Libraries/" + libraryManager->currentLibrary->dir + "/" + libraryManager->currentLibrary->components_location;
        QDir dir(location);
        if (dir.exists()) {
            componentEditor->parametersListWidget->location = location;
            componentEditor->parametersListWidget->setItems();
        }
        //добавление пути для иконок в thumbSelectDialog
        QString iconsPath = "./Libraries/" + libraryManager->currentLibrary->dir + "/" + libraryManager->currentLibrary->thumbnails_location;
        QDir dirp(iconsPath);
        if (dirp.exists()) {
            componentEditor->iconPaths.append(iconsPath);
        }
        //добавление пути для УГО в ugoTabs
        QString symbolsPath = "./Libraries/" + libraryManager->currentLibrary->dir + "/" + libraryManager->currentLibrary->symbols_location;
        QDir dirs(symbolsPath);
        if (dirs.exists()) {
            QStringList folderNames = dirs.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
            for (const QString& folderName : folderNames)
            {
                componentEditor->symbolPaths.append(symbolsPath + "/" + folderName);
            }
        }
    }
    //Заполнение компонентами
    for (auto& catalog : *catalogs)
    {
        for (auto& component : catalog.components)
        {
            componentEditor->modelsComboBox->addItem(component.model);
        }
    }
    componentsTable->setRowCount(0);
}

void LibWorkspace::setupUI()
{
    QSize  iconSize(64, 64);

    libraries = new QList<Library>();
    catalogs = new QList<Catalog>();
    parameters = new QList<Parameters>();

    libraryManager = new LibraryManager(libraries, catalogs);
    libraryManager->setIconSize(iconSize);

    componentsTable = new ComponentsTable();
    componentsTable->setIconSize(iconSize);

    componentEditor = new ComponentEditor(libraries, catalogs, parameters);

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

    mainLayout->setStretchFactor(splitter, 3);
    mainLayout->setStretchFactor(componentEditor, 7);

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
    connect(libraryManager, &QTreeView::collapsed, this, &LibWorkspace::RequestWithSelectedItem);
    connect(componentsTable, &QTableWidget::doubleClicked, this, &LibWorkspace::SelectComponent);
}

void LibWorkspace::RequestWithSelectedItem(const QModelIndex& index)
{
    QString selectedItem = libraryManager->model->data(index, Qt::DisplayRole).toString();
    if (!index.isValid()) {
        return;
    }
    componentsTable->setRowCount(0);
    componentEditor->clearUgo();
    componentEditor->clearIcons();
    for (auto& lib : *libraries) {
        if (lib.name == selectedItem) {
            QString fullPath = "./Libraries/" + lib.dir;
            libraryManager->currentPath = fullPath;
            currentLibrary = libraryManager->currentLibrary = &lib;
            return;
        }
    }
    for (auto& catalog : *catalogs)
    {
        if (catalog.name == selectedItem) {
            currentCatalog = libraryManager->currentCatalog = &catalog;
            //Обновление данных компонентов таблицы
            if (!libraryManager->currentCatalog->components.isEmpty())
            {
                componentsTable->updateComponents(libraryManager->currentCatalog->components);
                return;
            }
            return;
        }
    }
}

void LibWorkspace::SelectComponent(const QModelIndex& index)
{
    int row = index.row();
    QString searchName = componentsTable->item(row, 1)->text();
    
    int modelIndex = componentEditor->modelsComboBox->findText(searchName);
    if (modelIndex != -1) {
        componentEditor->modelsComboBox->setCurrentIndex(modelIndex);
    }
    componentEditor->selectModel(searchName);
    QString fullPath;
    for (auto& component : currentCatalog->components)
    {
        if (component.model == searchName)
        {
            QIcon icon = component.thumb;
            QPixmap pixmap = icon.pixmap(componentEditor->iconDisplay->size());
            componentEditor->iconDisplay->setPixmap(pixmap);
            componentEditor->currentParameterListWidget->clearItems();;
            for (Parameters par : component.parameters)
            {
                componentEditor->currentParameterListWidget->ParametersList::insertItem(par);
            }
        }
    }
}