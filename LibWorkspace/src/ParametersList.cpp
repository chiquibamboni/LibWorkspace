#include "ParametersList.h"
#include "readJson.h"

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
    QDir dir(location);
    QStringList fileList = dir.entryList(QDir::Files);
    for (const QString& fileName : fileList) {
        QString filePath = dir.filePath(fileName);
        nlohmann::json parametersJson = readJson(filePath, this);
        for (auto& par : parametersJson["parameters"]) 
        {
            Parameters parametr;
            addParametrFromJson(par, parametr);
            QListWidgetItem* item = new QListWidgetItem(parametr.name);
            QString tooltip = QString("name: %1\n"
                "type: %2\n"
                "%3"
                "%4"
                "%5"
                "%6"
                "display: %7")
                .arg(parametr.name)
                .arg(parametr.type)
                .arg(parametr.sdefault.isEmpty() ? "" : "default: " + parametr.sdefault + "\n")
                .arg(parametr.hasRdefault == false ? "" : "default: " + QString::number(parametr.rdefault) + "\n")
                .arg(parametr.factor.isEmpty() ? "" : "factor: " + parametr.factor + "\n")
                .arg(parametr.unit.isEmpty() ? "" : "unit: " + parametr.unit + "\n")
                .arg(parametr.display);

            item->setToolTip(tooltip);
            addItem(item);
        }
    }

}

void ParametersList::addParametrFromJson(nlohmann::json jsonObj, Parameters& parametr)
{
    parametr.name = QString::fromStdString(jsonObj["name"].get<std::string>());
    parametr.type = QString::fromStdString(jsonObj["type"].get<std::string>());

    if (jsonObj["type"] == "String" || jsonObj["default"] == "" || jsonObj["type"] == "Equation")
    {
        parametr.sdefault = QString::fromStdString(jsonObj["default"].get<std::string>());
    }
    else
    {
        parametr.rdefault = jsonObj["default"].get<double>();
        parametr.hasRdefault = true;
    }
    parametr.display = jsonObj["display"].get<bool>();
    if (jsonObj.contains("factor")) {
        parametr.factor = QString::fromStdString(jsonObj["factor"].get<std::string>());
    }
    if (jsonObj.contains("unit")) {
        parametr.unit = QString::fromStdString(jsonObj["unit"].get<std::string>());
    }
    parameters->push_back(parametr);
}

int ParametersList::getCurrentParametrIndex() const
{
    return 0;
}