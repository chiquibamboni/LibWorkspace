#include "ComponentEditor.h"
#include <qheaderview.h>
#include <QSplitter>
#include <QString>
#include <QMessageBox>
#include <QMenu>

#include <QLabel>

const QStringList ComponentEditor::TYPE_ITEMS = {
    "Real", "Integer", "Complex", "String", "Link"
};

const QStringList ComponentEditor::FEATURE_ITEMS = {
    "No Unit", "Angle", "Capacitance", "Conductance", "Current",
    "Datarate","dB", "Distance", "Frequency", "Inductance", "Length",
    "Power", "Resistance", "String", "Temperature", "Time", "Voltage"
};

const QStringList ComponentEditor::UNIT_ITEMS = {
    "Ohm", "Hz", "F"
};

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
    QVBoxLayout* parametersLayout = new QVBoxLayout(parametersGroup);

    parametersLayout->setContentsMargins(0, 0, 0, 0);
    parametersLayout->setSpacing(1);

    QWidget* formWidget = new QWidget();
    QFormLayout* formLayout = new QFormLayout(formWidget);

    QLabel* linkLabel = new QLabel("");
    formLayout->addRow(new QLabel(QStringLiteral(u"Ссылка")), linkLabel);

    QLineEdit* nameEdit = new QLineEdit();
    formLayout->addRow(new QLabel(QStringLiteral(u"Имя")), nameEdit);

    QComboBox* typeCombo = new QComboBox();
    typeCombo->addItems(TYPE_ITEMS);
    formLayout->addRow(new QLabel(QStringLiteral(u"Тип")), typeCombo);

    formLayout->addRow(new QLabel(""), new QLabel(QStringLiteral(u"Пример: 12.34e+6")));

    QLineEdit* defaultValueEdit = new QLineEdit();
    formLayout->addRow(new QLabel(QStringLiteral(u"Значение по умолчанию")), defaultValueEdit);

    QComboBox* featureCombo = new QComboBox();
    featureCombo->addItems(FEATURE_ITEMS);
    formLayout->addRow(new QLabel(QStringLiteral(u"Характеристика")), featureCombo);

    QComboBox* unitCombo = new QComboBox();
    unitCombo->addItems(UNIT_ITEMS);
    formLayout->addRow((QStringLiteral(u"Единица измерения")), unitCombo);

    QLineEdit* descriptionEdit = new QLineEdit();
    formLayout->addRow(new QLabel(QStringLiteral(u"Описание")), descriptionEdit);

    QGroupBox* additionalGroup = new QGroupBox(QStringLiteral(u"Дополнительно"));
    QVBoxLayout* additionalLayout = new QVBoxLayout(additionalGroup);

    QCheckBox* displayCheckBox = new QCheckBox(QStringLiteral(u"Отображать на схеме"));
    QCheckBox* optimizableCheckBox = new QCheckBox(QStringLiteral(u"Доступен для оптимизации"));
    QCheckBox* editedCheckBox = new QCheckBox(QStringLiteral(u"Запретить редактирование"));
    QCheckBox* netlistedCheckBox = new QCheckBox(QStringLiteral(u"Не добавлять в netlist"));

    additionalLayout->addWidget(displayCheckBox);
    additionalLayout->addWidget(optimizableCheckBox);
    additionalLayout->addWidget(editedCheckBox);
    additionalLayout->addWidget(netlistedCheckBox);

    parametersLayout->addWidget(formWidget);
    parametersLayout->addWidget(additionalGroup);

    parametersLayout->addStretch(1);

    mainLayout->addWidget(parametersGroup);

    mainLayout->setContentsMargins(5, 5, 5, 5);
   parametersGroup->setMinimumWidth(400);
}