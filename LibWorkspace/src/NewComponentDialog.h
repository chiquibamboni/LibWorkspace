#pragma once

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QDialogButtonBox>
#include "Library.h"

class NewComponentDialog : public QDialog
{
    Q_OBJECT

public:
    NewComponentDialog(QList<Library>* libraries, QList<Catalog>* catalogs,
        Library* libForCat, QWidget* parent = nullptr);
    ~NewComponentDialog();

    QString getName() const { return currentName; }
    QString getLibrary() const { return currentLib; }
    QString getDirectory() const { return currentDirectory; }
    QString getCategory() const { return currentCategory; }

private slots:
    void onAccept();
    void validateForm();
    void onDirectoryChanged();

private:
    void setupUI();
    void setupConnections();
    void loadComboBoxData(QList<Library>* lib, QList<Catalog>* catalogs, Library* libForCat);
    void updateCategories(const QString& directoryName);

    QLineEdit* nameEdit;
    QComboBox* libraryCombo;
    QComboBox* directoryCombo;
    QComboBox* categoryCombo;
    QPushButton* okButton;
    QPushButton* cancelButton;

    QList<Catalog>* catalogsList;

    QString currentName;
    QString currentLib;
    QString currentDirectory;
    QString currentCategory;
};