#pragma once

#include <QStatusBar>
#include <QMessageBox>
#include <QString>

class ErrorHandler
{
public:
    static void showWarning(QStatusBar* statusBar, const QString& message);
    static void showError(QStatusBar* statusBar, const QString& message);
};