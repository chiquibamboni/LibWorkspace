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

    QGroupBox* parametersGroup = new QGroupBox(QStringLiteral(u"���������"));
    QVBoxLayout* parametersLayout = new QVBoxLayout(parametersGroup);

    parametersLayout->setContentsMargins(0, 0, 0, 0);
    parametersLayout->setSpacing(1);

    QWidget* formWidget = new QWidget();
    QFormLayout* formLayout = new QFormLayout(formWidget);

    QLabel* linkLabel = new QLabel("");
    formLayout->addRow(new QLabel(QStringLiteral(u"������")), linkLabel);

    QLineEdit* nameEdit = new QLineEdit();
    formLayout->addRow(new QLabel(QStringLiteral(u"���")), nameEdit);

    QComboBox* typeCombo = new QComboBox();
    typeCombo->addItems(TYPE_ITEMS);
    formLayout->addRow(new QLabel(QStringLiteral(u"���")), typeCombo);

    formLayout->addRow(new QLabel(""), new QLabel(QStringLiteral(u"������: 12.34e+6")));

    QLineEdit* defaultValueEdit = new QLineEdit();
    formLayout->addRow(new QLabel(QStringLiteral(u"�������� �� ���������")), defaultValueEdit);

    QComboBox* featureCombo = new QComboBox();
    featureCombo->addItems(FEATURE_ITEMS);
    formLayout->addRow(new QLabel(QStringLiteral(u"��������������")), featureCombo);

    QComboBox* unitCombo = new QComboBox();
    unitCombo->addItems(UNIT_ITEMS);
    formLayout->addRow((QStringLiteral(u"������� ���������")), unitCombo);

    QLineEdit* descriptionEdit = new QLineEdit();
    formLayout->addRow(new QLabel(QStringLiteral(u"��������")), descriptionEdit);

    QGroupBox* additionalGroup = new QGroupBox(QStringLiteral(u"�������������"));
    QVBoxLayout* additionalLayout = new QVBoxLayout(additionalGroup);

    QCheckBox* displayCheckBox = new QCheckBox(QStringLiteral(u"���������� �� �����"));
    QCheckBox* optimizableCheckBox = new QCheckBox(QStringLiteral(u"�������� ��� �����������"));
    QCheckBox* editedCheckBox = new QCheckBox(QStringLiteral(u"��������� ��������������"));
    QCheckBox* netlistedCheckBox = new QCheckBox(QStringLiteral(u"�� ��������� � netlist"));

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