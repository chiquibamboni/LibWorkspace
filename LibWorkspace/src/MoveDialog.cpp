#include "MoveDialog.h"

MoveDialog::MoveDialog(Library* currentLibrary, Catalog* currentCatalog, Component* currentComponent, QWidget* parent)
    : QDialog(parent),
    lib(currentLibrary),
    cat(currentCatalog),
    comp(currentComponent)
{
    setupUI();
    setupConnections();
}

MoveDialog::~MoveDialog()
{
}

void MoveDialog::setupUI()
{
    setWindowTitle(QStringLiteral(u"Перемещение"));
    setMinimumWidth(400);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    QLabel* messageLabel = new QLabel(this);
    messageLabel->setWordWrap(true);

    QString message = QStringLiteral(u"Вы действительно хотите переместить компонент %1 из %2 в %3 ?")
        .arg(comp->model)
        .arg(cat->name)
        .arg(nextCat->name);

    messageLabel->setText(message);
    mainLayout->addWidget(messageLabel);

    QDialogButtonBox* buttonBox = new QDialogButtonBox();

    moveButton = buttonBox->addButton(QDialogButtonBox::Ok);
    cancelButton = buttonBox->addButton(QDialogButtonBox::Cancel);

    moveButton->setText(QStringLiteral(u"Переместить"));
    cancelButton->setText(QStringLiteral(u"Отмена"));

    mainLayout->addWidget(buttonBox);
}

void MoveDialog::setupConnections()
{
    connect(moveButton, &QPushButton::clicked, this, &MoveDialog::onAccept);
    connect(cancelButton, &QPushButton::clicked, this, &MoveDialog::onReject);
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
    accept();
}