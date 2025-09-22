#include "ComponentEditor.h"
#include <qheaderview.h>
#include <QSplitter>
#include <QString>
#include <QMessageBox>
#include <QMenu>

#include <QLabel>

ComponentEditor::ComponentEditor(QWidget* parent)
    : QMainWindow(parent)
{
    setupUi();
}

ComponentEditor::~ComponentEditor()
{
}

void ComponentEditor::setupUi()
{
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QHBoxLayout* mainLayout = new QHBoxLayout(centralWidget);

    QGroupBox* parametersGroup = new QGroupBox(QStringLiteral(u"Параметры"));
    QFormLayout* formLayout = new QFormLayout(parametersGroup);

    QLabel* linkLabel = new QLabel(QStringLiteral(u"Ссылка"));
    QLabel* linkLabelContent = new QLabel("");
    formLayout->addRow(linkLabel, linkLabelContent);

    QLabel* nameLabel = new QLabel(QStringLiteral(u"Имя"));
    QLineEdit* nameEdit = new QLineEdit();
    formLayout->addRow(nameLabel, nameEdit);

    QLabel* typeLabel = new QLabel(QStringLiteral(u"Тип"));
    QComboBox* typeCombo = new QComboBox();
    typeCombo->addItems(QStringList() << "Real" << "Integer" << "Complex" << "String" << "Link");
    formLayout->addRow(typeLabel, typeCombo);

    QLabel* defaultValueLabel = new QLabel(QStringLiteral(u"Значение по умолчанию"));
    QLineEdit* defaultValueEdit = new QLineEdit();
    formLayout->addRow(defaultValueLabel, defaultValueEdit);

    QLabel* featureLabel = new QLabel(QStringLiteral(u"Характеристика"));
    QComboBox* featureCombo = new QComboBox();
    featureCombo->addItems(QStringList() << "No Unit" << "Angle" << "Capacitance" << "Conductance" << "Current");
    formLayout->addRow(featureLabel, featureCombo);

    QLabel* unitLabel = new QLabel(QStringLiteral(u"Единица измерения"));
    QComboBox* unitCombo = new QComboBox();
    unitCombo->addItems(QStringList() << "Ohm" << "Hz" << "F");
    formLayout->addRow(unitLabel, unitCombo);

    QLabel* descriptionLabel = new QLabel(QStringLiteral(u"Описание"));
    QLineEdit* descriptionEdit = new QLineEdit();
    formLayout->addRow(descriptionLabel, descriptionEdit);

    // Добавляем группу в главный layout
    mainLayout->addWidget(parametersGroup);

    // Устанавливаем отступы и растяжение
    mainLayout->setContentsMargins(10, 10, 10, 10);
    parametersGroup->setMinimumWidth(400);
}