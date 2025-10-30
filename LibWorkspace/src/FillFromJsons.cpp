#include "FillFromJsons.h"
#include <algorithm> 
#include <QDebug>

// Инициализация статической переменной
QStatusBar* FillFromJsons::statusBar = nullptr;

// Метод для установки статус бара
void FillFromJsons::setStatusBar(QStatusBar* sb) {
    statusBar = sb;
}

// Метод для получения статус бара
QStatusBar* FillFromJsons::getStatusBar() {
    return statusBar;
}

// Функция для централизованного вывода ошибок
void FillFromJsons::showError(QWidget* parent, const QString& message) {
    // Вывод в терминал
    qDebug() << message;
    
    // Вывод в статус бар
    if (statusBar) {
        statusBar->showMessage(message, 5000);
    }
    
    // Вывод во всплывающем окне
    QMessageBox::warning(parent, QStringLiteral(u"Ошибка"), message);
}

nlohmann::json FillFromJsons::readJson(QString currentPath, QWidget* parent) 
{
    QFileInfo fileInfo(currentPath);

    if (!fileInfo.exists()) {
        QString message = QStringLiteral(u"Ресурсы не найдены: ") + currentPath;
        showError(parent, message);
        return nlohmann::json();
    }

    if (fileInfo.isDir()) {
        QDir librariesDir(currentPath);
        QStringList jsonFiles = librariesDir.entryList(QStringList() << "*.json", QDir::Files);

        if (!jsonFiles.isEmpty()) {
            QString jsonFilePath = librariesDir.filePath(jsonFiles.first());
            QFile jsonFile(jsonFilePath);

            // Проверяем, существует ли файл перед попыткой открытия
            if (!QFileInfo::exists(jsonFilePath)) {
                QString message = QStringLiteral(u"JSON файл не найден: ") + jsonFilePath;
                showError(parent, message);
                return nlohmann::json();
            }

            if (jsonFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
                QByteArray jsonContent = jsonFile.readAll();
                try {
                    nlohmann::json j = nlohmann::json::parse(jsonContent);
                    jsonFile.close();
                    return j;
                }
                catch (const nlohmann::json::parse_error& e) {
                    QString message = QString(QStringLiteral(u"Ошибка при парсинге файла %1: %2")).arg(jsonFilePath).arg(e.what());
                    showError(parent, message);
                    jsonFile.close();
                    return nlohmann::json();
                }
            }
            else {
                QString message = QStringLiteral(u"Ошибка при открытии json файла: ") + jsonFilePath;
                showError(parent, message);
            }
        }
        else {
            QString message = QStringLiteral(u"JSON файл не найден в директории: ") + currentPath;
            showError(parent, message);
        }
    }
    else
    {
        QFile jsonFile(currentPath);

        // Проверяем, существует ли файл перед попыткой открытия
        if (!QFileInfo::exists(currentPath)) {
            QString message = QStringLiteral(u"JSON файл не найден: ") + currentPath;
            showError(parent, message);
            return nlohmann::json();
        }

        if (jsonFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QByteArray jsonContent = jsonFile.readAll();
            try {
                nlohmann::json j = nlohmann::json::parse(jsonContent);
                jsonFile.close();
                return j;
            }
            catch (const nlohmann::json::parse_error& e) {
                QString message = QString(QStringLiteral(u"Ошибка при парсинге файла %1: %2")).arg(fileInfo.fileName()).arg(e.what());
                showError(parent, message);
                jsonFile.close();
                return nlohmann::json();
            }
        }
        else {
            QString message = QStringLiteral(u"Ошибка при открытии json файла: ") + fileInfo.fileName();
            showError(parent, message);
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

Catalog FillFromJsons::CatalogFromJson(const nlohmann::json& jsonObj, Library* currentLibrary, QList<Catalog>* catalogsList, Catalog* parent)
{
    QIcon folderIcon = QIcon("./icons/folder.svg");
    Catalog catalog;
    if (parent != nullptr)
    {
        catalog.parent = parent->name;
    }

    if (jsonObj.contains("name")) {
        catalog.name = QString::fromStdString(jsonObj["name"].get<std::string>());
    }
    else {
        catalog.name = QStringLiteral(u"Без названия");
        QString message = QStringLiteral(u"Каталог без названия");
        qDebug() << message;
    }

    if (jsonObj.contains("thumb")) {
        catalog.thumb = QString::fromStdString(jsonObj["thumb"].get<std::string>());
    }

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
            QString message = QString(QStringLiteral(u"Иконка не найдена: %1")).arg(iconPath);
            qDebug() << message;
        }
    }
    else
    {
        icon = folderIcon;
    }
    catalog.item->setIcon(icon);

    // Обработка вложенных каталогов
    if (jsonObj.contains("catalogs") && jsonObj["catalogs"].is_array()) {
        for (const auto& subCatalogJson : jsonObj["catalogs"]) {
            Catalog subCat = CatalogFromJson(subCatalogJson, currentLibrary, catalogsList, &catalog);
            if (catalog.item) {
                catalog.item->appendRow(subCat.item);
            }
            catalog.catalogs.push_back(subCat);
        }
    }
    else {
        if (jsonObj.contains("catalogs") && !jsonObj["catalogs"].is_array()) {
            QString message = QStringLiteral(u"Поле 'catalogs' не является массивом");
            qDebug() << message;
        }
    }

    // Обработка компонентов
    if (jsonObj.contains("components") && jsonObj["components"].is_array()) {
        for (const auto& compJson : jsonObj["components"]) {
            Component comp;
            ComponentFromJson(compJson, comp, currentLibrary);
            catalog.components.push_back(comp);
        }
    }
    else {
        if (jsonObj.contains("components") && !jsonObj["components"].is_array()) {
            QString message = QStringLiteral(u"Поле 'components' не является массивом");
            qDebug() << message;
        }
    }

    return catalog;
}

void FillFromJsons::ComponentFromJson(const nlohmann::json& jsonObj, Component& component, Library* currentLibrary)
{
    if (jsonObj.contains("model")) {
        component.model = QString::fromStdString(jsonObj["model"].get<std::string>());
    } else {
        component.model = QStringLiteral(u"Без названия");
        QString message = QStringLiteral(u"Компонент без названия");
        qDebug() << message;
    }
    
    if (jsonObj.contains("desc")) {
        component.desc = QString::fromStdString(jsonObj["desc"].get<std::string>());
    }
    
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
        } else {
            QString message = QString(QStringLiteral(u"Иконка компонента не найдена: %1")).arg(iconPath);
            qDebug() << message;
        }
    }
}

void FillFromJsons::addParametrFromJson(nlohmann::json jsonObj, Parameters& parametr)
{
    if (jsonObj.contains("name")) {
        parametr.name = QString::fromStdString(jsonObj["name"].get<std::string>());
    } else {
        qDebug() << QStringLiteral(u"Параметр без названия");
    }
    
    if (jsonObj.contains("type")) {
        parametr.type = QString::fromStdString(jsonObj["type"].get<std::string>());
    }
    
    if (jsonObj.contains("display")) {
        parametr.display = jsonObj["display"].get<bool>();
    } else {
        parametr.display = true; // значение по умолчанию
    }

    if (parametr.type == "String" || (jsonObj.contains("default") && jsonObj["default"].is_string()) || parametr.type == "Equation") {
        if (jsonObj.contains("default")) {
            try {
                // Пытаемся получить как строку
                parametr.pdefault = QString::fromStdString(jsonObj["default"].get<std::string>());
            }
            catch (const nlohmann::json::type_error&) {
                // Если не строка - преобразуем в строковое представление
                parametr.pdefault = QString::fromStdString(jsonObj["default"].dump());

                
            }
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

    // Проверяем, существует ли файл перед попыткой открытия
    QFileInfo fileInfo(location);
    if (!fileInfo.exists()) {
        QString message = QString(QStringLiteral(u"Файл компонента не найден: %1")).arg(location);
        showError(nullptr, message);
        return;
    }

    QFile jsonFile(location);

    if (jsonFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QByteArray jsonContent = jsonFile.readAll();
        try {
            nlohmann::json componentJson = nlohmann::json::parse(jsonContent);
            jsonFile.close();
            
            if (componentJson.contains("name")) {
                component.name = QString::fromStdString(componentJson["name"].get<std::string>());
            }
            if (componentJson.contains("library")) {
                component.library = QString::fromStdString(componentJson["library"].get<std::string>());
            }
            if (componentJson.contains("group")) {
                component.group = QString::fromStdString(componentJson["group"].get<std::string>());
            }

            if (componentJson.contains("view_model")) {
                component.view_model = componentJson["view_model"].get<bool>();
            }

            if (componentJson.contains("parameters") && componentJson["parameters"].is_array()) {
                for (auto& par : componentJson["parameters"])
                {
                    addParametrFromJson(par, parameter);
                    paramList.push_back(parameter);
                }
            }
            
            if (componentJson.contains("pins") && componentJson["pins"].is_array()) {
                for (auto& pin : componentJson["pins"]) {
                    component.pins.push_back(QString::fromStdString(pin.get<std::string>()));
                }
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
        catch (const nlohmann::json::parse_error& e) {
            QString message = QString(QStringLiteral(u"Ошибка при парсинге файла компонента %1: %2")).arg(location).arg(e.what());
            showError(nullptr, message);
            jsonFile.close();
            return;
        }
    } else {
        QString message = QString(QStringLiteral(u"Не удалось открыть файл компонента: %1")).arg(location);
        showError(nullptr, message);
        return;
    }
    component.parameters = paramList;
}

void FillFromJsons::AddNewComponentToJson(nlohmann::json& jsonObj, QString mainPath, Component& component, QString catalogName,
    QString thumbFileName, QString ugoFileName)
{
    QString mainJsonFilePath = mainPath + "/library.json";
    QString compPath = mainPath + "/components";
    QString ugoPath = mainPath + "/ugos/ansi";

    std::function<bool(nlohmann::json&, const std::string&)> findAndAddToCatalog;

    findAndAddToCatalog = [&](nlohmann::json& catalogObj, const std::string& targetCatalogName) -> bool {

        if (catalogObj.contains("name") && catalogObj["name"] == targetCatalogName)
        {
            nlohmann::json newComponent = {
                {"model", component.model.toStdString()},
                {"thumb", component.thumbName.toStdString()},
                {"desc", component.desc.toStdString()}
            };

            if (!catalogObj.contains("components") || !catalogObj["components"].is_array())
            {
                catalogObj["components"] = nlohmann::json::array();
            }
            catalogObj["components"].push_back(newComponent);

            ///добавление иконки для thumb 
            QDir componentsDir(mainPath);
            QString thumbnailsPath = componentsDir.filePath("thumbnails");
            QString sourceFilePath = thumbnailsPath + "/" + thumbFileName;
            if (!sourceFilePath.endsWith(".svg", Qt::CaseInsensitive)) {
                sourceFilePath += ".svg";
            }
            QString targetFilePath = thumbnailsPath + "/" + component.model + ".svg";

            QFile sourceFile(sourceFilePath);
            QFile targetFile(targetFilePath);

            if (sourceFile.exists()) {
                if (targetFile.exists()) {
                    targetFile.remove();
                }
                sourceFile.copy(targetFilePath);
            }

            QString symbolsPath = componentsDir.filePath("symbols/ansi");
            QString sourceUgoFilePath = symbolsPath + "/" + ugoFileName;
            if (!sourceUgoFilePath.endsWith(".svg", Qt::CaseInsensitive)) {
                sourceUgoFilePath += ".svg";
            }

            QString targetUgoFilePath = symbolsPath + "/" + component.model + ".svg";

            QFile sourceUgoFile(sourceUgoFilePath);
            QFile targetUgoFile(targetUgoFilePath);

            if (sourceUgoFile.exists()) {
                if (targetUgoFile.exists()) {
                    targetUgoFile.remove();
                }
                sourceUgoFile.copy(targetUgoFilePath);
            }

            nlohmann::json fullComponentJson = CreateComponentJson(component);
            QString filePath = compPath + "/" + component.model + ".json";
            saveJsonToFile(fullComponentJson, filePath);

            nlohmann::json fullUgoJson = CreateUgoJson(component);
            QString fileUgoPath = ugoPath + "/" + component.model + ".json";
            saveJsonToFile(fullUgoJson, fileUgoPath);

            saveJsonToFile(jsonObj, mainJsonFilePath);
            return true; 
        }

        if (catalogObj.contains("catalogs") && catalogObj["catalogs"].is_array())
        {
            for (auto& subCatalog : catalogObj["catalogs"])
            {
                if (findAndAddToCatalog(subCatalog, targetCatalogName))
                {
                    return true;
                }
            }
        }

        return false; 
     };

    for (auto& catalog : jsonObj["catalogs"])
    {
        if (findAndAddToCatalog(catalog, catalogName.toStdString()))
        {
            return; 
        }
    }
}

void FillFromJsons::AddNewCatalogToJson(nlohmann::json& jsonObj, QString libName, QString catalogName, QString NewCatalogName,
    QString mainPath, QString thumbFileName)
{
    QString mainJsonFilePath = mainPath + "/library.json";

    nlohmann::json newCatalog = {
        {"name", NewCatalogName.toStdString()},
        {"thumb", thumbFileName.toStdString()},
        {"catalogs", nlohmann::json::array()},
        {"components", nlohmann::json::array()}
    };

    if (catalogName.isEmpty())
    {
        if (!jsonObj.contains("catalogs") || !jsonObj["catalogs"].is_array())
        {
            jsonObj["catalogs"] = nlohmann::json::array();
        }
        jsonObj["catalogs"].push_back(newCatalog);
        saveJsonToFile(jsonObj, mainJsonFilePath);
        return;
    }

    std::string targetCatalogName = catalogName.toStdString();

    for (auto& catalog : jsonObj["catalogs"])
    {
        if (catalog.contains("name") && catalog["name"] == targetCatalogName)
        {
            if (!catalog.contains("catalogs") || !catalog["catalogs"].is_array())
            {
                catalog["catalogs"] = nlohmann::json::array();
            }
            catalog["catalogs"].push_back(newCatalog);
            saveJsonToFile(jsonObj, mainJsonFilePath);
            return;
        }
    }

    // Если каталог не найден, добавляем на верхний уровень
    if (!jsonObj.contains("catalogs") || !jsonObj["catalogs"].is_array())
    {
        QString message = QString(QStringLiteral(u"Не сужествует такого каталога"));
        showError(nullptr, message);
        return;
    }
    jsonObj["catalogs"].push_back(newCatalog);
    saveJsonToFile(jsonObj, mainJsonFilePath);
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

nlohmann::json FillFromJsons::CreateUgoJson(Component& comp)
{
    nlohmann::json j = {
        {"file", comp.model.toStdString()+ ".svg"},
        {"size", nlohmann::json::array()}
    };

    nlohmann::json pinsJson = nlohmann::json::array();
    for (const auto& pin : comp.pins) {
        pinsJson.push_back(pin.toStdString());
    }
    j["pins"] = pinsJson;

    return j;
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

nlohmann::json* FillFromJsons::findCatalog(nlohmann::json& j, Catalog& currentCatalog, bool findP) {
    if (!j.is_object()) {
        return nullptr;
    }

    if (currentCatalog.parent.isEmpty())
    {
        findCatalog(j, currentCatalog, true);
    }

    if (!findP)
    {
        if (j.contains("catalogs") && j["catalogs"].is_array()) {
            for (auto& catalog : j["catalogs"]) {
                if (catalog.contains("name") && QString::fromStdString(catalog["name"]) == currentCatalog.parent)
                {
                    nlohmann::json* found = findCatalog(catalog, currentCatalog, true);
                    if (found != nullptr) {
                        return found;
                    }
                    else
                    {
                        return nullptr;
                    }
                }
            }
        }
    }

    if (j.contains("catalogs") && j["catalogs"].is_array()) {
        for (auto& catalog : j["catalogs"]) {
            if (catalog.contains("name") && QString::fromStdString(catalog["name"]) == currentCatalog.name) {
                return &catalog;
            }
        }
    }
    return nullptr;
}

void FillFromJsons::deleteComponentFromJson(nlohmann::json& jsonObj, QString mainPath, Catalog& catalog, const QString& componentName) {
    nlohmann::json* catalogPtr = findCatalog(jsonObj, catalog);
    if (catalogPtr == nullptr || !catalogPtr->contains("components")) {
        return;
    }

    auto& components = (*catalogPtr)["components"];

    auto it = std::remove_if(components.begin(), components.end(),
        [&](nlohmann::json& comp) {
            if (comp.contains("model") && comp["model"].is_string()) {
                QString modelStr = QString::fromStdString(comp["model"].get<std::string>());
                return modelStr.compare(componentName, Qt::CaseInsensitive) == 0;
            }
            return false;
        });

    if (it != components.end()) {
        components.erase(it, components.end());
        saveJsonToFile(jsonObj,mainPath);
    }
}

void FillFromJsons::deleteCatalogFromJson(nlohmann::json& j, Catalog& catalog, const QString& mainPath, bool findP)
{
    if (!j.contains("catalogs") || !j["catalogs"].is_array()) {
        return;
    }

    if (catalog.parent.isEmpty())
    {
        findP = true;
        deleteCatalogFromJson(j, catalog, mainPath, findP);
        saveJsonToFile(j, mainPath);
        return;
    }

    auto& catalogsArray = j["catalogs"];

    if (!findP)
    {
        for (auto it = catalogsArray.begin(); it != catalogsArray.end();)
        {
            if (it->contains("name") && QString::fromStdString((*it)["name"]) == catalog.parent)
            {
                findP = true;
                deleteCatalogFromJson(*it, catalog, mainPath, findP);
                saveJsonToFile(j, mainPath);
                return;
            }
            else
            {
                ++it;
            }
        }
    }

    for (auto it = catalogsArray.begin(); it != catalogsArray.end();)
    {
        bool nameMatches = false;
        if (it->contains("name") && QString::fromStdString((*it)["name"]) == catalog.name)
        {
            nameMatches = true;
        }
        if (nameMatches) {
            it = catalogsArray.erase(it);
        }
        else {
            ++it;
        }
    }
    saveJsonToFile(j, mainPath);
}

void FillFromJsons::deleteJsonFile(const QString& folderPath, const QString& fileName)
{
    QDir dir(folderPath);

    if (!dir.exists()) {
        qWarning() << "Папка не найдена:" << folderPath;
        return;
    }

    QString filePath = dir.filePath(fileName);

    if (!QFile::exists(filePath)) {
        qWarning() << "Файл не найден:" << filePath;
        return;
    }

    if (QFile::remove(filePath)) {
        qDebug() << "Файл успешно удалён:" << filePath;
    }
    else {
        qWarning() << "Не удалось удалить файл:" << filePath;
    }
    return;
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

void FillFromJsons::MoveComponent(nlohmann::json jsonObj, QString mainPath, Catalog& currentCatalog, Catalog& nextCatalog, Component& component)
{
    deleteComponentFromJson(jsonObj, mainPath, currentCatalog, component.name);
    AddNewComponentToJson(jsonObj, mainPath, component, nextCatalog.name, component.thumbName, component.ugo.model);
}
