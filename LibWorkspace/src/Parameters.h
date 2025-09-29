#pragma once

#include <iostream> 
#include <QString>
#include <optional>

struct Parameters
{
    QString ref;
    QString name;
    QString type;
    QString sdefault;
    std::optional<double> rdefault;
    QString factor;
    QString feature;
    QString unit;
    QString desc;
    bool display;
    std::optional<bool> optimizable;
    std::optional<bool> edited;
    std::optional<bool> netlisted;
};
