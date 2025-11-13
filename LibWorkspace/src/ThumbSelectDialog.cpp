#include "ThumbSelectDialog.h"

ThumbSelectDialog::ThumbSelectDialog(const QList<QString>& iconsPaths, QString iconType, QWidget* parent)
    : type(iconType), QDialog(parent)
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
   *newThumbName = image.remove(".svg");
    if (item)
        return item->icon();
    return QIcon();
}

void ThumbSelectDialog::loadIcons(const QString& path)
{
    QDir dir(path);
    QStringList filters;
    filters << "*.png" << "*.jpg" << "*.bmp" << "*.ico" << "*.svg";

    dir.setNameFilters(filters);

    QStringList files = dir.entryList(QDir::Files);

    for (const QString& fileName : files) {
        QString filePath = dir.absoluteFilePath(fileName);
        QString baseName = QFileInfo(fileName).fileName();
        QString nameWithoutExt = QFileInfo(baseName).completeBaseName();
        if (type == "component")
        {
            if (!nameWithoutExt.isEmpty() && nameWithoutExt[0].isUpper()) {
                QIcon icon(filePath);
                QListWidgetItem* item = new QListWidgetItem(icon, baseName);
                listWidget->addItem(item);
            }
        }
        else if (type == "catalog")
        {
            if (!nameWithoutExt.isEmpty() && nameWithoutExt[0].isLower()) {
                QIcon icon(filePath);
                QListWidgetItem* item = new QListWidgetItem(icon, baseName);
                listWidget->addItem(item);
            }
        }
    }
}