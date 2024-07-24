#ifndef CHINESECONVERTPINYIN_H
#define CHINESECONVERTPINYIN_H

#include "../controller/utils.h"
#include <unordered_map>

class ChineseConvertPinyin
{
public:
    // 删除拷贝构造函数和拷贝赋值操作符
    ChineseConvertPinyin(ChineseConvertPinyin const&) = delete;
    ChineseConvertPinyin& operator=(ChineseConvertPinyin const&) = delete;
    // 提供一个全局唯一的接口
    static ChineseConvertPinyin& getInstance() {
        static ChineseConvertPinyin instance;
        return instance;
    }

    void init();

    std::wstring getPinyin(const std::wstring& chineseName);

    bool isChineseCharacter(wchar_t c);

private:
    bool isAsciiChar(wchar_t c);

    ChineseConvertPinyin();
    std::unordered_map<wchar_t, std::wstring> converter;

    QJsonArray readJsonArrayFromFile(const QString& filePath);

};

#endif // CHINESECONVERTPINYIN_H
