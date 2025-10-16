#include "ParametersList.h"
#include "FillFromJsons.h"

#include <QVector>
#include <QMessageBox>
#include <QDebug>

ParametersList::ParametersList(QWidget* parent) : QListWidget(parent)
{
    parameters = new QList<Parameters>();
}
ParametersList::~ParametersList()
{
    delete parameters;
}
void ParametersList::setItems()
{
    // Проверяем, что путь задан
    if (location.isEmpty()) {
        QString message = QStringLiteral(u"Не задан путь к файлам параметров");
        FillFromJsons::showError(this, message);
        return;
    }

    QDir dir(location);
    if (!dir.exists()) {
        QString message = QString(QStringLiteral(u"Путь к файлам параметров не существует: %1")).arg(location);
        FillFromJsons::showError(this, message);
        return;
    }

    QStringList fileList = dir.entryList(QDir::Files);

    for (const QString& fileName : fileList) {
        QString filePath = dir.filePath(fileName);
        nlohmann::json parametersJson = FillFromJsons::readJson(filePath, this);
        
        // Проверяем, что JSON успешно прочитан и содержит поле "parameters"
        if (parametersJson.is_null()) {
            continue; // Файл не удалось прочитать, переходим к следующему
        }
        
        if (!parametersJson.contains("parameters")) {
            QString message = QString(QStringLiteral(u"Файл %1 не содержит поле 'parameters'")).arg(fileName);
            FillFromJsons::showError(this, message);
            continue;
        }

        if (!parametersJson["parameters"].is_array()) {
            QString message = QString(QStringLiteral(u"Поле 'parameters' в файле %1 не является массивом")).arg(fileName);
            FillFromJsons::showError(this, message);
            continue;
        }

        for (auto& par : parametersJson["parameters"])
        {
            Parameters parametr;
            FillFromJsons::addParametrFromJson(par, parametr);
            insertItem(parametr);
        }
    }
}

QString ParametersList::pdefaultToString(const std::optional<QVariant>& pdefault) {
    if (!pdefault.has_value())
        return "";

    const QVariant& val = *pdefault;
    if (val.type() == QVariant::List || val.canConvert<QVariantList>()) {
        QVariantList list = val.toList();
        QStringList items;
        for (const QVariant& item : list) {
            items << item.toString();
        }
        return "default: [" + items.join(", ") + "]\n";
    }
    else {
        return "default: " + val.toString() + "\n";
    }
}

//Проверка на дубликаты только по имени
bool ParametersList::containsParam(const Parameters& param) const
{
    for (const auto& p : *parameters) {
        if (p.name == param.name) {
            return true;
        }
    }
    return false;
}

//Проверка на дубликаты по всем полям
//bool ParametersList::containsParam(const Parameters& param) const
//{
//    for (const auto& p : *parameters) {
//        if (p.name == param.name &&
//            p.type == param.type &&
//            p.display == param.display &&
//            p.sdefault == param.sdefault &&
//            p.rdefault == param.rdefault &&
//            p.ref == param.ref &&
//            p.factor == param.factor &&
//            p.feature == param.feature &&
//            p.unit == param.unit &&
//            p.desc == param.desc &&
//            p.optimizable == param.optimizable &&
//            p.edited == param.edited &&
//            p.netlisted == param.netlisted
//            ) {
//            return true;
//        }
//    }
//    return false;
//}

void ParametersList::clearItems()
{
    this->clear();
    parameters->clear();
}

void ParametersList::insertItem(Parameters parametr)
{
    if (!containsParam(parametr)) {
        parameters->push_back(parametr);
        QListWidgetItem* item = new QListWidgetItem(parametr.name);
        QString tooltip = QString("%1"
            "name: %2\n"
            "type: %3\n"
            "%4"
            "%5"
            "%6"
            "%7"
            "%8"
            "display: %9"
            "%10"
            "%11"
            "%12")
            .arg(parametr.ref.isEmpty() ? "" : "ref: " + parametr.ref + "\n")
            .arg(parametr.name)
            .arg(parametr.type)
            .arg(pdefaultToString(parametr.pdefault))
            .arg(parametr.factor.isEmpty() ? "" : "factor: " + parametr.factor + "\n")
            .arg(parametr.feature.isEmpty() ? "" : "feature: " + parametr.feature + "\n")
            .arg(parametr.unit.isEmpty() ? "" : "unit: " + parametr.unit + "\n")
            .arg(parametr.desc.isEmpty() ? "" : "desc: " + parametr.desc + "\n")
            .arg(parametr.display)
            .arg(parametr.optimizable.has_value()
                ? "\noptimizable: " + (parametr.optimizable.value() ? QString("true") : QString("false")) : "")
            .arg(parametr.edited.has_value()
                ? "\nedited: " + (parametr.edited.value() ? QString("true") : QString("false")) : "")
            .arg(parametr.netlisted.has_value()
                ? "\nnetlisted: " + (parametr.netlisted.value() ? QString("true") : QString("false")) : "");

        item->setToolTip(tooltip);
        addItem(item);
    }
}
