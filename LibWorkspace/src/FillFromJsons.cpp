#include "FillFromJsons.h"


nlohmann::json FillFromJsons::readJson(QString currentPath, QWidget* parent) 
{
    QFileInfo fileInfo(currentPath);

    if (!fileInfo.exists()) {
        QMessageBox::warning(parent, QStringLiteral(u"Ошибка"), QStringLiteral(u"ресурсы не найдены"));
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
    addComponentRest(component.model, component, currentLibrary);

    QIcon icon;
    if (jsonObj["thumb"] != "")
    {
        QString iconPath = "./Libraries/" + currentLibrary->dir + "/" + 
            currentLibrary->thumbnails_location + "/" +
            QString::fromStdString(jsonObj["thumb"].get<std::string>()) + ".svg";
        if (QFile::exists(iconPath))
        {
            icon = QIcon(iconPath);
            component.thumb = icon;
        }
    }
}

void FillFromJsons::addParametrFromJson(nlohmann::json jsonObj, Parameters& parametr)
{
    parametr.name = QString::fromStdString(jsonObj["name"].get<std::string>());
    parametr.type = QString::fromStdString(jsonObj["type"].get<std::string>());
    parametr.display = jsonObj["display"].get<bool>();

    if (parametr.type == "String" || (jsonObj.contains("default") && jsonObj["default"].is_string()) || parametr.type == "Equation") {
        if (jsonObj.contains("default")) {
            parametr.pdefault = QString::fromStdString(jsonObj["default"].get<std::string>());
        }
        else {
            parametr.pdefault = QString();
        }
    }
    else if (jsonObj.contains("default") && jsonObj["default"].is_array()) {
        //Для массива
        std::vector<double> arr = jsonObj["default"].get<std::vector<double>>();
        QVariantList qlist;
        for (double v : arr) {
            qlist << v;
        }
        parametr.pdefault = qlist;
    }
    else if (jsonObj.contains("default") && jsonObj["default"].is_number()) {
        parametr.pdefault = jsonObj["default"].get<double>();
    }
    else {
        parametr.pdefault = std::nullopt;
    }

    if (jsonObj.contains("ref")) {
        parametr.ref = QString::fromStdString(jsonObj["ref"].get<std::string>());
    }
    if (jsonObj.contains("factor")) {
        parametr.factor = QString::fromStdString(jsonObj["factor"].get<std::string>());
    }
    if (jsonObj.contains("feature")) {
        parametr.feature = QString::fromStdString(jsonObj["feature"].get<std::string>());
    }
    if (jsonObj.contains("unit")) {
        parametr.unit = QString::fromStdString(jsonObj["unit"].get<std::string>());
    }
    if (jsonObj.contains("desc")) {
        parametr.desc = QString::fromStdString(jsonObj["desc"].get<std::string>());
    }
    if (jsonObj.contains("optimizable")) {
        parametr.optimizable = jsonObj["optimizable"].get<bool>();
    }
    if (jsonObj.contains("edited")) {
        parametr.edited = jsonObj["edited"].get<bool>();
    }
    if (jsonObj.contains("netlisted")) {
        parametr.netlisted = jsonObj["netlisted"].get<bool>();
    }
}

void FillFromJsons::addComponentRest(QString& componentModel, Component& component, Library* currentLibrary)
{
    Parameters parameter;
    QList<Parameters> paramList;

    QString location = "./Libraries/" + currentLibrary->dir + "/" +
        currentLibrary->components_location + "/" + componentModel + ".json";

    QFile jsonFile(location);

    if (jsonFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QByteArray jsonContent = jsonFile.readAll();
        nlohmann::json componentJson = nlohmann::json::parse(jsonContent);
        jsonFile.close();
        component.name = QString::fromStdString(componentJson["name"].get<std::string>());
        component.library = QString::fromStdString(componentJson["library"].get<std::string>());
        component.group = QString::fromStdString(componentJson["group"].get<std::string>());

        if (componentJson.contains("view_model")) {
            component.view_model = componentJson["view_model"].get<bool>();
        }

        for (auto& par : componentJson["parameters"])
        {
            addParametrFromJson(par, parameter);
            paramList.push_back(parameter);
        }
        for (auto& pin : componentJson["pins"]) {
            component.pins.push_back(QString::fromStdString(pin.get<std::string>()));
        }

        if (componentJson.contains("schematic") && componentJson["schematic"].contains("netlist")) {
            auto& netlistJson = componentJson["schematic"]["netlist"];

            if (netlistJson.contains("model")) {
                component.schematic.netlist.model = QString::fromStdString(netlistJson["model"].get<std::string>());
            }

            if (netlistJson.contains("params")) {
                auto& paramsJson = netlistJson["params"];
                for (auto it = paramsJson.begin(); it != paramsJson.end(); ++it) {
                    QString key = QString::fromStdString(it.key());
                    QString value = QString::fromStdString(it.value().get<std::string>());
                    component.schematic.netlist.params.insert(key, value);
                }
            }
        }
        if (componentJson.contains("layout") && componentJson["layout"].contains("model")) {
            component.layout.model = QString::fromStdString(componentJson["layout"]["model"].get<std::string>());
        }

        if (componentJson.contains("ugo") && componentJson["ugo"].contains("model")) {
            component.ugo.model = QString::fromStdString(componentJson["ugo"]["model"].get<std::string>());
        }

        if (componentJson.contains("veriloga") && componentJson["veriloga"].contains("model")) {
            component.veriloga.model = QString::fromStdString(componentJson["veriloga"]["model"].get<std::string>());
        }
    }
    component.parameters = paramList;
}

void FillFromJsons::AddNewComponentToJson(nlohmann::json jsonObj, Component& component)
{
}

void FillFromJsons::MoveComponentUp(nlohmann::json jsonObj, Component& component)
{
}

void FillFromJsons::MoveComponentDown(nlohmann::json jsonObj, Component& component)
{
}
