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

    QGroupBox* parametersGroup = new QGroupBox(QStringLiteral(u"���������"));
    QFormLayout* formLayout = new QFormLayout(parametersGroup);

    QLabel* linkLabel = new QLabel(QStringLiteral(u"������"));
    QLabel* linkLabelContent = new QLabel("");
    formLayout->addRow(linkLabel, linkLabelContent);

    QLabel* nameLabel = new QLabel(QStringLiteral(u"���"));
    QLineEdit* nameEdit = new QLineEdit();
    formLayout->addRow(nameLabel, nameEdit);

    QLabel* typeLabel = new QLabel(QStringLiteral(u"���"));
    QComboBox* typeCombo = new QComboBox();
    typeCombo->addItems(QStringList() << "Real" << "Integer" << "Complex" << "String" << "Link");
    formLayout->addRow(typeLabel, typeCombo);

    QLabel* defaultValueLabel = new QLabel(QStringLiteral(u"�������� �� ���������"));
    QLineEdit* defaultValueEdit = new QLineEdit();
    formLayout->addRow(defaultValueLabel, defaultValueEdit);

    QLabel* featureLabel = new QLabel(QStringLiteral(u"��������������"));
    QComboBox* featureCombo = new QComboBox();
    featureCombo->addItems(QStringList() << "No Unit" << "Angle" << "Capacitance" << "Conductance" << "Current");
    formLayout->addRow(featureLabel, featureCombo);

    QLabel* unitLabel = new QLabel(QStringLiteral(u"������� ���������"));
    QComboBox* unitCombo = new QComboBox();
    unitCombo->addItems(QStringList() << "Ohm" << "Hz" << "F");
    formLayout->addRow(unitLabel, unitCombo);

    QLabel* descriptionLabel = new QLabel(QStringLiteral(u"��������"));
    QLineEdit* descriptionEdit = new QLineEdit();
    formLayout->addRow(descriptionLabel, descriptionEdit);

    // ��������� ������ � ������� layout
    mainLayout->addWidget(parametersGroup);

    // ������������� ������� � ����������
    mainLayout->setContentsMargins(10, 10, 10, 10);
    parametersGroup->setMinimumWidth(400);
}