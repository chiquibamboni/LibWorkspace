#include "FullTableDialog.h"
#include "SvgViewer.h"

FullTableDialog::FullTableDialog(QList<Component>& components, QWidget* parent)
    : QDialog(parent),
    componentsList(components)
{
    fullComponentsTable = new QTableView(this);
    mainLayout = new QVBoxLayout(this);

    mainLayout->addWidget(fullComponentsTable);
    setLayout(mainLayout);

    ugoTabs = new UgoTabs();
    ugoTabs->setTab("ANSI");
    ugoTabs->setTab(QStringLiteral(u"ГОСТ"));

    setupTable();
    fillTable();
    resize(1200, 600);
}

void FullTableDialog::setupTable()
{
    model = new QStandardItemModel(this);
    QStringList headers = {
        "Thumb", "Model", "Veriloga", "Library",
        "Symbol ANSI", "Symbol GOST", "Group", "Parameters", "Description"
    };
    model->setHorizontalHeaderLabels(headers);

    fullComponentsTable->setIconSize(QSize(64, 64));

    fullComponentsTable->setModel(model);

    fullComponentsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    fullComponentsTable->setSelectionMode(QAbstractItemView::SingleSelection);
    fullComponentsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    fullComponentsTable->setAlternatingRowColors(true);
    fullComponentsTable->setShowGrid(false);
    fullComponentsTable->verticalHeader()->setVisible(false);
    fullComponentsTable->horizontalHeader()->setStretchLastSection(true);

    fullComponentsTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    fullComponentsTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    fullComponentsTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    fullComponentsTable->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    fullComponentsTable->horizontalHeader()->setSectionResizeMode(5, QHeaderView::ResizeToContents);
}

void FullTableDialog::fillTable()
{
    model->removeRows(0, model->rowCount());

    int rowIndex = 0;
    for (const auto& component : componentsList)
    {
        QList<QStandardItem*> rowItems;

        // Symbol
        auto* itemSymbol = new QStandardItem();
        itemSymbol->setIcon(component.thumb);
        itemSymbol->setTextAlignment(Qt::AlignCenter);
        rowItems.append(itemSymbol);

        // Model
        auto* itemModel = new QStandardItem(component.model);
        itemModel->setTextAlignment(Qt::AlignCenter);
        rowItems.append(itemModel);

        // Veriloga
        auto* itemVeriloga = new QStandardItem(component.veriloga.model);
        itemVeriloga->setTextAlignment(Qt::AlignCenter);
        rowItems.append(itemVeriloga);

        // Library
        auto* itemLibrary = new QStandardItem(component.library);
        itemLibrary->setTextAlignment(Qt::AlignCenter);
        rowItems.append(itemLibrary);

        QSize symbolSize = QSize(100, 100);

        // UGO ANSI
        auto* itemUgoAnsi = new QStandardItem();
        auto* svgViewerAnsi = new SvgViewer(symbolSize);
        if (!component.ugo.ansiUgoSymbol.isNull())
        {
            svgViewerAnsi->loadSvg(component.ugo.ansiSymbolPath);
            //itemUgoAnsi->setIcon(component.ugo.ansiUgoSymbol);
        }
        itemUgoAnsi->setTextAlignment(Qt::AlignCenter);
        rowItems.append(itemUgoAnsi);

        // UGO GOST
        auto* itemUgoGost = new QStandardItem();
        auto* svgViewerGost = new SvgViewer(symbolSize);
        if (!component.ugo.gostUgoSymbol.isNull())
        {
            svgViewerGost->loadSvg(component.ugo.gostSymbolPath);
        }
        itemUgoGost->setTextAlignment(Qt::AlignCenter);
        rowItems.append(itemUgoGost);

        // Group
        auto* itemGroup = new QStandardItem(component.group);
        itemGroup->setTextAlignment(Qt::AlignCenter);
        rowItems.append(itemGroup);

        // Parameters
        QString parametersStr;
        for (const Parameters& param : component.parameters)
        {
            if (!parametersStr.isEmpty())
                parametersStr += ", ";
            parametersStr += param.name;
        }
        auto* itemParameters = new QStandardItem(parametersStr);
        itemParameters->setTextAlignment(Qt::AlignCenter);
        rowItems.append(itemParameters);

        // Description
        auto* itemDesc = new QStandardItem(component.desc);
        itemDesc->setTextAlignment(Qt::AlignCenter);
        rowItems.append(itemDesc);

        model->appendRow(rowItems);

        QModelIndex index1 = model->index(rowIndex, 4); 
        fullComponentsTable->setIndexWidget(index1, svgViewerAnsi);

        QModelIndex index2 = model->index(rowIndex, 5);
        fullComponentsTable->setIndexWidget(index2, svgViewerGost);

        rowIndex++;
    }

    fullComponentsTable->resizeRowsToContents();
}

FullTableDialog::~FullTableDialog()
{
}

