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

     QPushButton* selectIconBtn = new QPushButton(QStringLiteral(u"Âûáðàòü èêîíêó"));
     hLayout->addWidget(selectIconBtn);
     QLabel* currentIconDisplay = new QLabel();
     currentIconDisplay->setFixedSize(64, 64);
     currentIconDisplay->setFrameShape(QFrame::Box);
     mainLayout->addWidget(currentIconDisplay);

     mainLayout->addLayout(hLayout);

     QLabel* iconDisplay = new QLabel();
     iconDisplay->setFixedSize(100, 100);
     mainLayout->addWidget(iconDisplay, 0, Qt::AlignHCenter);

     QString iconsPath = "./icons";

     parameterEditor = new ParameterEditor();
     parametersList = new ParametersList(parameters);

     mainLayout->addWidget(parameterEditor);
     mainLayout->addWidget(parametersList);
}

void ComponentEditor::setupConnections()
{
    connect(parametersList, &QListWidget::itemDoubleClicked, this, &ComponentEditor::onItemDoubleClicked);
     //// Îáðàáîòêà êëèêà ïî êíîïêå
     //QObject::connect(selectIconBtn, &QPushButton::clicked, [&]() {
     //    ThumbSelectDialog dlg(nullptr, iconsPath);
     //    if (dlg.exec() == QDialog::Accepted) {
     //        QIcon icon = dlg.selectedIcon();

     //        // Îáíîâëÿåì îòîáðàæåíèå òåêóùåé èêîíêè
     //        QPixmap pixmap = icon.pixmap(currentIconDisplay->size());
     //        currentIconDisplay->setPixmap(pixmap);

     //        // Îáíîâëÿåì îñíîâíîå îòîáðàæåíèå
     //        QPixmap mainPixmap = icon.pixmap(iconDisplay->size());
     //        iconDisplay->setPixmap(mainPixmap);
     //    }
     //    });
}

void ComponentEditor::onItemDoubleClicked(QListWidgetItem* item)
{
    Parameters ñurrentParam;
    QString searchName = item->text();

    for (const Parameters& param : *parameters) {
        if (param.name == searchName) {
            ñurrentParam = param;
            break;
        }
    }
    parameterEditor->nameEdit->setText(ñurrentParam.name);
    parameterEditor->typeComboBox->setCurrentText(ñurrentParam.type);
    if (ñurrentParam.rdefault.has_value())
    {
        parameterEditor->defaultValueLineEdit->setText(QString::number(ñurrentParam.rdefault.value()));
    }
    else
    {
        parameterEditor->defaultValueLineEdit->setText(ñurrentParam.sdefault);
    }
    parameterEditor->featureComboBox->setCurrentText(ñurrentParam.feature);
    parameterEditor->unitComboBox->setCurrentText(ñurrentParam.unit);
    parameterEditor->descLineEdit->setText(ñurrentParam.desc);
    parameterEditor->displayCheckBox->setChecked(ñurrentParam.display);
    parameterEditor->optimizableCheckBox->setChecked(ñurrentParam.optimizable.value_or(false));
    parameterEditor->editedCheckBox->setChecked(ñurrentParam.edited.value_or(false));
    parameterEditor->netlistedCheckBox->setChecked(ñurrentParam.netlisted.value_or(false));

    QString link = parameterEditor->featureComboBox->currentText() + "." + parameterEditor->typeComboBox->currentText()
        + (parameterEditor->displayCheckBox->isChecked() ? ".D" : "")
        + (parameterEditor->optimizableCheckBox->isChecked() ? ".O" : "")
        + (parameterEditor->editedCheckBox->isChecked() ? ".E" : "")
        + (parameterEditor->netlistedCheckBox->isChecked() ? ".N" : "");
    parameterEditor->linkLabel->setText(link);
}