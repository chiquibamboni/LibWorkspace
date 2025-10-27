#include "NewCatalogDialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>

NewCatalogDialog::NewCatalogDialog(QList<Library>* libraries, QList<Catalog>* catalogs, QWidget* parent)
    : QDialog(parent), catalogsList(catalogs)
{
    setupUI();
    setupConnections();
    loadComboBoxData(libraries, catalogs);
    resize(300, 200);
}

NewCatalogDialog::~NewCatalogDialog()
{
}

void NewCatalogDialog::setupUI()
{
    setWindowTitle(QStringLiteral(u"Создание нового каталога"));
    setMinimumWidth(400);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    QFormLayout* formLayout = new QFormLayout();
    nameEdit = new QLineEdit();
    nameEdit->setPlaceholderText(QStringLiteral(u"Введите название каталога"));
    formLayout->addRow(QStringLiteral(u"Название каталога:"), nameEdit);

    QGroupBox* locationGroup = new QGroupBox(QStringLiteral(u"Местоположение каталога"));
    QFormLayout* locationLayout = new QFormLayout();

    libraryCombo = new QComboBox();
    directoryCombo = new QComboBox();

    locationLayout->addRow(QStringLiteral(u"Библиотека:"), libraryCombo);
    locationLayout->addRow(QStringLiteral(u"Каталог(необязательно):"), directoryCombo);

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

void NewCatalogDialog::setupConnections()
{
    connect(okButton, &QPushButton::clicked, this, &NewCatalogDialog::onAccept);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    connect(nameEdit, &QLineEdit::textChanged, this, &NewCatalogDialog::validateForm);
    connect(libraryCombo, &QComboBox::currentTextChanged, this, &NewCatalogDialog::validateForm);
    connect(directoryCombo, &QComboBox::currentTextChanged, this, &NewCatalogDialog::validateForm);
}

void NewCatalogDialog::loadComboBoxData(QList<Library>* lib, QList<Catalog>* catalogs)
{
    QStringList libraries = {};
    QStringList directories = { "" };

    for (auto& libr : *lib)
    {
        libraries.push_back(libr.name);
    }

    for (auto& cat : *catalogs)
    {
        if (cat.components.isEmpty())
        {
            directories.push_back(cat.name);
        }
    }

    libraryCombo->addItems(libraries);
    directoryCombo->addItems(directories);
}

void NewCatalogDialog::onAccept()
{
    currentName = nameEdit->text();
    currentLib = libraryCombo->currentText();
    currentDirectory = directoryCombo->currentText();

    bool nameValid = ValueValidator::hasCyrillicCharacters(currentName);

    if (nameValid)
    {
        QMessageBox::information(this, QStringLiteral(u"Ошибка"),
            QStringLiteral(u"Допустимы только латинские буквы"));
        return;
    }

    for (auto& newCat : *catalogsList) {
        if (newCat.name == currentName) {
            QMessageBox::information(this, QStringLiteral(u"Ошибка"),
                QStringLiteral(u"Каталог с таким именем уже существует"));
            return;
        }
    }

    accept();
}

void NewCatalogDialog::validateForm()
{
    bool isValid = !nameEdit->text().trimmed().isEmpty() &&
        !libraryCombo->currentText().isEmpty();

    okButton->setEnabled(isValid);
}