#include "LibWorkspace.h"
#include <qheaderview.h>
#include <QSplitter>
#include <QString>
#include <QMessageBox>

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

    QLabel* label3 = new QLabel("ComponentEditor");
    QLabel* label4 = new QLabel("ParametrsList");

    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    /*auto buttonLayout = new QHBoxLayout();*/

    //refreshButton = new QPushButton(QIcon("icons/refresh.svg"), "", this);
    //buttonLayout->addWidget(refreshButton);

    QHBoxLayout* mainLayout = new QHBoxLayout(centralWidget);
    QSplitter* splitter = new QSplitter(Qt::Vertical);

    splitter->addWidget(libraryManager);
    splitter->addWidget(componentsTable);

    mainLayout->addWidget(splitter);
    mainLayout->addWidget(label3);
    mainLayout->addWidget(label4);
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

