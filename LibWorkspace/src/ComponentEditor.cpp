#include "ComponentEditor.h"
#include <qheaderview.h>
#include <QSplitter>
#include <QString>
#include <QMessageBox>
#include <QMenu>


ComponentEditor::ComponentEditor(QList<Library>* libraries, QList<Catalog>* catalogs, QList<Component>* components, QList<Parameters>* parameters, QWidget* parent)
    : QMainWindow(parent)
{
    librariesList = libraries;
    catalogsList = catalogs;
    componentsList = components;
    parametersList = parameters;
    tabs = new QList<QString>();
    newThumbName = new QString();
    //newThumbName = NULL;
    setupUi();
    setupConnections();
}

ComponentEditor::~ComponentEditor()
{
    delete librariesList;
    delete catalogsList;
    delete componentsList;
    //delete parametersList;
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
    modelsComboBox->addItem("");

    imageLayout->addStretch();

    imageLayout->addWidget(selectIconBtn);
    imageLayout->addWidget(iconDisplay, 0, Qt::AlignHCenter);

    imageLayout->addSpacing(10);
  
    parametersListWidget = new ParametersList();
    parametersListWidget->parameters = parametersList;
    QHBoxLayout* leftLayout = new QHBoxLayout();
    parameterEditor = new ParameterEditor();
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

    listLayout->addWidget(new QLabel(QStringLiteral(u"Список параметров")));
    listLayout->addWidget(parametersListWidget);
    listLayout->addWidget(new QLabel(QStringLiteral(u"Список текущих параметров компонента")));
    listLayout->addWidget(currentParameterListWidget);

    listLayout->setStretchFactor(parametersListWidget, 2);
    listLayout->setStretchFactor(currentParameterListWidget, 1);

    delButton = new QPushButton(QStringLiteral(u"Удалить"));
    listLayout->addWidget(delButton);

    mainLayout->addLayout(listLayout);

    mainLayout->setStretchFactor(VLayout, 2);
    mainLayout->setStretchFactor(listLayout, 1);
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
    connect(currentParameterListWidget, &QListWidget::itemDoubleClicked, this, &ComponentEditor::onItemDoubleClickedCurList);
    connect(delButton, &QPushButton::clicked, this, &ComponentEditor::delParameter);
}

void ComponentEditor::onItemDoubleClickedCurList(QListWidgetItem* item)
{
    QString searchName = item->text();
    Parameters param = serchParam(searchName, *currentParameterListWidget->parameters);
    updateParameterEditor(param);
}

void ComponentEditor::onItemDoubleClicked(QListWidgetItem* item)
{
    QString searchName = item->text();
    Parameters param = serchParam(searchName, *parametersList);
    updateParameterEditor(param);
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
    //else if (jsonObj["default"].is_array()) {
    //    auto arr = jsonObj["default"].get<std::vector<double>>();
    //    QVector<double> qvec(arr.begin(), arr.end());
    //    pdefault = QVariant(qvec);
    //}
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
    if (currentParameter->name.isEmpty()) {
        QMessageBox::information(this, QStringLiteral(u"Ошибка"),
            QStringLiteral(u"Имя не должно быть пустым"));
        return;
    }
    bool descValid = !ValueValidator::hasCyrillicCharacters(currentParameter->desc);
    bool nameValid = !ValueValidator::hasCyrillicCharacters(currentParameter->name);
    bool typeValid = !ValueValidator::hasCyrillicCharacters(currentParameter->type);
    bool featureValid = !ValueValidator::hasCyrillicCharacters(currentParameter->feature);
    bool unitValid = !ValueValidator::hasCyrillicCharacters(currentParameter->unit);
    bool defaultValueValid = true;
    if (currentParameter->pdefault.value().canConvert<QString>())
    {
        defaultValueValid = !ValueValidator::hasCyrillicCharacters(currentParameter->pdefault.value().toString());
    }

    if (!descValid || !nameValid || !typeValid || !featureValid || !unitValid  || !defaultValueValid) {
        QMessageBox::information(this, QStringLiteral(u"Ошибка"),
            QStringLiteral(u"Допустимы только латинские буквы"));
        return;
    }

    if (currentParameterListWidget->containsParam(*currentParameter))
    {
        bool edit = currentParameterListWidget->ParametersList::editParamList(*currentParameter);
        if (edit)
        {
            for (int i = 0; i < currentParameterListWidget->count(); ++i) {
                QListWidgetItem* item = currentParameterListWidget->item(i);
                if (item->text() == currentParameter->name) {
                    currentParameterListWidget->setCurrentItem(item);
                    break;
                }
            }
            currentParameterListWidget->delCurParamm(currentParameterListWidget->currentItem());
            currentParameterListWidget->ParametersList::insertItem(*currentParameter);
            parameterEditor->clearEditor();
            return;
        }
        return;
    }

    currentParameterListWidget->ParametersList::insertItem(*currentParameter);
    parameterEditor->clearEditor();
}

void ComponentEditor::delParameter()
{
    QListWidgetItem* selectedItem = currentParameterListWidget->currentItem();

    //if (selectedItem) {
    //    for (int i = 0; i < currentParameterListWidget->parameters->size(); ++i) {
    //        Parameters par = currentParameterListWidget->parameters->at(i);
    //        if (par.name == selectedItem->text()) {
    //            currentParameterListWidget->parameters->removeAt(i);
    //            break;
    //        }
    //    }
    //    delete currentParameterListWidget->takeItem(currentParameterListWidget->row(selectedItem));
    //}
    //else {
    //    QMessageBox::information(this, QStringLiteral(u"Удаление"),
    //        QStringLiteral(u"Пожалуйста, выберите элемент для удаления"));
    //}

    currentParameterListWidget->delCurParamm(selectedItem);
}

Parameters ComponentEditor::serchParam(QString searchName, QList<Parameters> curParametersList)
{
    Parameters сurrentParam;

    for (const Parameters& param : curParametersList) {
        if (param.name == searchName) {
            сurrentParam = param;
            break;
        }
    }

    return сurrentParam;
}

void ComponentEditor::updateParameterEditor(Parameters сurrentParam) {
   /* Parameters сurrentParam;

    for (const Parameters& param : *parametersList) {
        if (param.name == searchName) {
            сurrentParam = param;
            break;
        }
    }*/
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
    //delete newThumbName;
    newThumbName = new QString();
}

void ComponentEditor::selectModel(const QString& text)
{
    bool modelFound = false;

    for (auto& component : *componentsList)
    {
        if (component.ugo.model == text)
        {

            modelsComboBox->setCurrentText(component.ugo.model);
            if (!component.ugo.ansiUgoSymbol.isNull())
            {
                ugoTabs->setTabImage("ANSI", component.ugo.ansiSymbolPath);
                modelFound = true;
            }
            if (!component.ugo.gostUgoSymbol.isNull())
            {
                ugoTabs->setTabImage(QStringLiteral(u"ГОСТ"), component.ugo.gostSymbolPath);
                modelFound = true;
            }

            modelsComboBox->setCurrentText(text);
            break;
        }
    }

    if (!modelFound)
    {
        clearUgo();
        //modelsComboBox->setCurrentText("");
    }
}