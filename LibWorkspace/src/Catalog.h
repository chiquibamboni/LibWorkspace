#pragma once

#include "Component.h"
#include <QString>

struct Catalog
{
	QStandardItem* item;
	Catalog* parentCatalog = nullptr;
	QString name;
	QString thumb;
	QList<Catalog> catalogs;
	QList<Component> components;
};

