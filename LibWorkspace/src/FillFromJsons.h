#pragma once

#include <QString>
#include <QDir>
#include <QMessageBox>
#include <nlohmann/json.hpp>
#include <QStandardItem>
#include <QTextStream>
#include <QStatusBar>
#include <QWidget>

#include "Library.h"
#include "Parameters.h"

class FillFromJsons {
public:
	static nlohmann::ordered_json readJson(QString currentPath, QWidget* parent);
	static QList<Library> addRootJsonToModel(const nlohmann::ordered_json& jsonObj);
	static Catalog CatalogFromJson(const nlohmann::ordered_json& jsonObj, Library* currentLibrary, QList<Catalog>* catalogsList, Catalog* parent = nullptr);
	static void ComponentFromJson(const nlohmann::ordered_json& jsonObj,
		Component& component, Library* currentLibrary);
	static void addParametrFromJson(nlohmann::ordered_json jsonObj, Parameters& parametr);
	static void addComponentRest(QString& componentModel, Component& component, Library* currentLibrary);
	static void AddComponentToLibraryJson(nlohmann::ordered_json& jsonObj, QString mainPath, Component& component, QString parentCatName, QString catalogName);
	static void AddNewComponentToJson(nlohmann::ordered_json& jsonObj, QString mainPath, Component& component, QString parentCatName,
		QString catalogName);
	static void AddNewCatalogToJson(nlohmann::ordered_json& jsonObj, QString libName, QString catalogName, QString NewCatalogName,
		QString mainPath, QString thumbFileName);
	static nlohmann::ordered_json QVariantToJson(const QVariant& var);
	static nlohmann::ordered_json ParametersToJson(QList<Parameters>& params);
	static nlohmann::ordered_json CreateComponentJson(Component& comp);
	static nlohmann::ordered_json* findCatalog(nlohmann::ordered_json& j, Catalog& currentCatalog, bool findP = false);
	static nlohmann::ordered_json* findCatalogByName(nlohmann::ordered_json& j, const QString& targetName);
	static void deleteCatalogFromJson(nlohmann::ordered_json& j, Catalog& catalog, const QString& mainPath, bool findP = false);
	static void deleteComponentFromJson(nlohmann::ordered_json& jsonObj, QString mainPath, Catalog& catalog, const QString& componentName);
	static void deleteJsonFile(const QString& folderPath, const QString& fileName);
	static void saveJsonToFile(const nlohmann::ordered_json& j, const QString& filePath);
	static void moveComponent(nlohmann::ordered_json jsonObj, QString mainPath, Catalog& currentCatalog, Catalog& nextCatalog, Component& component);
    static nlohmann::ordered_json CreateUgoJson(Component& comp);
    // Метод для установки статус бара
    static void setStatusBar(QStatusBar* statusBar);
    
    // Метод для получения статус бара
    static QStatusBar* getStatusBar();
    
    // Функция для централизованного вывода ошибок
    static void showError(QWidget* parent, const QString& message);
    
private:
    static QStatusBar* statusBar;

};