#pragma once

#include <QString>
#include <QDir>
#include <QMessageBox>
#include <nlohmann/json.hpp>
#include <QStandardItem>

#include "Library.h"

class FillFromJsons {
public:
	static nlohmann::json readJson(QString currentPath, QWidget* parent);
	static QList<Library> addRootJsonToModel(const nlohmann::json& jsonObj);
	static Catalog CatalogFromJson(const nlohmann::json& jsonObj,
		Library* currentLibrary, QList<Catalog>* catalogsList);
	static void ComponentFromJson(const nlohmann::json& jsonObj,
		Component& component, Library* currentLibrary);
};

