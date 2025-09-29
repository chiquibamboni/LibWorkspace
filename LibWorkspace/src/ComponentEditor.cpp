#include "ComponentEditor.h"
#include <qheaderview.h>
#include <QSplitter>
#include <QString>
#include <QMessageBox>
#include <QMenu>


ComponentEditor::ComponentEditor(QWidget* parent)
    : QMainWindow(parent)
{
    setupUi();
    setupConnections();
}

ComponentEditor::~ComponentEditor()
{
}

void ComponentEditor::setupUi()
{
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QHBoxLayout* mainLayout = new QHBoxLayout(centralWidget);

    parameters = new QList<Parameters>();
    currentParameter = new Parameters();

    QVBoxLayout* leftLayout = new QVBoxLayout();

    selectIconBtn = new QPushButton(QStringLiteral(u"Выбрать иконку"));
    iconDisplay = new QLabel();
    iconDisplay->setFixedSize(100, 100);
    iconDisplay->setFrameShape(QFrame::Box);
    iconDisplay->setAlignment(Qt::AlignCenter);

    leftLayout->addStretch();

    leftLayout->addWidget(selectIconBtn);
    leftLayout->addWidget(iconDisplay, 0, Qt::AlignHCenter);

    leftLayout->addSpacing(10);

    parameterEditor = new ParameterEditor();
    parametersList = new ParametersList(parameters);

    mainLayout->addLayout(leftLayout);
    mainLayout->addWidget(parameterEditor);
    mainLayout->addWidget(parametersList);

    mainLayout->setStretchFactor(leftLayout, 1);
    mainLayout->setStretchFactor(parameterEditor, 4);
    mainLayout->setStretchFactor(parametersList, 2);
}

void ComponentEditor::setupConnections()
{
    connect(parametersList, &QListWidget::itemDoubleClicked, this, &ComponentEditor::onItemDoubleClicked);
    connect(parameterEditor->nameEdit, &QLineEdit::textChanged, this, &ComponentEditor::onParameterChanged);
    connect(parameterEditor->typeComboBox, &QComboBox::currentTextChanged, this, &ComponentEditor::onParameterChanged);
    connect(parameterEditor->defaultValueLineEdit, &QLineEdit::textChanged, this, &ComponentEditor::onParameterChanged);
    connect(parameterEditor->featureComboBox, &QComboBox::currentTextChanged, this, &ComponentEditor::onParameterChanged);
    connect(parameterEditor->unitComboBox, &QComboBox::currentTextChanged, this, &ComponentEditor::onParameterChanged);
    connect(parameterEditor->descLineEdit, &QTextEdit::textChanged, this, &ComponentEditor::onParameterChanged);
    connect(parameterEditor->displayCheckBox, &QCheckBox::stateChanged, this, &ComponentEditor::onParameterChanged);
    connect(parameterEditor->optimizableCheckBox, &QCheckBox::stateChanged, this, &ComponentEditor::onParameterChanged);
    connect(parameterEditor->editedCheckBox, &QCheckBox::stateChanged, this, &ComponentEditor::onParameterChanged);
    connect(parameterEditor->netlistedCheckBox, &QCheckBox::stateChanged, this, &ComponentEditor::onParameterChanged);

     QObject::connect(selectIconBtn, &QPushButton::clicked, [&]() {
         ThumbSelectDialog dlg(iconsPath);
         if (dlg.exec() == QDialog::Accepted) {
             QIcon icon = dlg.selectedIcon();

             //// Обновляем отображение иконки
             //QPixmap pixmap = icon.pixmap(currentIconDisplay->size());
             //currentIconDisplay->setPixmap(pixmap);

             // Обновляем основное отображение
             QPixmap mainPixmap = icon.pixmap(iconDisplay->size());
             iconDisplay->setPixmap(mainPixmap);
         }
         });
}

void ComponentEditor::onItemDoubleClicked(QListWidgetItem* item)
{
    Parameters сurrentParam;
    QString searchName = item->text();

    for (const Parameters& param : *parameters) {
        if (param.name == searchName) {
            сurrentParam = param;
            break;
        }
    }
    parameterEditor->nameEdit->setText(сurrentParam.name);
    parameterEditor->typeComboBox->setCurrentText(сurrentParam.type);
    if (сurrentParam.rdefault.has_value())
    {
        parameterEditor->defaultValueLineEdit->setText(QString::number(сurrentParam.rdefault.value()));
    }
    else
    {
        parameterEditor->defaultValueLineEdit->setText(сurrentParam.sdefault);
    }
    parameterEditor->featureComboBox->setCurrentText(сurrentParam.feature);
    parameterEditor->unitComboBox->setCurrentText(сurrentParam.unit);
    parameterEditor->descLineEdit->setText(сurrentParam.desc);
    parameterEditor->displayCheckBox->setChecked(сurrentParam.display);
    parameterEditor->optimizableCheckBox->setChecked(сurrentParam.optimizable.value_or(false));
    parameterEditor->editedCheckBox->setChecked(сurrentParam.edited.value_or(false));
    parameterEditor->netlistedCheckBox->setChecked(сurrentParam.netlisted.value_or(false));

    QString link = parameterEditor->featureComboBox->currentText() + "." + parameterEditor->typeComboBox->currentText()
        + (parameterEditor->displayCheckBox->isChecked() ? ".D" : "")
        + (parameterEditor->optimizableCheckBox->isChecked() ? ".O" : "")
        + (parameterEditor->editedCheckBox->isChecked() ? ".E" : "")
        + (parameterEditor->netlistedCheckBox->isChecked() ? ".N" : "");
    parameterEditor->linkLabel->setText(link);
}

void ComponentEditor::onParameterChanged()
{
    currentParameter->name = parameterEditor->nameEdit->text();
    currentParameter->type = parameterEditor->typeComboBox->currentText();

    QString defaultText = parameterEditor->defaultValueLineEdit->text();
    bool ok;
    double numericValue = defaultText.toDouble(&ok);
    if (ok && !defaultText.isEmpty()) {
        currentParameter->rdefault = numericValue;
        currentParameter->sdefault = QString();
    }
    else {
        currentParameter->rdefault = std::nullopt;
        currentParameter->sdefault = defaultText;
    }

    currentParameter->feature = parameterEditor->featureComboBox->currentText();
    currentParameter->unit = parameterEditor->unitComboBox->currentText();
    currentParameter->desc = parameterEditor->descLineEdit->toPlainText();
    currentParameter->display = parameterEditor->displayCheckBox->isChecked();
    currentParameter->optimizable = parameterEditor->optimizableCheckBox->isChecked();
    currentParameter->edited = parameterEditor->editedCheckBox->isChecked();
    currentParameter->netlisted = parameterEditor->netlistedCheckBox->isChecked();

    updateParameterLink();
}

void ComponentEditor::updateParameterLink()
{
    QString link = parameterEditor->buildingLink(currentParameter);

    parameterEditor->linkLabel->setText(link);
}