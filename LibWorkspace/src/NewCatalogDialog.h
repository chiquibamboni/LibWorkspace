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
#include "Library.h"
#include "ValueValidator.h"
#include "ThumbSelectDialog.h"

class NewCatalogDialog : public QDialog
{
    Q_OBJECT

public:
    NewCatalogDialog(QList<Library>* libraries, QList<Catalog>* catalogs, QList<QString> iconPaths, QWidget* parent = nullptr);
    ~NewCatalogDialog();

    QString getName() const { return currentName; }
    QString getLibrary() const { return currentLib; }
    QString getDirectory() const { return currentDirectory; } 
    QString* newThumbName;

private slots:
    void onAccept();
    void validateForm();
    void openThumbDialog();

private:
    void setupUI();
    void setupConnections();
    void loadComboBoxData(QList<Library>* lib, QList<Catalog>* catalogs);

    QList<QString> iconPaths;


    QLineEdit* nameEdit;
    QComboBox* libraryCombo;
    QComboBox* directoryCombo;
    QPushButton* okButton;
    QPushButton* cancelButton;
    QPushButton* selectIconBtn;
    QLabel* iconDisplay;

    QList<Catalog>* catalogsList;

    QString currentName;
    QString currentLib;
    QString currentDirectory;
};