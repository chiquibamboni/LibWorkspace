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
    tabs = new QList<QString>();
    newThumbName = new QString();
    setupUi();
    setupConnections();
}

ComponentEditor::~ComponentEditor()
{
    delete librariesList;
    delete catalogsList;
    delete parametersList;
    delete tabs;
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

    ugoTabs = new UgoTabs();
    ugoTabs->setTab("ANSI");
    ugoTabs->setTab(QStringLiteral(u"ГОСТ"));

    modelsComboBox = new QComboBox();
    modelsComboBox->setPlaceholderText(QStringLiteral(u"Выберите модель"));

    imageLayout->addWidget(ugoTabs);
    imageLayout->addWidget(modelsComboBox);

    imageLayout->addStretch();

    imageLayout->addWidget(selectIconBtn);
    imageLayout->addWidget(iconDisplay, 0, Qt::AlignHCenter);

    imageLayout->addSpacing(10);

    parameterEditor = new ParameterEditor();
    parametersListWidget = new ParametersList();
    parametersListWidget->parameters = parametersList;
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

    QVBoxLayout* listLayout = new QVBoxLayout();
    
    currentParameterListWidget = new ParametersList;

    listLayout->addWidget(parametersListWidget);
    listLayout->addWidget(currentParameterListWidget);

    listLayout->setStretchFactor(parametersListWidget, 2);
    listLayout->setStretchFactor(currentParameterListWidget, 1);

    delButton = new QPushButton(QStringLiteral(u"Удалить"));
    listLayout->addWidget(delButton);

    mainLayout->addLayout(listLayout);
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
    connect(modelsComboBox, &QComboBox::currentTextChanged, this, &ComponentEditor::selectModel);
    connect(selectIconBtn, &QPushButton::clicked, [&]() {
        ThumbSelectDialog dlg(iconPaths);
        if (dlg.exec() == QDialog::Accepted) {
            QIcon icon = dlg.selectedIcon();
            newThumbName = dlg.newThumbName;
            // Обновляем основное отображение
            QPixmap mainPixmap = icon.pixmap(iconDisplay->size());
            iconDisplay->setPixmap(mainPixmap);
        }
        });
    connect(parameterEditor->addButton, &QPushButton::clicked, this, &ComponentEditor::addNewParameter);
    connect(currentParameterListWidget, &QListWidget::itemDoubleClicked, this, &ComponentEditor::onItemDoubleClicked);
    connect(delButton, &QPushButton::clicked, this, &ComponentEditor::delParameter);
}

void ComponentEditor::onItemDoubleClicked(QListWidgetItem* item)
{
    QString searchName = item->text();
    updateParameterEditor(searchName);
}

void ComponentEditor::onParameterChanged()
{
    currentParameter->name = parameterEditor->nameEdit->text();
    currentParameter->type = parameterEditor->typeComboBox->currentText();

    QString defaultText = parameterEditor->defaultValueLineEdit->text();
    bool ok;
    double numericValue = defaultText.toDouble(&ok);
    if (ok && !defaultText.isEmpty()) {
        currentParameter->pdefault = QVariant(numericValue); // число
    }
    //для массива
    /*else if (jsonObj["default"].is_array()) {
        auto arr = jsonObj["default"].get<std::vector<double>>();
        QVector<double> qvec(arr.begin(), arr.end());
        pdefault = QVariant(qvec);
    }*/
    else {
        currentParameter->pdefault = QVariant(defaultText);
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

void ComponentEditor::addNewParameter()
{
    currentParameterListWidget->ParametersList::insertItem(*currentParameter);
}

void ComponentEditor::delParameter()
{
    QListWidgetItem* selectedItem = currentParameterListWidget->currentItem();

    if (selectedItem) {
        for (int i = 0; i < currentParameterListWidget->parameters->size(); ++i) {
            Parameters par = currentParameterListWidget->parameters->at(i);
            if (par.name == selectedItem->text()) {
                currentParameterListWidget->parameters->removeAt(i);
                break;
            }
        }
        delete currentParameterListWidget->takeItem(currentParameterListWidget->row(selectedItem));
    }
    else {
        QMessageBox::information(this, QStringLiteral(u"Удаление"),
            QStringLiteral(u"Пожалуйста, выберите элемент для удаления"));
    }

}

void ComponentEditor::updateParameterEditor(QString searchName) {
    Parameters сurrentParam;

    for (const Parameters& param : *parametersList) {
        if (param.name == searchName) {
            сurrentParam = param;
            break;
        }
    }
    parameterEditor->nameEdit->setText(сurrentParam.name);
    parameterEditor->typeComboBox->setCurrentText(сurrentParam.type);

    if (сurrentParam.pdefault.has_value()) {
        const QVariant& val = сurrentParam.pdefault.value();

        if (val.canConvert<double>()) {
            parameterEditor->defaultValueLineEdit->setText(QString::number(val.toDouble()));
        }
        else if (val.canConvert<QString>()) {
            parameterEditor->defaultValueLineEdit->setText(val.toString());
        }
        else {
            // Для других типов или массива — можете задать пустой или обработать иначе
            parameterEditor->defaultValueLineEdit->setText("");
        }
    }
    else {
        parameterEditor->defaultValueLineEdit->setText("");
    }

    parameterEditor->featureComboBox->setCurrentText(сurrentParam.feature);
    parameterEditor->unitComboBox->setCurrentText(сurrentParam.unit);
    parameterEditor->descLineEdit->setText(сurrentParam.desc);
    parameterEditor->displayCheckBox->setChecked(сurrentParam.display);
    parameterEditor->optimizableCheckBox->setChecked(сurrentParam.optimizable.value_or(false));
    parameterEditor->editedCheckBox->setChecked(сurrentParam.edited.value_or(false));
    parameterEditor->netlistedCheckBox->setChecked(сurrentParam.netlisted.value_or(false));
}

void ComponentEditor::clearUgo()
{
    modelsComboBox->setPlaceholderText(QStringLiteral(u"Выберите модель"));
    ugoTabs->clearTabImage("ANSI");
    ugoTabs->clearTabImage(QStringLiteral(u"ГОСТ"));
}

void ComponentEditor::clearIcons()
{
    iconDisplay->clear();
}

void ComponentEditor::clearWidget()
{
    clearUgo();
    clearIcons();
    modelsComboBox->clear();
    librariesList->clear();
    catalogsList->clear();
    parametersList->clear();
    parametersListWidget->clearItems();
    currentParameterListWidget->clearItems();
}

void ComponentEditor::selectModel(const QString& text)
{
    for (auto& symbolsPath : symbolPaths)
    {
        QFileInfo info(symbolsPath);
        QString fullPath = symbolsPath + "/" + text + ".svg";
        if (QFile::exists(fullPath))
        {
            if (info.fileName() == "ansi")
            {
                ugoTabs->setTabImage("ANSI", fullPath);
            }
            if (info.fileName() == "gost")
            {
                ugoTabs->setTabImage(QStringLiteral(u"ГОСТ"), fullPath);
            }
            return;
        }
        else
        {
            ugoTabs->clearTabImage("ANSI");
            ugoTabs->clearTabImage(QStringLiteral(u"ГОСТ"));
        }
    }
}