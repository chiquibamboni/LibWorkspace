#pragma once

#include <QString>
#include "Parameters.h"
#include <QList>
#include <QIcon>
#include <QMap>


struct Component
{
    QString name;
    QString model;
    bool view_model;
    QString library;
    QString group;
    QIcon thumb;
    QString thumbName;
    QString desc;
    QList<Parameters> parameters;
    QList<QString> pins;
    struct Schematic {
        struct Netlist {
            QString model;
            QMap<QString, QString> params;
        } netlist;
    } schematic;

    struct Layout {
        QString model;
    } layout;

    struct Ugo {
        QString model;
        QIcon ansiUgoSymbol;
        QIcon gostUgoSymbol;
    } ugo;

    struct Veriloga {
        QString model;
    } veriloga;
};