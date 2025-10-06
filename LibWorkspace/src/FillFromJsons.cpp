#include "FillFromJsons.h"


nlohmann::json FillFromJsons::readJson(QString currentPath, QWidget* parent) 
{
    QFileInfo fileInfo(currentPath);

    if (!fileInfo.exists()) {
        QMessageBox::warning(parent, "Ошибка", QStringLiteral(u"ресурсы не найдены"));
        return nlohmann::json();
    }

    if (fileInfo.isDir()) {
        QDir librariesDir(currentPath);
        QStringList jsonFiles = librariesDir.entryList(QStringList() << "*.json", QDir::Files);

        if (!jsonFiles.isEmpty()) {
            QString jsonFilePath = librariesDir.filePath(jsonFiles.first());
            QFile jsonFile(jsonFilePath);

            if (jsonFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
                QByteArray jsonContent = jsonFile.readAll();
                nlohmann::json j = nlohmann::json::parse(jsonContent);
                jsonFile.close();
                return j;
            }
            else {
                QMessageBox::warning(parent, QStringLiteral(u"Ошибка при открытии json файла: "), jsonFilePath);
            }
        }
        else {
            QMessageBox::warning(parent, "Ошибка", QStringLiteral(u"json файл не найден"));
        }
    }
    else
    {
        QFile jsonFile(currentPath);

        if (jsonFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QByteArray jsonContent = jsonFile.readAll();
            nlohmann::json j = nlohmann::json::parse(jsonContent);
            jsonFile.close();
            return j;
        }
        else {
            QMessageBox::warning(parent, QStringLiteral(u"Ошибка при открытии json файла: "), fileInfo.fileName());
        }
    }

    return nlohmann::json();
}

QList<Library> FillFromJsons::addRootJsonToModel(const nlohmann::json& jsonObj)
{
    QIcon folderIcon = QIcon("./icons/folder.svg");
    QList<Library> libraryesList;
    Library library;
    for (const auto& lib : jsonObj["libraries"]) {

        library.name = QString::fromStdString(lib["name"].get<std::string>());
        library.dir = QString::fromStdString(lib["dir"].get<std::string>());
        library.ver = lib["ver"].get<double>();

        library.item = new QStandardItem(library.name);
        library.item->setIcon(folderIcon);

        QStandardItem* dummyChild = new QStandardItem(" ");
        library.item->appendRow(dummyChild);
        libraryesList.append(library);
     }
    return libraryesList;
}

Catalog FillFromJsons::CatalogFromJson(const nlohmann::json& jsonObj, Library* currentLibrary, QList<Catalog>* catalogsList)
{
    QIcon folderIcon = QIcon("./icons/folder.svg");
    Catalog catalog;
    catalog.name = QString::fromStdString(jsonObj["name"].get<std::string>());
    catalog.thumb = QString::fromStdString(jsonObj["thumb"].get<std::string>());

    catalog.item = new QStandardItem(catalog.name);
    QIcon icon;
    if (catalog.thumb != "")
    {
        QString iconPath = "./Libraries/" + currentLibrary->dir + "/" 
            + currentLibrary->thumbnails_location + "/" + catalog.thumb + ".svg";
        if (QFile::exists(iconPath))
        {
            icon = QIcon(iconPath);
        }
        else
        {
            icon = folderIcon;
        }
    }
    else
    {
        icon = folderIcon;
    }
    catalog.item->setIcon(icon);

    //Обработка вложенных каталогов
    if (jsonObj.contains("catalogs") && jsonObj["catalogs"].is_array()) {
        for (const auto& subCatalogJson : jsonObj["catalogs"]) {
            Catalog subCat = CatalogFromJson(subCatalogJson, currentLibrary, catalogsList);
            if (catalog.item) {
                catalog.item->appendRow(subCat.item);
            }
            catalog.catalogs.push_back(subCat);
        }
    }

    //Обработка компонентов (если есть)
    if (jsonObj.contains("components") && jsonObj["components"].is_array()) {
        for (const auto& compJson : jsonObj["components"]) {
            Component comp;
            ComponentFromJson(compJson, comp, currentLibrary);
            catalog.components.push_back(comp);
        }
    }

    catalogsList->push_back(catalog);

    return catalog;
}

void FillFromJsons::ComponentFromJson(const nlohmann::json& jsonObj, Component& component, Library* currentLibrary)
{
    component.model = QString::fromStdString(jsonObj["model"].get<std::string>());
    component.desc = QString::fromStdString(jsonObj["desc"].get<std::string>());

    QIcon icon;
    if (jsonObj["thumb"] != "")
    {
        QString iconPath = "./Libraries/" + currentLibrary->dir + "/" + currentLibrary->thumbnails_location + "/" + QString::fromStdString(jsonObj["thumb"].get<std::string>()) + ".svg";
        if (QFile::exists(iconPath))
        {
            icon = QIcon(iconPath);
            component.thumb = icon;
        }
    }
}