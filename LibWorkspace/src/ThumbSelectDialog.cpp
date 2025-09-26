#include "ThumbSelectDialog.h"

ThumbSelectDialog::ThumbSelectDialog(const QString& iconsPath, QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(QStringLiteral(u"Выберите иконку"));

    auto layout = new QVBoxLayout(this);

    listWidget = new QListWidget(this);
    listWidget->setViewMode(QListWidget::IconMode);
    listWidget->setIconSize(iconSize);
    listWidget->setGridSize(gridSize);
    listWidget->setSpacing(space);
    listWidget->setResizeMode(QListWidget::Adjust);
    listWidget->setSelectionMode(QAbstractItemView::SingleSelection);

    layout->addWidget(listWidget);

    connect(listWidget, &QListWidget::itemDoubleClicked, this, &ThumbSelectDialog::accept);

    loadIcons(iconsPath);
    resize(dialogSize);
}

ThumbSelectDialog::~ThumbSelectDialog() 
{

}

QIcon ThumbSelectDialog::selectedIcon() const {
    auto item = listWidget->currentItem();
    if (item)
        return item->icon();
    return QIcon();
}

void ThumbSelectDialog::loadIcons(const QString& path) {
    QDir dir(path);
    QStringList filters;
    filters << "*.png" << "*.jpg" << "*.bmp" << "*.ico" << "*.svg";

    dir.setNameFilters(filters);

    QStringList files = dir.entryList(QDir::Files);
    listWidget->clear();

    for (const QString& fileName : files) {
        QString filePath = dir.absoluteFilePath(fileName);
        QIcon icon(filePath);
        QString baseName = QFileInfo(fileName).fileName();
        QListWidgetItem* item = new QListWidgetItem(icon, baseName);
        listWidget->addItem(item);
    }
}