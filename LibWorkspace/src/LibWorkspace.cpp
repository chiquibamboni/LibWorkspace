#include "LibWorkspace.h"
#include <qheaderview.h>
#include <QSplitter>
#include <QString>
#include <QMessageBox>
#include <QMenu>
#include <QDir>
#include <QDebug>
#include "FillFromJsons.h"

LibWorkspace::LibWorkspace(QWidget* parent)
    : QMainWindow(parent)
{
    setupUI();
    setupConnections();
    
    // Установка статус бара для обработчика ошибок
    FillFromJsons::setStatusBar(statusBar);

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
        
        // Проверяем, существует ли каталог библиотеки
        QDir libDir(fullPath);
        if (!libDir.exists()) {
            QString message = QString(QStringLiteral(u"Каталог библиотеки не найден: %1")).arg(fullPath);
            FillFromJsons::showError(this, message);
            continue;
        }
        
        libraryManager->currentPath = fullPath;
        libraryManager->currentLibrary = &lib;
        libraryManager->request();

        //добавление пути для параметров в parametersListWidget
        QString location = "./Libraries/" + libraryManager->currentLibrary->dir + "/" + libraryManager->currentLibrary->components_location;
        QDir dir(location);
        if (dir.exists()) {
            componentEditor->parametersListWidget->location = location;
            componentEditor->parametersListWidget->setItems();
            componentEditor->parameterEditor->loadComboBoxData(componentEditor->parametersListWidget->parameters);
        } else {
            QString message = QString(QStringLiteral(u"Каталог компонентов библиотеки не найден: %1")).arg(location);
            FillFromJsons::showError(this, message);
        }
        
        //добавление пути для иконок в thumbSelectDialog
        QString iconsPath = "./Libraries/" + libraryManager->currentLibrary->dir + "/" + libraryManager->currentLibrary->thumbnails_location;
        QDir dirp(iconsPath);
        if (dirp.exists()) {
            componentEditor->iconPaths.append(iconsPath);
        } else {
            QString message = QString(QStringLiteral(u"Каталог иконок библиотеки не найден: %1")).arg(iconsPath);
            FillFromJsons::showError(this, message);
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
        } else {
            QString message = QString(QStringLiteral(u"Каталог символов библиотеки не найден: %1")).arg(symbolsPath);
            FillFromJsons::showError(this, message);
        }
    }
    //Заполнение компонентами
    for (auto& catalog : *catalogs)
    {
        if (!catalog.catalogs.isEmpty())
        {
            for (auto& subCat : catalog.catalogs)
            {
                for (auto& component : subCat.components)
                {
                    componentEditor->modelsComboBox->addItem(component.model);
                    components->push_back(component);
                }
            }
        }
        for (auto& component : catalog.components)
        {
            componentEditor->modelsComboBox->addItem(component.model);
            components->push_back(component);
        }
    }
    componentsTable->setRowCount(0);
}

void LibWorkspace::setupUI()
{
    QSize iconSize(64, 64);

    libraries = new QList<Library>();
    catalogs = new QList<Catalog>();
    components = new QList<Component>();
    parameters = new QList<Parameters>();

    libraryManager = new LibraryManager(libraries, catalogs);
    libraryManager->setIconSize(iconSize);

    componentsTable = new ComponentsTable();
    componentsTable->setIconSize(iconSize);

    componentEditor = new ComponentEditor(libraries, catalogs, components, parameters);

    setupMenuBar();
    setupToolBar();

    statusBar = new QStatusBar(this);
    setStatusBar(statusBar);

    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QHBoxLayout* mainLayout = new QHBoxLayout(centralWidget);
    QSplitter* splitter = new QSplitter(Qt::Vertical);

    splitter->addWidget(libraryManager);
    splitter->addWidget(componentsTable);

    QWidget* leftPanel = new QWidget();
    QVBoxLayout* leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->addWidget(splitter);

    resetButton = new QPushButton(QStringLiteral(u"Сброс"));
    leftLayout->addWidget(resetButton);

    mainLayout->addWidget(leftPanel, 1);
    mainLayout->addWidget(componentEditor, 2);
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

    catAction = new QAction(QStringLiteral(u"Каталог"), this);
    editMenu->addAction(catAction);

    compAction = new QAction(QStringLiteral(u"Компонент"), this);
    editMenu->addAction(compAction);

    fileMenu->addSeparator();

    QMenu* viewMenu = menuBar->addMenu(QStringLiteral(u"Вид"));
    showFullTableAction = viewMenu->addAction(QStringLiteral(u"Показать полную таблицу компонентов"));

    fileMenu->addSeparator();

    QMenu* helpMenu = menuBar->addMenu(QStringLiteral(u"Помощь"));
}

void LibWorkspace::setupToolBar()
{
    toolBar = new QToolBar(this);
    toolBar->setIconSize(QSize(32, 32));
    toolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);
    addToolBar(Qt::TopToolBarArea, toolBar);

    newAction = new QAction(QIcon("icons/plus.svg"), QStringLiteral(u"Добавить"), this);
    deleteAction = new QAction(QIcon("icons/cross.svg"), QStringLiteral(u"Удалить"), this);
    QAction* downAction = new QAction(QIcon("icons/arrow-down.svg"), QStringLiteral(u"Вниз"), this);
    QAction* upAction = new QAction(QIcon("icons/arrow-up.svg"), QStringLiteral(u"Вверх"), this);
    refreshAction = new QAction(QIcon("icons/refresh.svg"), QStringLiteral(u"Обновить"), this);

    toolBar->addAction(newAction);
    toolBar->addAction(deleteAction);
    toolBar->addAction(downAction);
    toolBar->addAction(upAction);
    toolBar->addAction(refreshAction);
}

void LibWorkspace::setupConnections()
{
    connect(libraryManager, &QTreeView::clicked, this, &LibWorkspace::RequestWithSelectedItem);
    connect(libraryManager, &QTreeView::expanded, this, &LibWorkspace::RequestWithSelectedItem);
    connect(libraryManager, &QTreeView::collapsed, this, &LibWorkspace::RequestWithSelectedItem);
    connect(resetButton, &QPushButton::clicked, this, &LibWorkspace::resetButtonClicked);
    connect(showFullTableAction, &QAction::triggered, this, &LibWorkspace::onShowFullTable);
    connect(newAction, &QAction::triggered, this, &LibWorkspace::openNewComponentDialog);
    connect(compAction, &QAction::triggered, this, &LibWorkspace::openNewComponentDialog);
    connect(catAction, &QAction::triggered, this, &LibWorkspace::openNewCatalogDialog);
    connect(deleteAction, &QAction::triggered, this, &LibWorkspace::openDeleteDialog);
    connect(refreshAction, &QAction::triggered, this, &LibWorkspace::refresh);
    connect(componentsTable->selectionModel(), &QItemSelectionModel::currentChanged, this, &LibWorkspace::SelectComponent);
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
            currentCatalog = nullptr;
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
                SelectComponent(componentsTable->model()->index(0, 0));
                currentComponent = nullptr;
                return;
            }
            return;
        }
        else {
            for (auto& subCat : catalog.catalogs)
            {
                if (subCat.name == selectedItem) {
                    currentCatalog = libraryManager->currentCatalog = &subCat;
                    //Обновление данных компонентов таблицы
                    if (!libraryManager->currentCatalog->components.isEmpty())
                    {
                        componentsTable->updateComponents(libraryManager->currentCatalog->components);
                        SelectComponent(componentsTable->model()->index(0, 0));
                        currentComponent = nullptr;
                        return;
                    }
                    return;
                }
            }
        }

    }
}

void LibWorkspace::SelectComponent(const QModelIndex& index)
{
    int row = index.row();
    if (row < 0) {
        return;
    }
    QString searchName = componentsTable->item(row, 1)->text();

    *componentEditor->newThumbName = searchName;

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
            currentComponent = &component;
        }
    }
}
void LibWorkspace::resetButtonClicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this,
        QStringLiteral(u"Подтверждение"),
        QStringLiteral(u"Вы действительно хотите сбросить библиотеки до начальных?"),
        QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        // Проверяем существование каталогов перед сбросом
        QDir librariesDir("./Libraries");
        QDir defaultLibrariesDir("./DefaultLibraries");
        
        if (!defaultLibrariesDir.exists()) {
            QString message = QStringLiteral(u"Каталог DefaultLibraries не найден");
            FillFromJsons::showError(this, message);
            return;
        }
        
        clearDirectory("./Libraries");
        copyDirectoryContents("./DefaultLibraries", "./Libraries");
        componentEditor->clearWidget();
        libraryManager->clearLibraries();
        parameters->clear();
        libraries->clear();
        catalogs->clear();
        libraryManager->request();
        setupFields();
        componentsTable->setRowCount(0);
    }
    return;
}

void LibWorkspace::onShowFullTable()
{
    FullTableDialog* dialog = new FullTableDialog(*components,this);
    dialog->exec();
}

bool LibWorkspace::clearDirectory(const QString& dirPath)
{
    QDir dir(dirPath);
    if (!dir.exists()) {
        QString message = QString(QStringLiteral(u"Каталог для очистки не существует: %1")).arg(dirPath);
        FillFromJsons::showError(this, message);
        return false;
    }

    QFileInfoList list = dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot);
    for (const QFileInfo& info : list) {
        if (info.isFile()) {
            if (!dir.remove(info.fileName())) {
                QString message = QString(QStringLiteral(u"Не удалось удалить файл: %1")).arg(info.filePath());
                qDebug() << message;
            }
        }
        else if (info.isDir()) {
            if (!clearDirectory(info.filePath())) {
                QString message = QString(QStringLiteral(u"Не удалось очистить подкаталог: %1")).arg(info.filePath());
                qDebug() << message;
            }
            if (!dir.rmdir(info.fileName())) {
                QString message = QString(QStringLiteral(u"Не удалось удалить каталог: %1")).arg(info.filePath());
                qDebug() << message;
            }
        }
    }
    return true;
}

bool LibWorkspace::copyDirectoryContents(const QString& sourceDirPath, const QString& targetDirPath)
{
    QDir sourceDir(sourceDirPath);
    QDir targetDir(targetDirPath);

    if (!sourceDir.exists()) {
        QString message = QString(QStringLiteral(u"Источник не существует: %1")).arg(sourceDirPath);
        FillFromJsons::showError(this, message);
        return false;
    }
    if (!targetDir.exists()) {
        if (!targetDir.mkpath(".")) {
            QString message = QString(QStringLiteral(u"Не удалось создать папку назначения: %1")).arg(targetDirPath);
            FillFromJsons::showError(this, message);
            return false;
        }
    }

    QFileInfoList list = sourceDir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot);
    for (const QFileInfo& info : list) {
        QString srcPath = info.absoluteFilePath();
        QString destPath = QDir(targetDir).filePath(info.fileName());

        if (info.isDir()) {
            QDir().mkpath(destPath);
            if (!copyDirectoryContents(srcPath, destPath)) {
                QString message = QString(QStringLiteral(u"Не удалось скопировать каталог: %1")).arg(srcPath);
                qDebug() << message;
            }
        }
        else if (info.isFile()) {
            if (!QFile::copy(srcPath, destPath)) {
                QString message = QString(QStringLiteral(u"Не удалось скопировать файл: %1")).arg(srcPath);
                qDebug() << message;
            }
        }
    }
    return true;
}

void LibWorkspace::openDeleteDialog()
{
    if (!currentLibrary)
    {
        QMessageBox::warning(this, QStringLiteral(u"Ошибка"), QStringLiteral(u"Выберите библиотеку/каталог/компонент, который хотите удалить"));
        return;
    }

    DeleteDialog*  delDialog = new DeleteDialog(currentLibrary, currentCatalog, currentComponent, this);

    if (delDialog->exec() == QDialog::Accepted) {

        refresh();
    }

    delete delDialog;
}

void LibWorkspace::openNewComponentDialog()
{
    dialogComp = new NewComponentDialog(libraries, catalogs, components, this);

    if (dialogComp->exec() == QDialog::Accepted) {
        QString name = dialogComp->getName();
        QString library = dialogComp->getLibrary();
        QString directory = dialogComp->getDirectory();
        QString category = dialogComp->getCategory();
        QString desc = dialogComp->getDesc();

        createNewComponent(name, library, category, desc);
        refresh();
    }

    delete dialogComp;
}

void LibWorkspace::openNewCatalogDialog()
{
    dialogCat = new NewCatalogDialog(libraries, catalogs, this);

    if (dialogCat->exec() == QDialog::Accepted) {
        QString name = dialogCat->getName();
        QString library = dialogCat->getLibrary();
        QString directory = dialogCat->getDirectory();

        createNewCatalog(name, library, directory);
        refresh();
    }

    delete dialogCat;
}

void LibWorkspace::createNewComponent(QString name, QString library, QString category, QString desc)
{
    Component* newComponent = new Component();
    QString libPath;
    QString mainPath;

    newComponent->name = name;
    newComponent->desc = desc;
    newComponent->library = library;
    newComponent->model = name;
    newComponent->parameters = *componentEditor->currentParameterListWidget->parameters;
    newComponent->thumbName = name;

    for (auto& lib : *libraries) {
        if (lib.name == newComponent->library) {
            libPath = "./Libraries/" + lib.dir + "/library.json";
            mainPath = "./Libraries/" + lib.dir;
            break;
        }
    }
    
    nlohmann::json jsonObj = FillFromJsons::readJson(libPath, this);

    FillFromJsons::AddNewComponentToJson(jsonObj, *newComponent, category, mainPath,
        *componentEditor->newThumbName, componentEditor->modelsComboBox->currentText());
}

void LibWorkspace::createNewCatalog(QString name, QString library, QString directory)
{
    QString libPath;
    QString mainPath;

    for (auto& lib : *libraries) {
        if (lib.name == library) {
            libPath = "./Libraries/" + lib.dir + "/library.json";
            mainPath = "./Libraries/" + lib.dir;
            break;
        }
    }

    nlohmann::json jsonObj = FillFromJsons::readJson(libPath, this);

    FillFromJsons::AddNewCatalogToJson(jsonObj, library, directory, name, mainPath, "");

}

void LibWorkspace::refresh()
{
    componentEditor->clearWidget();
    libraryManager->clearLibraries();
    parameters->clear();
    libraries->clear();
    catalogs->clear();
    components->clear();
    libraryManager->request();
    setupFields();
    componentsTable->setRowCount(0);
}