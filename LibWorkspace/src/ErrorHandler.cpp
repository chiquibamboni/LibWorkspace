#include "ErrorHandler.h"
#include <QApplication>
#include <QTimer>

void ErrorHandler::showWarning(QStatusBar* statusBar, const QString& message)
{
    // Отображаем сообщение в статус баре
    if (statusBar) {
        statusBar->showMessage(message, 5000); // Показываем 5 секунд
    }
    
    // Также показываем всплывающее окно
    QMessageBox::warning(nullptr, QStringLiteral(u"Предупреждение"), message);
}

void ErrorHandler::showError(QStatusBar* statusBar, const QString& message)
{
    // Отображаем сообщение в статус баре
    if (statusBar) {
        statusBar->showMessage(message, 10000); // Показываем 10 секунд
    }
    
    // Также показываем всплывающее окно
    QMessageBox::critical(nullptr, QStringLiteral(u"Ошибка"), message);
}