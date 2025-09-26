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
     auto hLayout = new QHBoxLayout();

     parameters = new QList<Parameters>();

     selectIconBtn = new QPushButton(QStringLiteral(u"Выбрать иконку"));
     hLayout->addWidget(selectIconBtn);
     /*currentIconDisplay = new QLabel();
     currentIconDisplay->setFixedSize(64, 64);
     currentIconDisplay->setFrameShape(QFrame::Box);*/
     mainLayout->addWidget(currentIconDisplay);

     mainLayout->addLayout(hLayout);

     iconDisplay = new QLabel();
     iconDisplay->setFixedSize(100, 100);
     mainLayout->addWidget(iconDisplay, 0, Qt::AlignHCenter);

     parameterEditor = new ParameterEditor();
     parametersList = new ParametersList(parameters);

     mainLayout->addWidget(parameterEditor);
     mainLayout->addWidget(parametersList);
}

void ComponentEditor::setupConnections()
{
    connect(parametersList, &QListWidget::itemDoubleClicked, this, &ComponentEditor::onItemDoubleClicked);
     // Обработка клика по кнопке
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