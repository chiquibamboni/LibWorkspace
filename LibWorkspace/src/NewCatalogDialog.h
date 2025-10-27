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

class NewCatalogDialog : public QDialog
{
    Q_OBJECT

public:
    NewCatalogDialog(QList<Library>* libraries, QList<Catalog>* catalogs, QWidget* parent = nullptr);
    ~NewCatalogDialog();

    QString getName() const { return currentName; }
    QString getLibrary() const { return currentLib; }
    QString getDirectory() const { return currentDirectory; }

private slots:
    void onAccept();
    void validateForm();

private:
    void setupUI();
    void setupConnections();
    void loadComboBoxData(QList<Library>* lib, QList<Catalog>* catalogs);

    QLineEdit* nameEdit;
    QComboBox* libraryCombo;
    QComboBox* directoryCombo;
    QPushButton* okButton;
    QPushButton* cancelButton;

    QList<Catalog>* catalogsList;

    QString currentName;
    QString currentLib;
    QString currentDirectory;
};