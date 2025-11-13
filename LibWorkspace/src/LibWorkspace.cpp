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

void LibWorkspace::closeEvent(QCloseEvent* event)
{
    if (checkChanges())
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle(QStringLiteral("Несохранённые изменения"));
        msgBox.setText(QStringLiteral("У вас есть несохранённые изменения. Вы действительно хотите закрыть?"));
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setButtonText(QMessageBox::Yes, QStringLiteral(u"Да"));
        msgBox.setButtonText(QMessageBox::No, QStringLiteral(u"Нет"));
        msgBox.setDefaultButton(QMessageBox::No);
        msgBox.setIcon(QMessageBox::Warning);

        if (msgBox.exec() == QMessageBox::Yes)
        {
            event->accept();
        }
        else
        {
            event->ignore();
        }
    }
    else
    {
        event->accept();
    }
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
            componentEditor->parameterEditor->clearEditor();
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
                    //if (!component.ugo.ansiUgoSymbol.isNull() || !component.ugo.gostUgoSymbol.isNull())
                        if (componentEditor->modelsComboBox->findText(component.ugo.model) == -1) {
                            componentEditor->modelsComboBox->addItem(component.ugo.model);
                        }
                     components->push_back(component);
                }
            }
        }
        for (auto& component : catalog.components)
        {
            //if (!component.ugo.ansiUgoSymbol.isNull() || !component.ugo.gostUgoSymbol.isNull())
                if (componentEditor->modelsComboBox->findText(component.ugo.model) == -1) {
                    componentEditor->modelsComboBox->addItem(component.ugo.model);
                }
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

    QMenu* editMenu = menuBar->addMenu(QStringLiteral(u"Добавить"));

    //QAction* libAction = new QAction(QStringLiteral(u"Библиотеку"), this);
    //editMenu->addAction(libAction);

    catAction = new QAction(QStringLiteral(u"Каталог"), this);
    editMenu->addAction(catAction);

    compAction = new QAction(QStringLiteral(u"Компонент"), this);
    editMenu->addAction(compAction);

    QMenu* viewMenu = menuBar->addMenu(QStringLiteral(u"Вид"));
    showFullTableAction = viewMenu->addAction(QStringLiteral(u"Показать полную таблицу компонентов"));
}

void LibWorkspace::setupToolBar()
{
    toolBar = new QToolBar(this);
    toolBar->setIconSize(QSize(32, 32));
    toolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);
    addToolBar(Qt::TopToolBarArea, toolBar);

    newAction = new QAction(QIcon("icons/plus.svg"), QStringLiteral(u"Добавить"), this);
    deleteAction = new QAction(QIcon("icons/cross.svg"), QStringLiteral(u"Удалить"), this);
    downAction = new QAction(QIcon("icons/arrow-down.svg"), QStringLiteral(u"Вниз"), this);
    upAction = new QAction(QIcon("icons/arrow-up.svg"), QStringLiteral(u"Вверх"), this);
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
    connect(newAction, &QAction::triggered, this, [this]() {
        openNewComponentDialog(false);
        });

    connect(compAction, &QAction::triggered, this, [this]() {
        openNewComponentDialog(false);
        });
    connect(catAction, &QAction::triggered, this, &LibWorkspace::openNewCatalogDialog);
    connect(deleteAction, &QAction::triggered, this, &LibWorkspace::openDeleteDialog);
    connect(refreshAction, &QAction::triggered, this, &LibWorkspace::refresh);
    connect(downAction, &QAction::triggered, this, &LibWorkspace::openMoveDownDialog);
    connect(upAction, &QAction::triggered, this, &LibWorkspace::openMoveUpDialog);
    connect(componentsTable->selectionModel(), &QItemSelectionModel::currentChanged, this, &LibWorkspace::SelectComponent);
    connect(componentEditor->saveButton, &QPushButton::clicked, this, &LibWorkspace::saveChanges);
}

void LibWorkspace::RequestWithSelectedItem(const QModelIndex& index)
{
    QString selectedItem = libraryManager->model->data(index, Qt::DisplayRole).toString();
    QModelIndex parentIndex = index.parent();
    QString parentItem = libraryManager->model->data(parentIndex, Qt::DisplayRole).toString();
    if (!index.isValid()) {
        return;
    }
    componentsTable->setRowCount(0);
    for (auto& lib : *libraries) {
        if (lib.name == selectedItem) {
            if (currentComponent != nullptr) {
                currentComponent = nullptr;
                componentEditor->clearUgo();
                componentEditor->clearIcons();
                componentEditor->modelsComboBox->setCurrentText("");
                componentEditor->currentParameterListWidget->clearItems();
                componentEditor->newThumbName = new QString();
            }
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
            if (currentComponent != nullptr) {
                currentComponent = nullptr;
                componentEditor->clearUgo();
                componentEditor->clearIcons();
                componentEditor->modelsComboBox->setCurrentText("");
                componentEditor->currentParameterListWidget->clearItems();
                componentEditor->newThumbName = new QString();
                componentEditor->saveButton->setVisible(false);
            }
            currentCatalog = libraryManager->currentCatalog = &catalog;
            //Обновление данных компонентов таблицы
            if (!libraryManager->currentCatalog->components.isEmpty())
            {
                componentsTable->updateComponents(libraryManager->currentCatalog->components);
                //SelectComponent(componentsTable->model()->index(0, 0));
                currentComponent = nullptr;
                componentEditor->clearUgo();
                componentEditor->clearIcons();
                componentEditor->modelsComboBox->setCurrentText("");
                componentEditor->currentParameterListWidget->clearItems();
                componentEditor->newThumbName = new QString();
                componentEditor->saveButton->setVisible(false);
                return;
            }
            return;
        }
        else if (catalog.name == parentItem){
            if (currentComponent != nullptr) {
                currentComponent = nullptr;
                componentEditor->clearUgo();
                componentEditor->clearIcons();
                componentEditor->modelsComboBox->setCurrentText("");
                componentEditor->currentParameterListWidget->clearItems();
                componentEditor->newThumbName = new QString();
                componentEditor->saveButton->setVisible(false);
            }
            for (auto& subCat : catalog.catalogs)
            {
                if (subCat.name == selectedItem) {
                    componentEditor->saveButton->setVisible(false);
                    currentCatalog = libraryManager->currentCatalog = &subCat;
                    //Обновление данных компонентов таблицы
                    if (!libraryManager->currentCatalog->components.isEmpty())
                    {
                        componentsTable->updateComponents(libraryManager->currentCatalog->components);
                        //SelectComponent(componentsTable->model()->index(0, 0));
                        currentComponent = nullptr;
                        return;
                    }
                    currentComponent = nullptr;
                    return;
                }
            }
        }

    }
}


bool LibWorkspace::compareParametersLists(const QList<Parameters>& list1, const QList<Parameters>& list2)
{
    if (list1.size() != list2.size()) {
        return false;
    }

    for (int i = 0; i < list1.size(); ++i) {
        const Parameters& param1 = list1.at(i);
        const Parameters& param2 = list2.at(i);

        if (param1.ref != param2.ref ||
            param1.name != param2.name ||
            param1.type != param2.type ||
            param1.factor != param2.factor ||
            param1.feature != param2.feature ||
            param1.unit != param2.unit ||
            param1.desc != param2.desc ||
            param1.display != param2.display) {
            return false;
        }

        if (param1.pdefault.has_value() != param2.pdefault.has_value()) {
            return false;
        }
        if (param1.pdefault.has_value() && param2.pdefault.has_value()) {
            if (param1.pdefault.value() != param2.pdefault.value()) {
                return false;
            }
        }

        if (param1.optimizable.has_value() != param2.optimizable.has_value()) {
            return false;
        }
        if (param1.optimizable.has_value() && param2.optimizable.has_value()) {
            if (param1.optimizable.value() != param2.optimizable.value()) {
                return false;
            }
        }

        if (param1.edited.has_value() != param2.edited.has_value()) {
            return false;
        }
        if (param1.edited.has_value() && param2.edited.has_value()) {
            if (param1.edited.value() != param2.edited.value()) {
                return false;
            }
        }

        if (param1.netlisted.has_value() != param2.netlisted.has_value()) {
            return false;
        }
        if (param1.netlisted.has_value() && param2.netlisted.has_value()) {
            if (param1.netlisted.value() != param2.netlisted.value()) {
                return false;
            }
        }
    }

    return true;
}

void LibWorkspace::SelectComponent(const QModelIndex& index)
{
    bool changesMark = checkChanges();
    if (changesMark)
        return;

    componentEditor->saveButton->setVisible(true);

    int row = index.row();
    if (row < 0) {
        return;
    }
    QString searchName = componentsTable->item(row, 1)->text();

    QString fullPath;
    for (auto& component : currentCatalog->components)
    {
        if (component.model == searchName)
        {
            QIcon icon = component.thumb;
            QPixmap pixmap = icon.pixmap(componentEditor->iconDisplay->size());
            componentEditor->iconDisplay->setPixmap(pixmap);
            componentEditor->currentParameterListWidget->clearItems();;
            componentEditor->newThumbName = &component.thumbName;
            componentEditor->selectModel(component.ugo.model);
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
    QMessageBox msgBox;
    msgBox.setWindowTitle(QStringLiteral(u"Подтверждение сброса"));
    msgBox.setText(QStringLiteral(u"Вы действительно хотите сбросить библиотеки до начальных?"));
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setButtonText(QMessageBox::Yes, QStringLiteral(u"Да"));
    msgBox.setButtonText(QMessageBox::No, QStringLiteral(u"Нет"));

    if (msgBox.exec() == QMessageBox::Yes) {
        QDir librariesDir("./Libraries");
        QDir defaultLibrariesDir("./DefaultLibraries");
        
        if (!defaultLibrariesDir.exists()) {
            QString message = QStringLiteral(u"Каталог DefaultLibraries не найден");
            FillFromJsons::showError(this, message);
            return;
        }
        
        clearDirectory("./Libraries");
        copyDirectoryContents("./DefaultLibraries", "./Libraries");
        reset = true;
        refresh();
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

void LibWorkspace::openMoveDownDialog()
{
    QString direction = "down";

    if (!currentComponent)
    {
        QMessageBox::warning(this, QStringLiteral(u"Ошибка"), QStringLiteral(u"Выберите компонент, который хотите переместить"));
        return;
    }

    MoveDialog* moveDialog = new MoveDialog(currentLibrary, currentCatalog, currentComponent, direction, this);

    if (moveDialog->exec() == QDialog::Accepted) {

        refresh();
    }

    delete moveDialog;
}

void LibWorkspace::openMoveUpDialog()
{
    QString direction = "up";

    if (!currentComponent)
    {
        QMessageBox::warning(this, QStringLiteral(u"Ошибка"), QStringLiteral(u"Выберите компонент, который хотите переместить"));
        return;
    }

    MoveDialog* moveDialog = new MoveDialog(currentLibrary, currentCatalog, currentComponent, direction, this);

    if (moveDialog->exec() == QDialog::Accepted) {

        refresh();
    }

    delete moveDialog;
}

bool LibWorkspace::openNewComponentDialog(bool edting)
{
    dialogComp = new NewComponentDialog(libraries, catalogs, components, 
        componentEditor->currentParameterListWidget->parameters,
        *componentEditor->newThumbName, componentEditor->modelsComboBox->currentText(), this);

    if (currentComponent != nullptr)
    {
        dialogComp->loadCurrentCompData(*currentComponent, *currentCatalog);
    }
    else if (currentCatalog != nullptr)
    {
        dialogComp->loadCurrentCatData(*currentCatalog);
    }

    if (edting == true) {
        dialogComp->editMakeDialog();
    }

    if (dialogComp->exec() == QDialog::Accepted) {
        refresh();
    }
    else
    {

        delete dialogComp;
        return false;
    }

    delete dialogComp;
    return true;
}

bool LibWorkspace::checkChanges()
{
    bool f1 = !componentEditor->currentParameterListWidget->parameters->isEmpty();
    bool f2 = componentEditor->modelsComboBox->currentText() != "";
    bool f3 = !componentEditor->newThumbName->isEmpty();
    bool f4 = true;
    if (currentComponent)
    {
        try
        {
            f4 = (currentComponent->model == nullptr);
        }
        catch (...)
        {
            f4 = true;
        }
    }

    if ((f1 || f2 || f3) && f4)
    {
        QMessageBox reply;
        reply.setWindowTitle(QStringLiteral(u"Подтверждение"));
        reply.setText(QStringLiteral(u"Несохраненные изменения будут потеряны. Хотите сохранить их и добавить новый компонент?"));
        reply.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        reply.setIcon(QMessageBox::Warning);
        reply.setButtonText(QMessageBox::Yes, QStringLiteral(u"Да"));
        reply.setButtonText(QMessageBox::No, QStringLiteral(u"Нет"));
        if (reply.exec() == QMessageBox::Yes) {
            bool acceptFlag = openNewComponentDialog(false);
            if (!acceptFlag)
                return true;
        }
        else
        {
            componentEditor->clearUgo();
            componentEditor->clearIcons();
            componentEditor->modelsComboBox->setCurrentText("");
            componentEditor->currentParameterListWidget->clearItems();
            componentEditor->newThumbName = new QString();
        }
    }
    else if (!f4)
    {
        bool f5 = !compareParametersLists(currentComponent->parameters, *componentEditor->currentParameterListWidget->parameters);
        if (currentComponent->thumbName != componentEditor->newThumbName || componentEditor->modelsComboBox->currentText() != currentComponent->ugo.model || f5)
        {
            QMessageBox reply;
            reply.setWindowTitle(QStringLiteral(u"Подтверждение"));
            reply.setText(QStringLiteral(u"Данные выбранного компонента были изменены. Несохраненные изменения будут потеряны. Хотите сохранить изменения?"));
            reply.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            reply.setIcon(QMessageBox::Warning);
            reply.setButtonText(QMessageBox::Yes, QStringLiteral(u"Да"));
            reply.setButtonText(QMessageBox::No, QStringLiteral(u"Нет"));
            if (reply.exec() == QMessageBox::Yes) {
                bool acceptFlag = openNewComponentDialog(true);
                if (!acceptFlag)
                    return true;
            }
            else
            {
                componentEditor->clearUgo();
                componentEditor->clearIcons();
                componentEditor->modelsComboBox->setCurrentText("");
                componentEditor->currentParameterListWidget->clearItems();
                componentEditor->newThumbName = new QString();
            }
        }
    }

    return false;
}

void LibWorkspace::openNewCatalogDialog()
{
    dialogCat = new NewCatalogDialog(libraries, catalogs, componentEditor->iconPaths, this);

    if (dialogCat->exec() == QDialog::Accepted) {
        QString name = dialogCat->getName();
        QString library = dialogCat->getLibrary();
        QString directory = dialogCat->getDirectory();
        QString thumb = "";
        if (dialogCat->newThumbName != NULL)
            thumb = *dialogCat->newThumbName;
        createNewCatalog(name, library, directory, thumb);
        refresh();
    }

    delete dialogCat;
}

void LibWorkspace::createNewCatalog(QString name, QString library, QString directory, QString thumb)
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

    nlohmann::ordered_json jsonObj = FillFromJsons::readJson(libPath, this);

    FillFromJsons::AddNewCatalogToJson(jsonObj, library, directory, name, mainPath, thumb);

}


void LibWorkspace::refresh()
{
    TreeState treeState;
    if (currentLibrary) {
        treeState.library = currentLibrary->name;
        if (currentCatalog)
        {
            treeState.catalog = currentCatalog->name;
            if (!currentCatalog->parent.isEmpty())
            {
                treeState.parentCatalog = currentCatalog->parent;
            }
        }
    }
    currentLibrary = nullptr;
    currentCatalog = nullptr;
    currentComponent = nullptr;
    componentEditor->clearWidget();
    componentEditor->iconPaths.clear();
    libraryManager->clearLibraries();
    parameters->clear();
    libraries->clear();
    catalogs->clear();
    components->clear();
    libraryManager->request();
    setupFields();
    componentsTable->setRowCount(0);
    if (!reset)
    {
        libraryManager->reExpand(treeState.library, treeState.parentCatalog, treeState.catalog);
    }
    reset = false;
}

void LibWorkspace::saveChanges()
{
    bool f5 = !compareParametersLists(currentComponent->parameters, *componentEditor->currentParameterListWidget->parameters);
    if (currentComponent->thumbName != componentEditor->newThumbName || componentEditor->modelsComboBox->currentText() != currentComponent->ugo.model || f5)
    {
        openNewComponentDialog(true);
    }
    else
    {
        QMessageBox::information(this, QStringLiteral(u"Сохранение"),
            QStringLiteral(u"Нет изменений, доступных для сохранения"));
    }
}
