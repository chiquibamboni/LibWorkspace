#pragma once
#include <QDialog>
#include "Component.h"
#include "Parameters.h"
#include "UgoTabs.h"
#include <QStandardItemModel>
#include <QTableView>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QStringList>
#include <QList>
#include <QSize>

class FullTableDialog : public QDialog
{
    Q_OBJECT
public:
    FullTableDialog(QList<Component>& components, QWidget* parent = nullptr);
    ~FullTableDialog();

private:
    void setupTable();
    void fillTable();

    UgoTabs* ugoTabs;
    QList<Component> componentsList;
    QTableView* fullComponentsTable;
    QStandardItemModel* model;
    QVBoxLayout* mainLayout;
};

