#pragma once

#include <QDialog>
#include <QLineEdit>
#include <QTextEdit>
#include <QComboBox>
#include <QPushButton>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QDialogButtonBox>
#include <QToolTip>
#include "FillFromJsons.h"
#include "Library.h"
#include "ValueValidator.h"

class NewComponentDialog : public QDialog
{
    Q_OBJECT

public:
    NewComponentDialog(QList<Library>* libraries, QList<Catalog>* catalogs,
        QList<Component>* components, QList<Parameters>* curParameters,
        QString thumbName, QString ugoName, QWidget* parent = nullptr);
    ~NewComponentDialog();

    QString getName() const { return currentName; }
    QString getDesc() const { return currentDesc; }
    QString getLibrary() const { return currentLib; }
    QString getDirectory() const { return currentDirectory; }
    QString getCategory() const { return currentCategory; }
    void loadCurrentCompData(Component curComp, Catalog curCat);

private slots:
    void onAccept();
    void validateForm();
    void onDirectoryChanged();

private:
    void setupUI();
    void setupConnections();
    void loadComboBoxData(QList<Library>* lib, QList<Catalog>* catalogs);
    void updateCategories(const QString& directoryName);
    void createNewComponent(QString name, QString library, QString directory, QString category, QString desc);
    void editComponent(QString currentName, QString currentDesc);


    QLineEdit* nameEdit;
    QTextEdit* descEdit;
    QComboBox* libraryCombo;
    QComboBox* directoryCombo;
    QComboBox* categoryCombo;
    QPushButton* okButton;
    QPushButton* cancelButton;

    QList<Library>*librariesList;
    QList<Catalog>* catalogsList;
    QList <Component>* componentsList;
    QList <Parameters>* curParametersList;

    QString newThumbName;
    QString newUgoName;

    QString currentName;
    QString currentDesc;
    QString currentLib;
    QString currentDirectory;
    QString currentCategory;
};