#pragma once

#include <QtWidgets/QMainWindow>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QGroupBox>
#include <QFormLayout>
#include <QCheckBox>
#include <QTextEdit>
#include <QLabel>
#include <QPushButton>
#include "Parameters.h"

class ParameterEditor : public QMainWindow
{
    Q_OBJECT

public:
    ParameterEditor(QWidget* parent = nullptr);
    ~ParameterEditor();
    QString buildingLink(Parameters* parameter);
    QLabel* linkLabel;
    QLineEdit* nameEdit;
    QComboBox* typeComboBox;
    QLineEdit* defaultValueLineEdit;
    QComboBox* featureComboBox;
    QComboBox* unitComboBox;
    QTextEdit* descLineEdit;
    QCheckBox* displayCheckBox;
    QCheckBox* optimizableCheckBox;
    QCheckBox* editedCheckBox;
    QCheckBox* netlistedCheckBox;
    QPushButton* addButton;
    void loadComboBoxData(QList<Parameters>* parameters);
private:
    void setupUi();
    //static const QStringList TYPE_ITEMS;
    static const QStringList FEATURE_ITEMS;
    static const QStringList UNIT_ITEMS;
};