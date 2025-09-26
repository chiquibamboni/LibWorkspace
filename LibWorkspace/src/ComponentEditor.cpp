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

     QPushButton* selectIconBtn = new QPushButton(QStringLiteral(u"������� ������"));
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
     //// ��������� ����� �� ������
     //QObject::connect(selectIconBtn, &QPushButton::clicked, [&]() {
     //    ThumbSelectDialog dlg(nullptr, iconsPath);
     //    if (dlg.exec() == QDialog::Accepted) {
     //        QIcon icon = dlg.selectedIcon();

     //        // ��������� ����������� ������� ������
     //        QPixmap pixmap = icon.pixmap(currentIconDisplay->size());
     //        currentIconDisplay->setPixmap(pixmap);

     //        // ��������� �������� �����������
     //        QPixmap mainPixmap = icon.pixmap(iconDisplay->size());
     //        iconDisplay->setPixmap(mainPixmap);
     //    }
     //    });
}

void ComponentEditor::onItemDoubleClicked(QListWidgetItem* item)
{
    Parameters �urrentParam;
    QString searchName = item->text();

    for (const Parameters& param : *parameters) {
        if (param.name == searchName) {
            �urrentParam = param;
            break;
        }
    }
    parameterEditor->nameEdit->setText(�urrentParam.name);
    parameterEditor->typeComboBox->setCurrentText(�urrentParam.type);
    if (�urrentParam.rdefault.has_value())
    {
        parameterEditor->defaultValueLineEdit->setText(QString::number(�urrentParam.rdefault.value()));
    }
    else
    {
        parameterEditor->defaultValueLineEdit->setText(�urrentParam.sdefault);
    }
    parameterEditor->featureComboBox->setCurrentText(�urrentParam.feature);
    parameterEditor->unitComboBox->setCurrentText(�urrentParam.unit);
    parameterEditor->descLineEdit->setText(�urrentParam.desc);
    parameterEditor->displayCheckBox->setChecked(�urrentParam.display);
    parameterEditor->optimizableCheckBox->setChecked(�urrentParam.optimizable.value_or(false));
    parameterEditor->editedCheckBox->setChecked(�urrentParam.edited.value_or(false));
    parameterEditor->netlistedCheckBox->setChecked(�urrentParam.netlisted.value_or(false));

    QString link = parameterEditor->featureComboBox->currentText() + "." + parameterEditor->typeComboBox->currentText()
        + (parameterEditor->displayCheckBox->isChecked() ? ".D" : "")
        + (parameterEditor->optimizableCheckBox->isChecked() ? ".O" : "")
        + (parameterEditor->editedCheckBox->isChecked() ? ".E" : "")
        + (parameterEditor->netlistedCheckBox->isChecked() ? ".N" : "");
    parameterEditor->linkLabel->setText(link);
}