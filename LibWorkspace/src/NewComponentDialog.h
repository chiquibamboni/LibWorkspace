#pragma once

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QDialogButtonBox>

class NewComponentDialog : public QDialog
{
    Q_OBJECT

public:
    NewComponentDialog(QWidget* parent = nullptr);
    ~NewComponentDialog();

    QString getName() const { return currentName; }
    QString getLibrary() const { return currentLib; }
    QString getDirectory() const { return currentDirectory; }
    QString getCategory() const { return currentCategory; }

private slots:
    void onAccept();
    void validateForm();

private:
    void setupUI();
    void setupConnections();
    void loadComboBoxData();

    QLineEdit* nameEdit;
    QComboBox* libraryCombo;
    QComboBox* directoryCombo;
    QComboBox* categoryCombo;
    QPushButton* okButton;
    QPushButton* cancelButton;

    QString currentName;
    QString currentLib;
    QString currentDirectory;
    QString currentCategory;
};
