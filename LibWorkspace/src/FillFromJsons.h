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
	static nlohmann::json readJson(QString currentPath, QWidget* parent);
	static QList<Library> addRootJsonToModel(const nlohmann::json& jsonObj);
	static Catalog CatalogFromJson(const nlohmann::json& jsonObj,
		Library* currentLibrary, QList<Catalog>* catalogsList);
	static void ComponentFromJson(const nlohmann::json& jsonObj,
		Component& component, Library* currentLibrary);
	static void addParametrFromJson(nlohmann::json jsonObj, Parameters& parametr);
	static void addComponentRest(QString& componentModel, Component& component, Library* currentLibrary);
	static void AddNewComponentToJson(nlohmann::json& jsonObj,Component& component, QString catalogName,
		QString mainPath, QString thumbFileName, QString ugoFileName);
	static void AddNewCatalogToJson(nlohmann::json& jsonObj, QString libName, QString catalogName, QString NewCatalogName,
		QString mainPath, QString thumbFileName);
	static nlohmann::json QVariantToJson(const QVariant& var);
	static nlohmann::json ParametersToJson(QList<Parameters>& params);
	static nlohmann::json CreateComponentJson(Component& comp);
	static nlohmann::json* findCatalogByName(nlohmann::json& j, const QString& targetName);
	static void deleteComponentFromJson(nlohmann::json& jsonObj, QString mainPath, const QString& catalogName, const QString& componentName);
	static void deleteJsonFile(const QString& folderPath, const QString& fileName);
	static void saveJsonToFile(const nlohmann::json& j, const QString& filePath);
	static void MoveComponentUp(nlohmann::json jsonObj, Component& component);
	static void MoveComponentDown(nlohmann::json jsonObj, Component& component);
    static nlohmann::json CreateUgoJson(Component& comp);
    // Метод для установки статус бара
    static void setStatusBar(QStatusBar* statusBar);
    
    // Метод для получения статус бара
    static QStatusBar* getStatusBar();
    
    // Функция для централизованного вывода ошибок
    static void showError(QWidget* parent, const QString& message);
    
private:
    static QStatusBar* statusBar;

};