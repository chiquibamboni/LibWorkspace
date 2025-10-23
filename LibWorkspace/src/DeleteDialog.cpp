#include "DeleteDialog.h"

DeleteDialog::DeleteDialog(Library* currentLibrary, Catalog* currentCatalog, Component* currentComponent, QWidget* parent)
    : QDialog(parent),
    lib(currentLibrary),
    cat(currentCatalog),
    comp(currentComponent)
{
    setupUI();
    setupConnections();
    updateNameLabel();
}

DeleteDialog::~DeleteDialog()
{
}

void DeleteDialog::setupUI()
{
    setWindowTitle(QStringLiteral(u"Удаление"));
    setMinimumWidth(400);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    QHBoxLayout* questionLayout = new QHBoxLayout();

    QLabel* questionLabel1 = new QLabel(QStringLiteral(u"Вы действительно хотите удалить "));
    questionLabel1->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    questionLayout->addWidget(questionLabel1);

    elementComboBox = new QComboBox();
    QString item1 = QStringLiteral(u"компонент");
    QString item2 = QStringLiteral(u"каталог");
    QString item3 = QStringLiteral(u"библиотеку");
    if (comp != nullptr)
    {
        elementComboBox->addItem(item1);
    }
    if (cat != nullptr)
    {
        elementComboBox->addItem(item2);
    }
    elementComboBox->addItem(item3);
    questionLayout->addWidget(elementComboBox);

    nameLabel = new QLabel();
    nameLabel->setStyleSheet("font-weight: bold;");
    questionLayout->addWidget(nameLabel);

    QLabel* questionLabel2 = new QLabel(QStringLiteral(u"?"));
    questionLabel2->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    questionLayout->addWidget(questionLabel2);

    questionLayout->addStretch();

    mainLayout->addLayout(questionLayout);

    QDialogButtonBox* buttonBox = new QDialogButtonBox();
    okButton = buttonBox->addButton(QDialogButtonBox::Ok);
    cancelButton = buttonBox->addButton(QDialogButtonBox::Cancel);

    okButton->setText(QStringLiteral(u"Удалить"));
    cancelButton->setText(QStringLiteral(u"Отмена"));

    mainLayout->addWidget(buttonBox);
}

void DeleteDialog::setupConnections()
{
    connect(okButton, &QPushButton::clicked, this, &DeleteDialog::onAccept);
    connect(cancelButton, &QPushButton::clicked, this, &DeleteDialog::onReject);
    connect(elementComboBox, &QComboBox::currentTextChanged, this, &DeleteDialog::updateNameLabel);
}

void DeleteDialog::onReject()
{
    elementComboBox->clear();
    lib = nullptr;
    cat = nullptr;
    comp = nullptr;
    reject();
}

void DeleteDialog::updateNameLabel()
{

    QString currentText = elementComboBox->currentText();

    if (currentText == QStringLiteral(u"компонент")) {
        nameLabel->setText(comp->model);
    }
    else if (currentText == QStringLiteral(u"каталог")) {
        nameLabel->setText(cat->name);
    }
    else if (currentText == QStringLiteral(u"библиотеку")) {
        nameLabel->setText(lib->name);
    }
}


void DeleteDialog::onAccept()
{
    QString currentText = elementComboBox->currentText();

    if (currentText == QStringLiteral(u"компонент")) {
        // Вызов метода для удаления компонента
        QString filePath = "./Libraries/" + lib->dir + "/" + lib->components_location;
        QString fileName = comp->model + ".json";
        FillFromJsons::deleteJsonFile(filePath, fileName);
        QString libraryPath = "./Libraries/" + lib->dir + "/library.json";
        nlohmann::json jsonObj = FillFromJsons::readJson(libraryPath, this);
        FillFromJsons::deleteComponentFromJson(jsonObj, libraryPath, cat->name, comp->model);
    }
    else if (currentText == QStringLiteral(u"каталог")) {
        // Вызов метода для удаления каталога
    }
    else if (currentText == QStringLiteral(u"библиотеку")) {
        // Вызов метода для удаления библиотеки
    }

    accept();
}