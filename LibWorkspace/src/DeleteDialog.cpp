#include "DeleteDialog.h"

DeleteDialog::DeleteDialog(Library* currentLibrary, Catalog* currentCatalog, Component* currentComponent, QWidget* parent)
    : QDialog(parent),
    lib(currentLibrary),
    cat(currentCatalog),
    comp(currentComponent)
{
    setupUI();
    setupConnections();
    //updateNameLabel();
}

DeleteDialog::~DeleteDialog()
{
}

void DeleteDialog::setupUI()
{
    setWindowTitle(QStringLiteral(u"Удаление"));
    setMinimumWidth(400);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // Создаем Label для полного сообщения
    QLabel* questionTextLabel = new QLabel();
    questionTextLabel->setWordWrap(true);
    mainLayout->addWidget(questionTextLabel);

    // Инициализация типа и имени
    QString item1 = QStringLiteral(u"компонент");
    QString item2 = QStringLiteral(u"каталог");
    QString item3 = QStringLiteral(u"библиотека");

    bool canDelete = true; // флаг, можно ли удалить объект

    int contentCount = 0; // для подсчета содержимого библиотеки

    if (comp != nullptr)
    {
        currentType = item1;
        currentName = comp->model;
    }
    else if (cat != nullptr)
    {
        // Проверка на непустой каталог
        if (!cat->catalogs.isEmpty() || !cat->components.isEmpty())
        {
            canDelete = false;
        }
        currentType = item2;
        currentName = cat->name;
    }
    else if (lib != nullptr)
    {
        // Проверка на непустую библиотеку
        contentCount = lib->catalogs.size();

        if (contentCount > 0)
        {
            canDelete = false;
        }
        currentType = item3;
        currentName = lib->name;
    }
    else
    {
        currentType = "";
        currentName = "";
    }

    if (canDelete)
    {
        // Формируем обычное сообщение
        QString fullText = QStringLiteral(u"Вы действительно хотите удалить %1 %2?")
            .arg(currentType)
            .arg(currentName);
        questionTextLabel->setText(fullText);
    }
    else
    {
        // Сообщение, что удаление невозможно
        QString errorMsg;

        if (currentType == item3 && contentCount > 0)
        {
            // Правильное склонение для количества каталогов
            errorMsg = QStringLiteral(u"Удаление невозможно: выбранная %1 '%2' не пустая.")
                .arg(currentType)
                .arg(currentName);
        }
        else
        {
            // Общее сообщение для других случаев
            errorMsg = QStringLiteral(u"Удаление невозможно: выбранный %1 '%2' не пустой.")
                .arg(currentType)
                .arg(currentName);
        }

        questionTextLabel->setText(errorMsg);
    }

    // Создаём кнопку
    QDialogButtonBox* buttonBox = new QDialogButtonBox();

    if (canDelete)
    {
        // Обычные кнопки "Удалить" и "Отмена"
        deleteButton = buttonBox->addButton(QDialogButtonBox::Ok);
        cancelButton = buttonBox->addButton(QDialogButtonBox::Cancel);

        deleteButton->setText(QStringLiteral(u"Удалить"));
        cancelButton->setText(QStringLiteral(u"Отмена"));
    }
    else
    {
        okButton = new QPushButton(QStringLiteral(u"ОК"));
        buttonBox->addButton(okButton, QDialogButtonBox::AcceptRole);
    }

    mainLayout->addWidget(buttonBox);
}

void DeleteDialog::setupConnections()
{
    connect(deleteButton, &QPushButton::clicked, this, &DeleteDialog::onAccept);
    connect(cancelButton, &QPushButton::clicked, this, &DeleteDialog::onReject);
    connect(okButton, &QPushButton::clicked, this, &DeleteDialog::onReject);
    connect(elementComboBox, &QComboBox::currentTextChanged, this, &DeleteDialog::updateNameLabel);
}

void DeleteDialog::onReject()
{
    //elementComboBox->clear();
    lib = nullptr;
    cat = nullptr;
    comp = nullptr;
    reject();
}

void DeleteDialog::updateNameLabel()
{

    //QString currentText = elementComboBox->currentText();

    //if (currentText == QStringLiteral(u"компонент")) {
    //    nameLabel->setText(comp->model);
    //}
    //else if (currentText == QStringLiteral(u"каталог")) {
    //    nameLabel->setText(cat->name);
    //}
    //else if (currentText == QStringLiteral(u"библиотеку")) {
    //    nameLabel->setText(lib->name);
    //}
}


void DeleteDialog::onAccept()
{

    if (currentType == QStringLiteral(u"компонент")) {
        // Вызов метода для удаления компонента
        QString filePath = "./Libraries/" + lib->dir + "/" + lib->components_location;
        QString fileName = comp->model + ".json";
        FillFromJsons::deleteJsonFile(filePath, fileName);
        QString libraryPath = "./Libraries/" + lib->dir + "/library.json";
        nlohmann::json jsonObj = FillFromJsons::readJson(libraryPath, this);
        FillFromJsons::deleteComponentFromJson(jsonObj, libraryPath, *cat, comp->model);
    }
    else if (currentType == QStringLiteral(u"каталог")) {
        QString filePath = "./Libraries/" + lib->dir + "/" + lib->components_location;
        QString libraryPath = "./Libraries/" + lib->dir + "/library.json";
        nlohmann::json jsonObj = FillFromJsons::readJson(libraryPath, this);
        FillFromJsons::deleteCatalogFromJson(jsonObj, *cat, libraryPath);
    }
    else if (currentType == QStringLiteral(u"библиотеку")) {
        QString filePath = "./Libraries/" + lib->dir;
        QString fileName = "library.json";
        FillFromJsons::deleteJsonFile(filePath, fileName);
    }

    accept();
}