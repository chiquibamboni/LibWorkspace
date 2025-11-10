#include "LibraryManager.h"
#include <QDebug>
#include "FillFromJsons.h"

LibraryManager::LibraryManager(QList<Library>* libraries, QList<Catalog>* catalogs, QWidget* parent)
    : QTreeView(parent)
{
    librariesList = libraries;
    catalogsList = catalogs;
    setupTree();
}

LibraryManager::~LibraryManager()
{

}

void LibraryManager::setupTree()
{
    model = new QStandardItemModel(this);
    root = model->invisibleRootItem();
    setModel(model);
    setHeaderHidden(true);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    installEventFilter(this);
}

//Обновление QTreeView после получения данных с сервера
void LibraryManager::updateTree(const nlohmann::json& jsonData)
{
    if (currentPath == "./Libraries")
    {
        if (!firstRequest)
        {
            root->removeRows(0, root->rowCount());
        }
        if (jsonData.contains("libraries") && jsonData["libraries"].is_array())
        {
            QList<Library> libraryes = FillFromJsons::addRootJsonToModel(jsonData);
            for (Library library : libraryes)
            {
                root->appendRow(library.item);
                librariesList->append(library);
            }
        } else {
            if (!jsonData.contains("libraries")) {
                QString message = QStringLiteral(u"JSON не содержит поле 'libraries'");
                FillFromJsons::showError(this, message);
            } else if (!jsonData["libraries"].is_array()) {
                QString message = QStringLiteral(u"Поле 'libraries' в JSON не является массивом");
                FillFromJsons::showError(this, message);
            }
        }
        firstRequest = false;
        return;
    }
    if (currentLibrary->components_location.isEmpty())
    {
        addLibraryToModel(jsonData, currentLibrary->item);
    }
}

void LibraryManager::addLibraryToModel(const nlohmann::json& jsonObj, QStandardItem* parentItem)
{
    parentItem->removeRows(0, parentItem->rowCount());
    if (jsonObj.contains("components_location"))
    {
        currentLibrary->components_location = QString::fromStdString(jsonObj["components_location"].get<std::string>());
        currentLibrary->layouts_location = QString::fromStdString(jsonObj["layouts_location"].get<std::string>());
        currentLibrary->sparam_location = QString::fromStdString(jsonObj["sparam_location"].get<std::string>());
        currentLibrary->symbols_location = QString::fromStdString(jsonObj["symbols_location"].get<std::string>());
        currentLibrary->thumbnails_location = QString::fromStdString(jsonObj["thumbnails_location"].get<std::string>());
        currentLibrary->ugos_location = QString::fromStdString(jsonObj["ugos_location"].get<std::string>());
        currentLibrary->veriloga_location = QString::fromStdString(jsonObj["veriloga_location"].get<std::string>());
        if (jsonObj.contains("catalogs") && jsonObj["catalogs"].is_array()) {
            for (auto& catalogJson : jsonObj["catalogs"]) {
                Catalog catalog = FillFromJsons::CatalogFromJson(catalogJson, currentLibrary, catalogsList);
                catalogsList->push_back(catalog);
                if (parentItem) {
                    parentItem->appendRow(catalog.item);
                }
            }
        } else {
            if (jsonObj.contains("catalogs") && !jsonObj["catalogs"].is_array()) {
                QString message = QStringLiteral(u"Поле 'catalogs' в JSON не является массивом");
                FillFromJsons::showError(this, message);
            }
        }
        currentLibrary->catalogs = *catalogsList;
    } else {
        QString message = QStringLiteral(u"JSON не содержит поле 'components_location'");
        FillFromJsons::showError(this, message);
    }
}

void LibraryManager::request()
{
    nlohmann::json jsonData = FillFromJsons::readJson(currentPath, this);
    if (jsonData.is_null()) {
        QString message = QString(QStringLiteral(u"Не удалось загрузить данные из: %1")).arg(currentPath);
        FillFromJsons::showError(this, message);
    }
    updateTree(jsonData);
}

void LibraryManager::clearLibraries()
{
    model->clear();
    currentPath = "./Libraries";
    firstRequest = true;
    root = model->invisibleRootItem();
    librariesList->clear();
    catalogsList->clear();
}

void LibraryManager::reExpand(QString node1, QString node2, QString node3)
{
    if (!model) return;

    collapseAll();

    QStandardItem* level1Item = findItemByText(root, node1);
    if (!level1Item) return;

    setExpanded(level1Item->index(), true);

    if (!node2.isEmpty()) {
        QStandardItem* level2Item = findItemByText(level1Item, node2);
        if (level2Item) {
            setExpanded(level2Item->index(), true);

            if (!node3.isEmpty()) {
                QStandardItem* level3Item = findItemByText(level2Item, node3);
                if (level3Item) {
                    setExpanded(level3Item->index(), true);
                    scrollTo(level3Item->index(), QAbstractItemView::PositionAtCenter);
                    setCurrentIndex(level3Item->index());
                }
            }
            else {
                scrollTo(level2Item->index(), QAbstractItemView::PositionAtCenter);
                setCurrentIndex(level2Item->index());
            }
        }
    }
    else {
        scrollTo(level1Item->index(), QAbstractItemView::PositionAtCenter);
        setCurrentIndex(level1Item->index());
    }
}

QStandardItem* LibraryManager::findItemByText(QStandardItem* parent, const QString& text)
{
    if (!parent) return nullptr;

    for (int row = 0; row < parent->rowCount(); ++row) {
        QStandardItem* child = parent->child(row);
        if (child && child->text() == text) {
            return child;
        }
    }
    return nullptr;
}
