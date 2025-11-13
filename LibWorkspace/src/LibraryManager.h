#pragma once

#include <QWidget>
#include <QTreeView>
#include <QHeaderView>
#include <QStringList>
#include <QMessageBox>
#include <QDir>

#include <nlohmann/json.hpp>

#include "Library.h"
#include "Catalog.h"
#include "Component.h"
#include "FillFromJsons.h"



class LibraryManager : public QTreeView
{
    Q_OBJECT

public:
    LibraryManager(QList<Library>* libraries, QList<Catalog>* catalogs, QWidget* parent = nullptr);
    ~LibraryManager();

    QStandardItemModel* model;
    QString currentPath = "./Libraries";
    Library* currentLibrary = nullptr;
    Catalog* currentCatalog = nullptr;

    void request();
    void clearLibraries();
    void reExpand(QString node1, QString node2, QString node3);
private:
    QStandardItem* root;
    QIcon defaultFolder = QIcon("./icons/folder.svg");
    bool firstRequest = true;
    QString iconPath;
    QList<Library>* librariesList;
    QList<Catalog>* catalogsList;

    void setupTree();
    void updateTree(const nlohmann::ordered_json& jsonData);
    void addLibraryToModel(const nlohmann::ordered_json& jsonObj, QStandardItem* parentItem);
    QStandardItem* findItemByText(QStandardItem* parent, const QString& text);
};