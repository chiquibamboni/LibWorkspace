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

class NewComponentDialog : public QDialog
{
    Q_OBJECT

public:
    NewComponentDialog(QList<Library>* libraries, QList<Catalog>* catalogs, QList<Component>* components,
        QWidget* parent = nullptr);
    ~NewComponentDialog();

    QString getName() const { return currentName; }
    QString getDesc() const { return currentDesc; }
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
    void loadComboBoxData(QList<Library>* lib, QList<Catalog>* catalogs);
    void updateCategories(const QString& directoryName);
    //bool hasCyrillicCharacters(const QString& text);

    QLineEdit* nameEdit;
    QTextEdit* descEdit;
    QComboBox* libraryCombo;
    QComboBox* directoryCombo;
    QComboBox* categoryCombo;
    QPushButton* okButton;
    QPushButton* cancelButton;

    QList<Catalog>* catalogsList;
    QList <Component>* componentsList;

    QString currentName;
    QString currentDesc;
    QString currentLib;
    QString currentDirectory;
    QString currentCategory;
};