#include "NewComponentDialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>

NewComponentDialog::NewComponentDialog(QList<Library>* libraries, QList<Catalog>* catalogs,
    QList<Component>* components, QList<Parameters>* curParameters, QString thumbName,
    QString ugoName, QWidget* parent)
    : QDialog(parent),librariesList(libraries), catalogsList(catalogs), componentsList(components)
{
    curParametersList = curParameters;
    newThumbName = thumbName;
    newUgoName = ugoName;
    setupUI();
    setupConnections();
    loadComboBoxData(libraries, catalogs);
    resize(300, 300);
}

NewComponentDialog::~NewComponentDialog()
{
}

void NewComponentDialog::setupUI()
{
    setWindowTitle(QStringLiteral(u"Создание нового компонента"));
    setMinimumWidth(400);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    QFormLayout* formLayout = new QFormLayout();
    nameEdit = new QLineEdit();
    nameEdit->setPlaceholderText(QStringLiteral(u"Введите название компонента"));
    formLayout->addRow(QStringLiteral(u"Название компонента:"), nameEdit);
    descEdit = new QTextEdit();
    descEdit->setPlaceholderText(QStringLiteral(u"Введите описание компонента"));
    formLayout->addRow(QStringLiteral(u"Описание:"), descEdit);

    QGroupBox* locationGroup = new QGroupBox(QStringLiteral(u"Местоположение компонента"));
    QFormLayout* locationLayout = new QFormLayout();

    libraryCombo = new QComboBox();
    directoryCombo = new QComboBox();
    categoryCombo = new QComboBox();

    locationLayout->addRow(QStringLiteral(u"Библиотека:"), libraryCombo);
    locationLayout->addRow(QStringLiteral(u"Каталог:"), directoryCombo);
    locationLayout->addRow(QStringLiteral(u"Категория:"), categoryCombo);

    locationGroup->setLayout(locationLayout);

    QDialogButtonBox* buttonBox = new QDialogButtonBox();
    okButton = buttonBox->addButton(QDialogButtonBox::Ok);
    cancelButton = buttonBox->addButton(QDialogButtonBox::Cancel);

    okButton->setText(QStringLiteral(u"Создать"));
    cancelButton->setText(QStringLiteral(u"Отмена"));

    mainLayout->addLayout(formLayout);
    mainLayout->addWidget(locationGroup);
    mainLayout->addWidget(buttonBox);

    okButton->setEnabled(false);
}

void NewComponentDialog::setupConnections()
{
    connect(okButton, &QPushButton::clicked, this, &NewComponentDialog::onAccept);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    connect(nameEdit, &QLineEdit::textChanged, this, &NewComponentDialog::validateForm);
    connect(libraryCombo, &QComboBox::currentTextChanged, this, &NewComponentDialog::validateForm);
    connect(directoryCombo, &QComboBox::currentTextChanged, this, &NewComponentDialog::validateForm);
    connect(categoryCombo, &QComboBox::currentTextChanged, this, &NewComponentDialog::validateForm);
    connect(directoryCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
        this, &NewComponentDialog::onDirectoryChanged);
}

void NewComponentDialog::loadComboBoxData(QList<Library>* lib, QList<Catalog>* catalogs)
{
    QStringList libraries = {};
    QStringList directories = {"None"};

    for (auto& libr : *lib)
    {
        libraries.push_back(libr.name);
    }

    for (auto& cat : *catalogs)
    {
        if (!cat.catalogs.isEmpty())
        {
            directories.push_back(cat.name);
        }
    }

    libraryCombo->addItems(libraries);
    directoryCombo->addItems(directories);

    if (!libraries.isEmpty()) libraryCombo->setCurrentIndex(0);
    if (!directories.isEmpty())
    {
        directoryCombo->setCurrentIndex(0);
        updateCategories(directoryCombo->currentText());
    }
}

void NewComponentDialog::onDirectoryChanged()
{
    QString selectedDirectory = directoryCombo->currentText();
    updateCategories(selectedDirectory);
    validateForm();
}

void NewComponentDialog::updateCategories(const QString& directoryName)
{
    categoryCombo->clear();
    QStringList categories;

    if (directoryName == "None")
    {
        for (auto& cat : *catalogsList)
        {
            if (cat.catalogs.isEmpty())
            {
                categories.push_back(cat.name);
            }
        }

        categoryCombo->addItems(categories);

        if (!categories.isEmpty())
        {
            categoryCombo->setCurrentIndex(0);
        }
        return;
    }

    for (auto& catalog : *catalogsList)
    {
        if (catalog.name == directoryName && !catalog.catalogs.isEmpty())
        {

            for (auto& subCatalog : catalog.catalogs)
            {
                categories.push_back(subCatalog.name);
            }

            categoryCombo->addItems(categories);

            if (!categories.isEmpty())
            {
                categoryCombo->setCurrentIndex(0);
            }
            break;
        }
    }
}

void NewComponentDialog::createNewComponent(QString name, QString library, QString directory, QString category, QString desc)
{
    Component* newComponent = new Component();
    QString libPath;
    QString mainPath;

    newComponent->name = name;
    newComponent->desc = desc;
    newComponent->library = library;
    newComponent->model = name;
    newComponent->parameters = *curParametersList;
    newComponent->thumbName = newThumbName;
    newComponent->ugo.model = newUgoName;

    for (auto& lib : *librariesList) {
        if (lib.name == newComponent->library) {
            libPath = "./Libraries/" + lib.dir + "/library.json";
            mainPath = "./Libraries/" + lib.dir;
            break;
        }
    }

    nlohmann::ordered_json jsonObj = FillFromJsons::readJson(libPath, this);

    FillFromJsons::AddNewComponentToJson(jsonObj, mainPath, *newComponent, directory, category);
}

void NewComponentDialog::editComponent(QString currentName, QString currentDesc)
{
    QString foundLib;
    QString foundDur;
    QString foundCat;

    for (auto& lib : *librariesList) {
        for (auto& cat : *catalogsList)
        {
            if (!cat.components.isEmpty())
            {
                for (auto& comp : cat.components)
                {
                    if (comp.model == currentName)
                    {
                        foundLib = lib.name;
                        foundDur = cat.parent;
                        foundCat = cat.name;
                        QString filePath = "./Libraries/" + lib.dir + "/" + lib.components_location;
                        QString fileName = comp.model + ".json";
                        FillFromJsons::deleteJsonFile(filePath, fileName);
                        QString libraryPath = "./Libraries/" + lib.dir + "/library.json";
                        nlohmann::ordered_json jsonObj = FillFromJsons::readJson(libraryPath, this);
                        FillFromJsons::deleteComponentFromJson(jsonObj, libraryPath, cat, comp.model);
                        createNewComponent(comp.model, foundLib, foundDur, foundCat, currentDesc);
                        return;
                    }
                }

            } 
            if (!cat.catalogs.isEmpty())
            {
                for (auto& Subcat : cat.catalogs)
                {
                    if (!Subcat.components.isEmpty())
                    {
                        for (auto& comp : Subcat.components)
                        {
                            if (comp.model == currentName)
                            {
                                foundLib = lib.name;
                                foundDur = Subcat.parent;
                                foundCat = Subcat.name;
                                QString filePath = "./Libraries/" + lib.dir + "/" + lib.components_location;
                                QString fileName = comp.model + ".json";
                                FillFromJsons::deleteJsonFile(filePath, fileName);
                                QString libraryPath = "./Libraries/" + lib.dir + "/library.json";
                                nlohmann::ordered_json jsonObj = FillFromJsons::readJson(libraryPath, this);
                                FillFromJsons::deleteComponentFromJson(jsonObj, libraryPath, Subcat, comp.model);
                                createNewComponent(comp.model, foundLib, foundDur, foundCat, currentDesc);
                                return;
                            }
                        }
                    }
                }
            }
        }
    }
}

void NewComponentDialog::loadCurrentCompData(Component curComp, Catalog curCat)
{
    if (!curComp.model.isEmpty())
    {
        nameEdit->setText(curComp.model);
        descEdit->setText(curComp.desc);
        libraryCombo->setCurrentText(curComp.library);
        if (!curCat.parent.isEmpty())
            directoryCombo->setCurrentText(curCat.parent);
        else
            directoryCombo->setCurrentText("None");
        categoryCombo->setCurrentText(curCat.name);
    }
}

void NewComponentDialog::loadCurrentCatData(Catalog curCat)
{
    if (!curCat.name.isEmpty())
    {
        if (!curCat.parent.isEmpty())
        {
            directoryCombo->setCurrentText(curCat.parent);
            categoryCombo->setCurrentText(curCat.name);
        }
        else
        {
            if (directoryCombo->findText(curCat.name))
                directoryCombo->setCurrentText(curCat.name);
            else
            {
                directoryCombo->setCurrentText("None");
                categoryCombo->setCurrentText(curCat.name);
            }
        }
    }
}

void NewComponentDialog::editMakeDialog()
{
    setWindowTitle(QStringLiteral(u"Редактирование компонента"));
    nameEdit->setEnabled(false);
    libraryCombo->setEnabled(false);
    directoryCombo->setEnabled(false);
    categoryCombo->setEnabled(false);
    okButton->setText(QStringLiteral(u"Редактировать"));
}

void NewComponentDialog::onAccept()
{
    currentName = nameEdit->text();
    currentDesc = descEdit->toPlainText();
    currentLib = libraryCombo->currentText();
    currentDirectory = directoryCombo->currentText();
    currentCategory = categoryCombo->currentText();

    bool descValid = ValueValidator::hasCyrillicCharacters(currentDesc);
    bool nameValid = ValueValidator::hasCyrillicCharacters(currentName);

    if (descValid || nameValid)
    {
        QMessageBox::information(this, QStringLiteral(u"Ошибка"),
            QStringLiteral(u"Допустимы только латинские буквы"));
        return;
    }

    for (auto& newComp : *componentsList) {
        if (newComp.model == currentName) {
            if (okButton->text() == QStringLiteral(u"Редактировать"))
            {
                editComponent(currentName, currentDesc);
                accept();
            }
            else
            {
                QMessageBox reply;
                reply.setWindowTitle(QStringLiteral(u"Подтверждение"));
                reply.setText(QStringLiteral(u"Компонент с таким именем уже существует. Хотите его отредактировать?"));
                reply.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
                reply.setButtonText(QMessageBox::Yes, QStringLiteral(u"Да"));
                reply.setButtonText(QMessageBox::No, QStringLiteral(u"Нет"));
                if (reply.exec() == QMessageBox::Yes) {
                    editComponent(currentName, currentDesc);
                    accept();
                }
            }
            return;
        }
    }
    
    createNewComponent(currentName, currentLib, currentDirectory, currentCategory, currentDesc);

    accept();
}

void NewComponentDialog::validateForm()
{
    bool isValid = !nameEdit->text().trimmed().isEmpty() &&
        !libraryCombo->currentText().isEmpty() &&
        !directoryCombo->currentText().isEmpty() &&
        !categoryCombo->currentText().isEmpty();

    okButton->setEnabled(isValid);
}