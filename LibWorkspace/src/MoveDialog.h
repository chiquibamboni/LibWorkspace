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

class MoveDialog : public QDialog
{
    Q_OBJECT

public:
    MoveDialog(Library* currentLibrary, Catalog* currentCatalog, Component* currentComponent, QString direction, QWidget* parent = nullptr);
    ~MoveDialog();

private:
    void setupUI(QString direction);
    void setupConnections();
    Catalog* findNeighborCatalog(Library* lib, Catalog* currentCatalog, QString direction);

    Library* lib;
    Catalog* cat;
    Catalog* nextCat;
    Component* comp;

    QPushButton* okButton;
    QPushButton* moveButton;
    QPushButton* cancelButton;

private slots:
    void onAccept();
    void onReject();
};