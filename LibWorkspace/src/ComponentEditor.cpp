#include "ComponentEditor.h"
#include <qheaderview.h>
#include <QSplitter>
#include <QString>
#include <QMessageBox>
#include <QMenu>


ComponentEditor::ComponentEditor(QList<Library>* libraries, QList<Catalog>* catalogs, QList<Parameters>* parameters, QWidget* parent)
    : QMainWindow(parent)
{
    librariesList = libraries;
    catalogsList = catalogs;
    parametersList = parameters;
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

    currentParameter = new Parameters();

    QVBoxLayout* imageLayout = new QVBoxLayout();

    selectIconBtn = new QPushButton(QStringLiteral(u"Выбрать иконку"));
    iconDisplay = new QLabel();
    iconDisplay->setFixedSize(100, 100);
    iconDisplay->setFrameShape(QFrame::Box);
    iconDisplay->setAlignment(Qt::AlignCenter);

    imageLayout->addStretch();
    modelComboBox = new QComboBox();
    modelsList = new QStringList();

    imageLayout->addWidget(modelComboBox);

    imageLayout->addStretch();

    imageLayout->addWidget(selectIconBtn);
    imageLayout->addWidget(iconDisplay, 0, Qt::AlignHCenter);

    imageLayout->addSpacing(10);

    parameterEditor = new ParameterEditor();
    parametersListWidget = new ParametersList(parametersList);

    QHBoxLayout* leftLayout = new QHBoxLayout();

    leftLayout->addLayout(imageLayout);
    leftLayout->addWidget(parameterEditor);

    leftLayout->setStretchFactor(imageLayout, 1);
    leftLayout->setStretchFactor(parameterEditor, 3);

    QVBoxLayout* VLayout = new QVBoxLayout();

    VLayout->addLayout(leftLayout);
    QLabel* label = new QLabel(QStringLiteral(u"Модель"));
    label->setAlignment(Qt::AlignCenter);
    VLayout->addWidget(label);

    QWidget* buttonsContainer = new QWidget();
    QHBoxLayout* containerLayout = new QHBoxLayout(buttonsContainer);

    QHBoxLayout* buttonsGroup = new QHBoxLayout();

    QVBoxLayout* leftColumn = new QVBoxLayout();
    leftColumn->addWidget(new QPushButton("SCS"));
    leftColumn->addWidget(new QPushButton("QDS"));

    QVBoxLayout* rightColumn = new QVBoxLayout();
    rightColumn->addWidget(new QPushButton("VA"));
    rightColumn->addWidget(new QPushButton("STD"));

    buttonsGroup->addLayout(leftColumn);
    buttonsGroup->addSpacing(30);
    buttonsGroup->addLayout(rightColumn);

    containerLayout->addStretch();
    containerLayout->addLayout(buttonsGroup);
    containerLayout->addStretch();

    VLayout->addWidget(buttonsContainer);

    mainLayout->addLayout(VLayout);
    mainLayout->addWidget(parametersListWidget);
}

void ComponentEditor::setupConnections()
{
    connect(parametersListWidget, &QListWidget::itemDoubleClicked, this, &ComponentEditor::onItemDoubleClicked);
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

    connect(selectIconBtn, &QPushButton::clicked, [&]() {
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

    for (const Parameters& param : *parametersList) {
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