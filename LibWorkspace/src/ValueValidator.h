#pragma once

#include <QString>

class ValueValidator {
public:
	static bool hasCyrillicCharacters(const QString& text);
};