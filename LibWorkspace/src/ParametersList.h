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
	ParametersList(QWidget* parent = nullptr);
	~ParametersList();

	QString location;
	QList<Parameters>* parameters;

	void setItems();
	void clearItems();
private:
	void addParametrFromJson(nlohmann::json jsonObj, Parameters& parametr);

};