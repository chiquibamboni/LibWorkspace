#pragma once

#include <QString>
#include <QDir>
#include <QMessageBox>
#include <nlohmann/json.hpp>

nlohmann::json readJson(QString currentPath, QWidget* parent);