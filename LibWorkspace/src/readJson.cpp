#include "readJson.h"

nlohmann::json readJson(QString currentPath, QWidget* parent) {

    QFileInfo fileInfo(currentPath);

    if (!fileInfo.exists()) {
        QMessageBox::warning(parent, "Ошибка", QStringLiteral(u"ресурсы не найдены"));
        return nlohmann::json();
    }

    if (fileInfo.isDir()) {
        QDir librariesDir(currentPath);
        QStringList jsonFiles = librariesDir.entryList(QStringList() << "*.json", QDir::Files);

        if (!jsonFiles.isEmpty()) {
            QString jsonFilePath = librariesDir.filePath(jsonFiles.first());
            QFile jsonFile(jsonFilePath);

            if (jsonFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
                QByteArray jsonContent = jsonFile.readAll();
                nlohmann::json j = nlohmann::json::parse(jsonContent);
                jsonFile.close();
                return j;
            }
            else {
                QMessageBox::warning(parent, QStringLiteral(u"Ошибка при открытии json файла: "), jsonFilePath);
            }
        }
        else {
            QMessageBox::warning(parent, "Ошибка", QStringLiteral(u"json файл не найден"));
        }
    }
    else
    {
        QFile jsonFile(currentPath);

        if (jsonFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QByteArray jsonContent = jsonFile.readAll();
            nlohmann::json j = nlohmann::json::parse(jsonContent);
            jsonFile.close();
            return j;
        }
        else {
            QMessageBox::warning(parent, QStringLiteral(u"Ошибка при открытии json файла: "), fileInfo.fileName());
        }
    }

    return nlohmann::json();
}