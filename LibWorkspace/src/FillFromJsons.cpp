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
    if (jsonObj.contains("thumb") && jsonObj["thumb"].is_string() && !jsonObj["thumb"].get<std::string>().empty())
    {
        component.thumbName = QString::fromStdString(jsonObj["thumb"].get<std::string>());
        QString iconPath = QString("./Libraries/") +
            currentLibrary->dir + "/" +
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
            QIcon icon;
            QString ansiIconPath = QString("./Libraries/") +
                currentLibrary->dir + "/" +
                currentLibrary->symbols_location + "/ansi/" +
                component.ugo.model + ".svg";
            if (QFile::exists(ansiIconPath))
            {
                icon = QIcon(ansiIconPath);
                component.ugo.ansiUgoSymbol = icon;
            }
            QString gostIconPath = QString("./Libraries/") +
                currentLibrary->dir + "/" +
                currentLibrary->symbols_location + "/gost/" +
                component.ugo.model + ".svg";
            if (QFile::exists(gostIconPath))
            {
                icon = QIcon(gostIconPath);
                component.ugo.gostUgoSymbol = icon;
            }
        }

        if (componentJson.contains("veriloga") && componentJson["veriloga"].contains("model")) {
            component.veriloga.model = QString::fromStdString(componentJson["veriloga"]["model"].get<std::string>());
        }
    }
    component.parameters = paramList;
}

void FillFromJsons::AddNewComponentToJson(nlohmann::json& jsonObj, Component& component, QString catalogName, QString componentsPath)
{
    for (auto& catalog : jsonObj["catalogs"])
    {
        if (catalog.contains("name") && catalog["name"] == catalogName.toStdString())
        {
            nlohmann::json newComponent = {
                {"model", component.model.toStdString()},
                {"thumb", component.thumbName.toStdString()},
                {"desc", component.desc.toStdString()}
            };

            if (!catalog.contains("components") || !catalog["components"].is_array())
            {
                catalog["components"] = nlohmann::json::array();
            }
            catalog["components"].push_back(newComponent);
            // создание нового json
            nlohmann::json fullComponentJson = CreateComponentJson(component);
            QString filePath = componentsPath + "/" + component.name + ".json";
            saveJsonToFile(fullComponentJson, filePath);
            return;
        }
    }

}

// Функция для преобразования QVariant в json
nlohmann::json FillFromJsons::QVariantToJson(const QVariant& var)
{
    switch (var.type())
    {
    case QVariant::Int:
        return var.toInt();
    case QVariant::Double:
        return var.toDouble();
    case QVariant::String:
        return var.toString().toStdString();
    case QVariant::Bool:
        return var.toBool();
    default:
        return var.toString().toStdString();
    }
}

nlohmann::json FillFromJsons::ParametersToJson(QList<Parameters>& params)
{
    nlohmann::json arr = nlohmann::json::array();
    for (const auto& param : params)
    {
        nlohmann::json jParam;
        jParam["ref"] = param.ref.toStdString();
        jParam["name"] = param.name.toStdString();
        jParam["type"] = param.type.toStdString();

        if (param.pdefault.has_value())
        {
            jParam["default"] = QVariantToJson(*param.pdefault);
        }

        if (!param.factor.isEmpty())
            jParam["factor"] = param.factor.toStdString();
        if (!param.feature.isEmpty())
            jParam["feature"] = param.feature.toStdString();
        if (!param.unit.isEmpty())
            jParam["unit"] = param.unit.toStdString();
        if (!param.desc.isEmpty())
            jParam["desc"] = param.desc.toStdString();

        jParam["display"] = param.display;

        if (param.optimizable.has_value())
            jParam["optimizable"] = *param.optimizable;

        if (param.edited.has_value())
            jParam["edited"] = *param.edited;

        if (param.netlisted.has_value())
            jParam["netlisted"] = *param.netlisted;

        arr.push_back(jParam);
    }
    return arr;
}

nlohmann::json FillFromJsons::CreateComponentJson(Component& comp)
{
    nlohmann::json paramsJson = ParametersToJson(comp.parameters);

    nlohmann::json j = {
        {"library", comp.library.toStdString()},
        {"name", comp.name.toStdString()},
        {"model", comp.model.toStdString()},
        {"group", comp.group.toStdString()},
        {"desc", comp.desc.toStdString()},
        {"parameters", paramsJson}
    };

    // Обработка pins как QList<QString>
    nlohmann::json pinsJson = nlohmann::json::array();
    for (const auto& pin : comp.pins) {
        pinsJson.push_back(pin.toStdString());
    }
    j["pins"] = pinsJson;

    j["thumbName"] = comp.thumbName.toStdString();

    // schematics
    j["schematic"] = {
        {"netlist", {
            {"model", comp.schematic.netlist.model.toStdString()},
            {"params", nlohmann::json::object()}
        }}
    };

    // layout
    j["layout"] = { {"model", comp.layout.model.toStdString()} };
    // ugo
    j["ugo"] = { {"model", comp.ugo.model.toStdString()} };
    // veriloga
    j["veriloga"] = { {"model", comp.veriloga.model.toStdString()} };

    // netlist params
    for (auto it = comp.schematic.netlist.params.cbegin(); it != comp.schematic.netlist.params.cend(); ++it)
    {
        j["schematic"]["netlist"]["params"][it.key().toStdString()] = it.value().toStdString();
    }

    return j;
}

void FillFromJsons::saveJsonToFile(const nlohmann::json& j, const QString& filePath) 
{
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QString jsonString = QString::fromStdString(j.dump(4)); 
        QTextStream out(&file);
        out << jsonString;
        file.close();
    }
}

void FillFromJsons::MoveComponentUp(nlohmann::json jsonObj, Component& component)
{

}

void FillFromJsons::MoveComponentDown(nlohmann::json jsonObj, Component& component)
{
}
