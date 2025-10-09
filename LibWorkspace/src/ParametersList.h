﻿#pragma once

#include <QList>
#include <QListWidget>
#include <QStandardItemModel>

#include <nlohmann/json.hpp>

#include "Parameters.h"
#include "FillFromJsons.h"

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
	void insertItem(Parameters parametr);

private:

	QString  pdefaultToString(const std::optional<QVariant>& pdefault);
	bool containsParam(const Parameters& param) const;
};