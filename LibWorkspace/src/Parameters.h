#pragma once

#include <QString>

struct Parameters
{
    QString name;
    QString type;
    QString sdefault;
    double rdefault;
    bool hasRdefault = false;
    QString factor;
    QString unit;
    bool display;
};
