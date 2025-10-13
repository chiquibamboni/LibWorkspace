#include "FullTableDialog.h"

FullTableDialog::FullTableDialog(QList<Component>& components, QWidget* parent)
    : QDialog(parent),
    componentsList(components)
{
    fullComponentsTable = new QTableWidget(this);
    mainLayout = new QVBoxLayout(this);

    mainLayout->addWidget(fullComponentsTable);
    setLayout(mainLayout);

    ugoTabs = new UgoTabs();
    ugoTabs->setTab("ANSI");
    ugoTabs->setTab(QStringLiteral(u"ГОСТ"));

    setupTable();
    fillTable();
    resize(800, 600);
}

void FullTableDialog::setupTable()
{
    QSize iconSize(64, 64);
    fullComponentsTable->setIconSize(iconSize);
    fullComponentsTable->setColumnCount(7);

    QStringList headers;
    headers << "Symbol" << "Model" << "Library" << "UGO" << "Group" << "Parameters" << "Description";
    fullComponentsTable->setHorizontalHeaderLabels(headers);

    fullComponentsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    fullComponentsTable->setSelectionMode(QAbstractItemView::SingleSelection);
    fullComponentsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    fullComponentsTable->setAlternatingRowColors(true);
    fullComponentsTable->setShowGrid(false);
    fullComponentsTable->verticalHeader()->setVisible(false);

    fullComponentsTable->horizontalHeader()->setStretchLastSection(true);
    fullComponentsTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    fullComponentsTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    fullComponentsTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);

    fullComponentsTable->setColumnWidth(0, 30);
    fullComponentsTable->setColumnWidth(1, 150);
}

void FullTableDialog::fillTable()
{
    fullComponentsTable->setRowCount(0);

    for (const auto& component : componentsList)
    {
        int row = fullComponentsTable->rowCount();
        fullComponentsTable->insertRow(row);

        // Symbol
        QTableWidgetItem* item1 = new QTableWidgetItem();
        item1->setIcon(component.thumb);
        item1->setTextAlignment(Qt::AlignCenter);
        fullComponentsTable->setItem(row, 0, item1);

        // Model
        auto* modelItem = new QTableWidgetItem(component.model);
        modelItem->setTextAlignment(Qt::AlignCenter);
        fullComponentsTable->setItem(row, 1, modelItem);

        // Library
        auto* libraryItem = new QTableWidgetItem(component.library);
        libraryItem->setTextAlignment(Qt::AlignCenter);
        fullComponentsTable->setItem(row, 2, libraryItem);

        // UGO
        QTableWidgetItem* item2 = new QTableWidgetItem();
        if (!component.ugo.ansiUgoSymbol.isNull())
        {
            item2->setIcon(component.ugo.ansiUgoSymbol);
        }
        item2->setTextAlignment(Qt::AlignCenter);
        fullComponentsTable->setItem(row, 3, item2);

        // Group
        auto* groupItem = new QTableWidgetItem(component.group);
        groupItem->setTextAlignment(Qt::AlignCenter);
        fullComponentsTable->setItem(row, 4, groupItem);

        // Parameters
        QString parametersStr;
        for (const Parameters& param : component.parameters)
        {
            if (!parametersStr.isEmpty())
                parametersStr += ", ";
            parametersStr += param.name;
        }
        auto* paramItem = new QTableWidgetItem(parametersStr);
        paramItem->setTextAlignment(Qt::AlignCenter);
        fullComponentsTable->setItem(row, 5, paramItem);

        // Description
        auto* descItem = new QTableWidgetItem(component.desc);
        descItem->setTextAlignment(Qt::AlignCenter);
        fullComponentsTable->setItem(row, 6, descItem);
    }

    fullComponentsTable->resizeRowsToContents();
}

FullTableDialog::~FullTableDialog()
{
}