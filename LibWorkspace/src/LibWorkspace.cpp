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
    
    // Добавляем статус бар
    statusBar = new QStatusBar(this);
    setStatusBar(statusBar);

    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QHBoxLayout* mainLayout = new QHBoxLayout(centralWidget);
    QSplitter* splitter = new QSplitter(Qt::Vertical);

    splitter->addWidget(libraryManager);
    splitter->addWidget(componentsTable);

    QVBoxLayout* libLayout = new QVBoxLayout();

    libLayout->addWidget(splitter);

    resetButton = new QPushButton(QStringLiteral(u"Сброс"));

    libLayout->addWidget(resetButton);

    mainLayout->addLayout(libLayout);
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
    connect(resetButton, &QPushButton::clicked, this, &LibWorkspace::resetButtonClicked);
    connect(showFullTableAction, &QAction::triggered, this, &LibWorkspace::onShowFullTable);
    connect(newAction, &QAction::triggered, this, &LibWorkspace::openNewComponentDialog);
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

void LibWorkspace::openNewComponentDialog()
{
    dialog = new NewComponentDialog(this);

    if (dialog->exec() == QDialog::Accepted) {
        QString name = dialog->getName();
        QString library = dialog->getLibrary();
        QString directory = dialog->getDirectory();
        QString category = dialog->getCategory();

        createNewComponent(name, library, directory, category);
    }

    delete dialog; // Не забудьте очистить память
}

void LibWorkspace::createNewComponent(QString name, QString library, QString directory, QString category)
{
    Component* newComponent = new Component();

    newComponent->name = name;
    newComponent->library = library;
    newComponent->model = name;
    newComponent->parameters = *componentEditor->currentParameterListWidget->parameters;
    newComponent->thumbName = name;

    QString libPath = "./Libraries/" + currentLibrary->dir + "/library.json";
    nlohmann::json jsonObj = FillFromJsons::readJson(libPath, this);

    //QString componentsPath = "./Libraries/" + currentLibrary->dir;

    QString mainPath = "./Libraries/" + currentLibrary->dir;

    FillFromJsons::AddNewComponentToJson(jsonObj, *newComponent, currentCatalog->name, mainPath,
        *componentEditor->newThumbName, componentEditor->modelsComboBox->currentText());

}