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

    parametersList = new ParametersList();

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
    mainLayout->addWidget(parametersList);
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
    connect(parametersList, &QListWidget::itemDoubleClicked, this, &LibWorkspace::onItemDoubleClicked);

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
            parametersList->location = "./Libraries/" + libraryManager->currentLibrary->dir + "/" + libraryManager->currentLibrary->components_location;
            parametersList->setItems();
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

Parameters LibWorkspace::parseTooltip(const QString& tooltip)
{
    Parameters params;

    if (tooltip.isEmpty()) {
        return params;
    }

    QStringList lines = tooltip.split('\n', Qt::SkipEmptyParts);

    QRegularExpression re("(\\w+):\\s*(.+)");

    for (const QString& line : lines) {
        QRegularExpressionMatch match = re.match(line);
        if (match.hasMatch()) {
            QString key = match.captured(1).trimmed();
            QString value = match.captured(2).trimmed();

            if (key == "ref") {
                params.ref = value;
            }
            else if (key == "name") {
                params.name = value;
            }
            else if (key == "type") {
                params.type = value;
            }
            else if (key == "default") {
                bool ok;
                double doubleValue = value.toDouble(&ok);
                if (ok) {
                    params.rdefault = doubleValue;
                }
                else {
                    params.sdefault = value;
                }
            }
            else if (key == "factor") {
                params.factor = value;
            }
            else if (key == "feature") {
                params.feature = value;
            }
            else if (key == "unit") {
                params.unit = value;
            }
            else if (key == "desc") {
                params.desc = value;
            }
            else if (key == "display") {
                params.display = (value == "1");
            }
            else if (key == "optimizable") {
                params.optimizable = (value == "1");
            }
            else if (key == "edited") {
                params.edited = (value == "1");
            }
            else if (key == "netlisted") {
                params.netlisted = (value == "1");
            }

        }
    }
    return params;

}
void LibWorkspace::onItemDoubleClicked(QListWidgetItem* item)
{
    Parameters сurrentParam = parseTooltip(item->toolTip());
    componentEditor->nameEdit->setText(сurrentParam.name);
    componentEditor->typeComboBox->setCurrentText(сurrentParam.type);
    if (сurrentParam.rdefault.has_value())
    {
        componentEditor->defaultValueLineEdit->setText(QString::number(сurrentParam.rdefault.value()));
    }
    else
    {
        componentEditor->defaultValueLineEdit->setText(сurrentParam.sdefault);
    }
    componentEditor->featureComboBox->setCurrentText(сurrentParam.feature);
    componentEditor->unitComboBox->setCurrentText(сurrentParam.unit);
    componentEditor->descLineEdit->setText(сurrentParam.desc);
    componentEditor->displayCheckBox->setChecked(сurrentParam.display);
    componentEditor->optimizableCheckBox->setChecked(сurrentParam.optimizable.value_or(false));
    componentEditor->editedCheckBox->setChecked(сurrentParam.edited.value_or(false));
    componentEditor->netlistedCheckBox->setChecked(сurrentParam.netlisted.value_or(false));
}

//void LibWorkspace::refreshButtonClicked()
//{
//    libraryManager->request();
//    componentsTable->setRowCount(0);
//}

