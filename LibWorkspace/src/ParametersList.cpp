#include "ParametersList.h"
#include "readJson.h"

#include <QVector>

ParametersList::ParametersList(QWidget* parent) : QListWidget(parent)
{

}
ParametersList::~ParametersList()
{

}
void ParametersList::setItems()
{
    QDir dir(location);
    QStringList fileList = dir.entryList(QDir::Files);

    for (const QString& fileName : fileList) {
        QString filePath = dir.filePath(fileName);
        nlohmann::json parametersJson = readJson(filePath, this);

        for (auto& par : parametersJson["parameters"])
        {
            Parameters parametr;
            addParametrFromJson(par, parametr);

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
                    "%9"
                    "display: %10"
                    "%11"
                    "%12"
                    "%13")
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
                        ? "optimizable: " + (parametr.optimizable.value() ? QString("true") : QString("false")) + "\n" : "")
                    .arg(parametr.edited.has_value()
                        ? "edited: " + (parametr.edited.value() ? QString("true") : QString("false")) + "\n" : "")
                    .arg(parametr.netlisted.has_value()
                        ? "netlisted: " + (parametr.netlisted.value() ? QString("true") : QString("false")) + "\n" : "");

                item->setToolTip(tooltip);
                addItem(item);
            }
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

void ParametersList::addParametrFromJson(nlohmann::json jsonObj, Parameters& parametr)
{
    parametr.name = QString::fromStdString(jsonObj["name"].get<std::string>());
    parametr.type = QString::fromStdString(jsonObj["type"].get<std::string>());
    parametr.display = jsonObj["display"].get<bool>();

    if (parametr.type == "String" || (jsonObj.contains("default") && jsonObj["default"].is_string()) || parametr.type == "Equation") {
        if (jsonObj.contains("default")) {
            parametr.pdefault = QString::fromStdString(jsonObj["default"].get<std::string>());
        }
        else {
            parametr.pdefault = QString();
        }
    }
    else if (jsonObj.contains("default") && jsonObj["default"].is_array()) {
        //Для массива
        std::vector<double> arr = jsonObj["default"].get<std::vector<double>>();
        QVariantList qlist;
        for (double v : arr) {
            qlist << v;
        }
        parametr.pdefault = qlist;
    }
    else if (jsonObj.contains("default") && jsonObj["default"].is_number()) {
        parametr.pdefault = jsonObj["default"].get<double>();
    }
    else {
        parametr.pdefault = std::nullopt;
    }

    if (jsonObj.contains("ref")) {
        parametr.ref = QString::fromStdString(jsonObj["ref"].get<std::string>());
    }
    if (jsonObj.contains("factor")) {
        parametr.factor = QString::fromStdString(jsonObj["factor"].get<std::string>());
    }
    if (jsonObj.contains("feature")) {
        parametr.feature = QString::fromStdString(jsonObj["feature"].get<std::string>());
    }
    if (jsonObj.contains("unit")) {
        parametr.unit = QString::fromStdString(jsonObj["unit"].get<std::string>());
    }
    if (jsonObj.contains("desc")) {
        parametr.desc = QString::fromStdString(jsonObj["desc"].get<std::string>());
    }
    if (jsonObj.contains("optimizable")) {
        parametr.optimizable = jsonObj["optimizable"].get<bool>();
    }
    if (jsonObj.contains("edited")) {
        parametr.edited = jsonObj["edited"].get<bool>();
    }
    if (jsonObj.contains("netlisted")) {
        parametr.netlisted = jsonObj["netlisted"].get<bool>();
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