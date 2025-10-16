#include "NewComponentDialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>

NewComponentDialog::NewComponentDialog(QWidget* parent)
    : QDialog(parent)
{
    setupUI();
    setupConnections();
    loadComboBoxData();
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
}

void NewComponentDialog::loadComboBoxData()
{

    QStringList libraries = {
        "Basic"

    };

    QStringList directories = {
         "tt"
    };

    QStringList categories = {
         "tt"
    };

    libraryCombo->addItems(libraries);
    directoryCombo->addItems(directories);
    categoryCombo->addItems(categories);

    if (!libraries.isEmpty()) libraryCombo->setCurrentIndex(0);
    if (!directories.isEmpty()) directoryCombo->setCurrentIndex(0);
    if (!categories.isEmpty()) categoryCombo->setCurrentIndex(0);
}

void NewComponentDialog::onAccept()
{
    currentName = nameEdit->text();
    currentLib = libraryCombo->currentText();
    currentDirectory = categoryCombo->currentText();
    currentCategory = categoryCombo->currentText();

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