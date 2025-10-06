#include "LibraryManager.h"

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
                if (parentItem) {
                    parentItem->appendRow(catalog.item);
                }
            }
        }
    }
}

void LibraryManager::request()
{
    updateTree(FillFromJsons::readJson(currentPath, this));
}
