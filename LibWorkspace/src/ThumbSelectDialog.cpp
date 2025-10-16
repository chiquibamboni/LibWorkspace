#include "ThumbSelectDialog.h"

ThumbSelectDialog::ThumbSelectDialog(const QList<QString>& iconsPaths, QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(QStringLiteral(u"Выберите иконку"));

    newThumbName = new QString();

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
    for (auto& iconsPath : iconsPaths)
    {
        loadIcons(iconsPath);
    }
    resize(dialogSize);
}

ThumbSelectDialog::~ThumbSelectDialog() 
{

}

QIcon ThumbSelectDialog::selectedIcon() const{
    auto item = listWidget->currentItem();
    QString image = listWidget->currentItem()->text();
   *newThumbName = image;
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

    for (const QString& fileName : files) {
        QString filePath = dir.absoluteFilePath(fileName);
        QIcon icon(filePath);
        QString baseName = QFileInfo(fileName).fileName();
        QListWidgetItem* item = new QListWidgetItem(icon, baseName);
        listWidget->addItem(item);
    }
}