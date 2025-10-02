#pragma once

#include <QList>
#include <QListWidget>
#include <QStandardItemModel>

#include <nlohmann/json.hpp>

#include "Parameters.h"

class ParametersList : public QListWidget
{
	Q_OBJECT

public:
	ParametersList(QList<Parameters>* parameters, QWidget* parent = nullptr);
	~ParametersList();

	QString location;

	void setItems();
	void clearItems();

private:
	QList<Parameters>* parametersList;

	QString  pdefaultToString(const std::optional<QVariant>& pdefault);
	void addParametrFromJson(nlohmann::json jsonObj, Parameters& parametr);
	bool containsParam(const Parameters& param) const;

};