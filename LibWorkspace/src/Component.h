#pragma once

#include <QString>
#include "Parameters.h"
#include <QList>


struct Component
{
    QString model;
    QString group;
    QIcon thumb;
    QString desc;
    QList<Parameters> parameters;
};