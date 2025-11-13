#include "MoveDialog.h"

MoveDialog::MoveDialog(Library* currentLibrary, Catalog* currentCatalog, Component* currentComponent, QString direction, QWidget* parent)
    : QDialog(parent),
    lib(currentLibrary),
    cat(currentCatalog),
    comp(currentComponent)
{
    nextCat = findNeighborCatalog(lib, cat, direction);

    setupUI(direction);
    setupConnections();
}

MoveDialog::~MoveDialog()
{
}

void MoveDialog::setupUI(QString direction)
{
    setWindowTitle(QStringLiteral(u"Перемещение"));
    setMinimumWidth(400);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    QLabel* messageLabel = new QLabel(this);
    messageLabel->setWordWrap(true);

    QString message;
    if (!nextCat)
    {
        QString directionStr;
        if (direction == "up")
            directionStr = QStringLiteral(u"вверх");
        else  if (direction == "down")
            directionStr = QStringLiteral(u"вниз");

        message = QStringLiteral(u"Перемещение компонента <b>%1</b> %2 невозможно")
            .arg(comp->model)
            .arg(directionStr);
    }
    else
    {
        message = QStringLiteral(u"Вы действительно хотите переместить компонент <b>%1</b> из %2 в %3?")
            .arg(comp->model)
            .arg(cat->name)
            .arg(nextCat->name);
    }

    messageLabel->setText(message);
    mainLayout->addWidget(messageLabel);

    QDialogButtonBox* buttonBox = new QDialogButtonBox();

    if (!nextCat)
    {
        okButton = buttonBox->addButton(QDialogButtonBox::Ok);
        okButton->setText(QStringLiteral(u"OK"));
        buttonBox->setCenterButtons(true);
    }
    else
    {
        moveButton = buttonBox->addButton(QDialogButtonBox::Ok);
        cancelButton = buttonBox->addButton(QDialogButtonBox::Cancel);

        moveButton->setText(QStringLiteral(u"Переместить"));
        cancelButton->setText(QStringLiteral(u"Отмена"));

        buttonBox->setCenterButtons(false);
    }

    mainLayout->addWidget(buttonBox);
}

void MoveDialog::setupConnections()
{
    connect(moveButton, &QPushButton::clicked, this, &MoveDialog::onAccept);
    connect(cancelButton, &QPushButton::clicked, this, &MoveDialog::onReject);
    connect(okButton, &QPushButton::clicked, this, &MoveDialog::onReject);
}

Catalog* MoveDialog::findNeighborCatalog(Library* lib, Catalog* currentCatalog, QString direction) {
    if (!lib || !currentCatalog || currentCatalog->parent.isEmpty()) return nullptr;

    Catalog* parentCat = nullptr;
    for (auto& catalog : lib->catalogs) {
        if (catalog.name == currentCatalog->parent) {
            parentCat = &catalog;
            break;
        }
    }
    if (!parentCat) {
        parentCat = currentCatalog;
    }

    auto& subCatalogs = parentCat->catalogs;

    int index = -1;
    for (int i = 0; i < subCatalogs.size(); ++i) {
        if (subCatalogs[i].name == currentCatalog->name) {
            index = i;
            break;
        }
    }
    if (index == -1) return nullptr;

    int targetIndex = -1;
    if (direction == "down") {
        if (index + 1 < subCatalogs.size()) {
            targetIndex = index + 1;
        }
    }
    else if (direction == "up") {
        if (index - 1 >= 0) {
            targetIndex = index - 1;
        }
    }
    else {
        return nullptr;
    }

    if (targetIndex >= 0 && targetIndex < subCatalogs.size()) {
        Catalog* neighborCatalog = &subCatalogs[targetIndex];
        return neighborCatalog;
    }

    return nullptr;
}

void MoveDialog::onReject()
{
    lib = nullptr;
    cat = nullptr;
    comp = nullptr;
    reject();
}

void MoveDialog::onAccept()
{
    QString libraryPath = "./Libraries/" + lib->dir + "/library.json";
    nlohmann::ordered_json jsonObj = FillFromJsons::readJson(libraryPath, this);
    FillFromJsons::moveComponent(jsonObj, libraryPath, *cat, *nextCat, *comp);
    accept();
}