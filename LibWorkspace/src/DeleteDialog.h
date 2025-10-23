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
#include "FillFromJsons.h"

class DeleteDialog : public QDialog
{
    Q_OBJECT

public:
    DeleteDialog(Library* currentLibrary, Catalog* currentCatalog, Component* currentComponent, QWidget* parent = nullptr);
    ~DeleteDialog();

private:
    void setupUI();
    void setupConnections();

    QLabel* nameLabel;

    QString libName;
    QString catName;
    QString compName;

    Library* lib;
    Catalog* cat;
    Component* comp;

    QComboBox* elementComboBox;
    QPushButton* okButton;
    QPushButton* cancelButton;

private slots:
    void onAccept();
    void onReject();
    void updateNameLabel();
};