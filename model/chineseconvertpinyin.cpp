#include "chineseconvertpinyin.h"
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QChar>

ChineseConvertPinyin::ChineseConvertPinyin() {}

QJsonArray ChineseConvertPinyin::readJsonArrayFromFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Could not open file" << filePath << "for reading.";
        return QJsonArray();
    }

    QByteArray fileData = file.readAll();
    file.close();

    QJsonParseError jsonError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(fileData, &jsonError);

    if (jsonError.error != QJsonParseError::NoError) {
        qWarning() << "JSON parse error:" << jsonError.errorString();
        return QJsonArray();
    }
    return jsonDoc.array();
}

void ChineseConvertPinyin::init()
{
    QString path = getPinyinConfigPath();
    QJsonArray array = readJsonArrayFromFile(path);

    for (const auto& i : array) {
        QJsonObject obj = i.toObject();
        converter[obj["word"].toString().toStdWString()[0]] = obj["pinyin"].toString().toStdWString();
    }
}

std::wstring ChineseConvertPinyin::getPinyin(const std::wstring &chineseName)
{
    std::wstring ret;
    for (const auto& i : chineseName) {
        // 只有当是中文或英文时，才进行转换
        if (converter.contains(i)) {
            ret.append(converter[i]);
                ret.push_back(L' ');
        } else {
            if (isAsciiChar(i)) {
                ret.push_back(i);
            }
        }
    }
    while((*ret.rbegin()) == L' ') ret.pop_back();
    return ret;
}

bool ChineseConvertPinyin::isAsciiChar(wchar_t c)
{
    return c <= 0x7F;
}
