#include "ValueValidator.h"

bool ValueValidator::hasCyrillicCharacters(const QString& text)
{
    for (const QChar& ch : text) {
        ushort unicode = ch.unicode();
        // Русские буквы в Unicode: 
        // А-Я: 1040-1071, а-я: 1072-1103, Ё: 1025, ё: 1105
        if ((unicode >= 1040 && unicode <= 1103) ||
            unicode == 1025 || unicode == 1105) {
            return true;
        }
    }
    return false;
}