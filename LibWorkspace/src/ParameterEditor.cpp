#include "ParameterEditor.h"
#include <qheaderview.h>
#include <QSplitter>
#include <QString>
#include <QMessageBox>
#include <QMenu>

#include <QLabel>

const QStringList ParameterEditor::FEATURE_ITEMS = {
    "No Unit", "Angle", "Capacitance", "Conductance", "Current",
    "Datarate","dB", "Distance", "Frequency", "Inductance", "Length",
    "Power", "Resistance", "String", "Temperature", "Time", "Voltage"
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
    formLayout->addRow(new QLabel(QStringLiteral(u"Тип")), typeComboBox);
    typeComboBox->setEditable(true);

    formLayout->addRow(new QLabel(""), new QLabel(QStringLiteral(u"Пример: 12.34e+6")));

    defaultValueLineEdit = new QLineEdit();
    formLayout->addRow(new QLabel(QStringLiteral(u"Значение по умолчанию")), defaultValueLineEdit);

    featureComboBox = new QComboBox();
    featureComboBox->addItems(FEATURE_ITEMS);
    formLayout->addRow(new QLabel(QStringLiteral(u"Характеристика")), featureComboBox);
    featureComboBox->setEditable(true);

    unitComboBox = new QComboBox();
    formLayout->addRow((QStringLiteral(u"Единица измерения")), unitComboBox);
    unitComboBox->setEditable(true);

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

void ParameterEditor::loadComboBoxData(QList<Parameters>* parameters)
{
    QSet<QString> typeItems = {};
    QSet<QString> unitItems = {};

    for (auto& param : *parameters)
    {
        if (!param.type.isEmpty())
            typeItems.insert(param.type);
        if (!param.unit.isEmpty())
            unitItems.insert(param.unit);
    }

    unitComboBox->clear();
    typeComboBox->clear();

    unitComboBox->addItems(unitItems.values());
    typeComboBox->addItems(typeItems.values());
}

void ParameterEditor::clearEditor()
{
    nameEdit->clear();
    typeComboBox->setCurrentText("");
    defaultValueLineEdit->clear();
    featureComboBox->setCurrentText("");
    unitComboBox->setCurrentText("");
    descLineEdit->clear();
    displayCheckBox->setChecked(false);
    optimizableCheckBox->setChecked(false);
    editedCheckBox->setChecked(false);
    netlistedCheckBox->setChecked(false);
}

QString ParameterEditor::buildingLink(Parameters* parameter)
{
    QString link = parameter->feature + "." + parameter->type + (parameter->display ? ".D" : "")
        + (parameter->optimizable.value_or(false) ? ".O" : "")
        + (parameter->edited.value_or(false) ? ".E" : "")
        + (parameter->netlisted.value_or(false) ? ".N" : "");

    return link;
}