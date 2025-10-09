#include "ParameterEditor.h"
#include <qheaderview.h>
#include <QSplitter>
#include <QString>
#include <QMessageBox>
#include <QMenu>

#include <QLabel>

const QStringList ParameterEditor::TYPE_ITEMS = {
    "Real", "Integer", "Complex", "String", "Link"
};

const QStringList ParameterEditor::FEATURE_ITEMS = {
    "No Unit", "Angle", "Capacitance", "Conductance", "Current",
    "Datarate","dB", "Distance", "Frequency", "Inductance", "Length",
    "Power", "Resistance", "String", "Temperature", "Time", "Voltage"
};

const QStringList ParameterEditor::UNIT_ITEMS = {
    "Ohm", "Hz", "F", "m", "H", "sec", "V", "A"
};

ParameterEditor::ParameterEditor(QWidget* parent)
    : QMainWindow(parent)
{
    setupUi();
}

ParameterEditor::~ParameterEditor()
{
}

void ParameterEditor::setupUi()
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

    linkLabel = new QLabel("");
    formLayout->addRow(new QLabel(QStringLiteral(u"Ссылка")), linkLabel);

    nameEdit = new QLineEdit();
    formLayout->addRow(new QLabel(QStringLiteral(u"Имя")), nameEdit);

    typeComboBox = new QComboBox();
    typeComboBox->addItems(TYPE_ITEMS);
    formLayout->addRow(new QLabel(QStringLiteral(u"Тип")), typeComboBox);

    formLayout->addRow(new QLabel(""), new QLabel(QStringLiteral(u"Пример: 12.34e+6")));

    defaultValueLineEdit = new QLineEdit();
    formLayout->addRow(new QLabel(QStringLiteral(u"Значение по умолчанию")), defaultValueLineEdit);

    featureComboBox = new QComboBox();
    featureComboBox->addItems(FEATURE_ITEMS);
    formLayout->addRow(new QLabel(QStringLiteral(u"Характеристика")), featureComboBox);

    unitComboBox = new QComboBox();
    unitComboBox->addItems(UNIT_ITEMS);
    formLayout->addRow((QStringLiteral(u"Единица измерения")), unitComboBox);

    descLineEdit = new QTextEdit();
    formLayout->addRow(new QLabel(QStringLiteral(u"Описание")), descLineEdit);

    QGroupBox* additionalGroup = new QGroupBox(QStringLiteral(u"Дополнительно"));
    QVBoxLayout* additionalLayout = new QVBoxLayout(additionalGroup);

    displayCheckBox = new QCheckBox(QStringLiteral(u"Отображать на схеме"));
    optimizableCheckBox = new QCheckBox(QStringLiteral(u"Доступен для оптимизации"));
    editedCheckBox = new QCheckBox(QStringLiteral(u"Запретить редактирование"));
    netlistedCheckBox = new QCheckBox(QStringLiteral(u"Не добавлять в netlist"));

    additionalLayout->addWidget(displayCheckBox);
    additionalLayout->addWidget(optimizableCheckBox);
    additionalLayout->addWidget(editedCheckBox);
    additionalLayout->addWidget(netlistedCheckBox);

    addButton = new QPushButton(QStringLiteral(u"Добавить"));
    
    parametersLayout->addWidget(formWidget);
    parametersLayout->addWidget(additionalGroup);

    parametersLayout->addWidget(addButton);

    parametersLayout->addStretch(1);

    mainLayout->addWidget(parametersGroup);


    mainLayout->setContentsMargins(5, 5, 5, 5);
   parametersGroup->setMinimumWidth(300);   
}

QString ParameterEditor::buildingLink(Parameters* parameter)
{
    QString link = parameter->feature + "." + parameter->type + (parameter->display ? ".D" : "")
        + (parameter->optimizable.value_or(false) ? ".O" : "")
        + (parameter->edited.value_or(false) ? ".E" : "")
        + (parameter->netlisted.value_or(false) ? ".N" : "");

    return link;
}