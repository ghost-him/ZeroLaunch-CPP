#include "database.h"
#include <algorithm>
#include "chineseconvertpinyin.h"
#include <cmath>
#include <cwctype>
#include <QDebug>
Database::Database() {}
// 防止使用windows自己的min宏定义
#undef min
#undef max

void Database::insertProgramInfo(const std::wstring &programName, const std::wstring &programPath, const std::wstring& iconPath, int stableLevel, bool isUWPApp)
{
    int t = programName.find_last_of(L".");
    std::wstring showName = programName.substr(0, t);

    std::wstring compareName = preprocess(showName);
    if (isExist(compareName))
        return ;

    // 检测是否是有效的名字
    if (!isValidName(compareName)) {
        // qDebug() << compareName << "++++++++++++++++++++++++++++++++++++++++++++";
        return ;
    }

    // 将中文转为拼音
    ChineseConvertPinyin& converter = ChineseConvertPinyin::getInstance();
    std::wstring pinyin = converter.getPinyin(compareName);

    if (pinyin.empty()) {
        pinyin = compareName;
    }

    std::vector<std::wstring> nameParts = splitStringBySpace(pinyin);
    int pinyinLength = 0;
    for (auto &i : nameParts) {
        pinyinLength += i.size();
    }

    ProgramNode app {
        .showName = showName,
        .compareName = compareName,
        .nameParts = nameParts,
        .programPath = programPath,
        .iconPath = iconPath,
        .compatibility = 0,
        .stableBias = stableLevel,
        .launchTime = 0,
        .isUWPApp = isUWPApp,
        .pinyinLength = pinyinLength,
    };

    programs.emplace_back(app);
}

void Database::updateScores(const std::wstring &inputName)
{
    //qDebug() << "update score name: " << inputName;

    auto splits = splitString(inputName);
    for (auto& app : programs) {
        app.compatibility = -10000;
        if (inputName.size() > std::max(app.pinyinLength, (int)app.compareName.size())) {
            continue;
        }
        // 计算分割形式的匹配度
        calculateNameParts(app, splits);
        // 计算整体形式的匹配度
        calculateCompareName(app, inputName);
        app.compatibility += app.stableBias;
    }


    std::sort(programs.begin(), programs.end());
    std::reverse(programs.begin(), programs.end());
    // debugProgramNode();

}

const std::vector<ProgramNode>& Database::getProgramsFile()
{
    return programs;
}

bool Database::isExist(const std::wstring &key)
{
    if (cache.contains(key)) return true;
    cache.insert(key);
    return false;
}


std::wstring& Database::tolower(std::wstring &other)
{
    for (auto& i : other) {
        if (std::iswupper(i)) {
            i = std::towlower(i);
        }
    }
    return other;
}

std::wstring Database::preprocess(const std::wstring &inputText)
{
    std::wstring ret;
    int s = 0;
    bool inVersion = false;

    for (size_t i = 0; i < inputText.size(); ++i) {
        wchar_t ch = inputText[i];

        // Handle parentheses
        if (ch == L'(') {
            s++;
            inVersion = true;  // Assume version info starts after '('
        }
        else if (ch == L')') {
            s--;
            inVersion = false;  // Assume version info ends at ')'
        }
        else if (s == 0 && !inVersion) {
            // Check if the current character is part of a version number
            if (std::iswdigit(ch) || ch == L'.') {
                // Check if the previous character is a space (to identify version numbers correctly)
                if (i > 0 && inputText[i - 1] == L' ') {
                    // Skip the entire version number
                    while (i < inputText.size() && (std::iswdigit(inputText[i]) || inputText[i] == L'.')) {
                        i++;
                    }
                    // Skip any trailing spaces
                    while (i < inputText.size() && inputText[i] == L' ') {
                        i++;
                    }
                    // Decrement i to counter the next iteration's increment
                    i--;
                    continue;
                }
            }
            ret.push_back(ch);
        }
    }

    // 去除结尾的空格
    while(ret.size() && (*ret.rbegin()) == L' ') {
        ret.pop_back();
    }

    tolower(ret);
    return ret;
}

std::vector<std::wstring> Database::splitStringBySpace(const std::wstring &str)
{
    std::vector<std::wstring> result;
    std::wistringstream iss(str);
    std::wstring word;
    while (iss >> word) result.push_back(word);
    return result;
}

std::vector<std::vector<std::wstring> > Database::splitString(const std::wstring &s)
{
    std::vector<std::vector<std::wstring>> result;
    int maxSplits = std::min(5, static_cast<int>(std::sqrt(s.length())));

    // 1. 按空格分割
    result.push_back(splitStringBySpace(s));

    // 2. 按大小写分割
    result.push_back(splitStringByCamelCase(s));

    // 3. 滑动窗口分割
    for (int windowSize = 1; windowSize <= maxSplits; ++windowSize) {
        std::vector<std::wstring> split;
        for (int i = 0; i < s.length(); i += windowSize) {
            split.push_back(s.substr(i, windowSize));
        }
        result.push_back(split);
    }

    // 4. 从左到右逐步分割
    std::vector<std::wstring> leftToRight;
    for (int i = 1; i <= s.length(); ++i) {
        leftToRight.push_back(s.substr(0, i));
        if (leftToRight.size() >= maxSplits) break;
    }
    result.push_back(leftToRight);

    // 5. 从右到左逐步分割
    std::vector<std::wstring> rightToLeft;
    for (int i = s.length(); i > 0; --i) {
        rightToLeft.insert(rightToLeft.begin(), s.substr(i - 1));
        if (rightToLeft.size() >= maxSplits) break;
    }
    result.push_back(rightToLeft);

    return result;
}

std::vector<std::wstring> Database::splitStringByCamelCase(const std::wstring &s)
{
    std::vector<std::wstring> result;
    std::wstring current;
    for (wchar_t c : s) {
        if (std::isupper(c) && !current.empty()) {
            result.push_back(current);
            current.clear();
        }
        current += c;
    }
    if (!current.empty()) {
        result.push_back(current);
    }
    return result;
}

void Database::calculateCompareName(ProgramNode &app, std::wstring inputName)
{
    double score = calculateEditDistance(app.compareName, inputName);
    score = score * calculateWeight((double) inputName.size() / app.compareName.size());
    app.compatibility = std::max(app.compatibility, score);
}

void Database::calculateNameParts(ProgramNode &app, const std::vector<std::vector<std::wstring> > &splits)
{

    // 分割形式的匹配度计算
    for (const auto& split : splits) {
        double score = 0.0;
        int n = std::min(split.size(), app.nameParts.size());
        for (int i = 0; i < n; ++i) {
            score += calculateScore(split[i], app.nameParts[i]);
        }
        score *= calculatePenalty(app.nameParts.size());
        app.compatibility = std::max(score, app.compatibility);
    }
}

double Database::calculateScore(const std::wstring &inputPart, const std::wstring &targetPart)
{
    double k = calculateWeight(inputPart.size());
    if (inputPart == targetPart) return 3 * k;
    if (targetPart.starts_with(inputPart)) return 2 * k;
    if (targetPart.find(inputPart) != std::wstring::npos) return k;
    return calculateEditDistance(targetPart, inputPart);
}

double Database::calculateWeight(double inputLen)
{
    return 3 * std::log2(inputLen + 1);
}

double Database::calculatePenalty(double x)
{
    return 1 / (x + std::log2(2 * (x + 1))) + 0.5;
}

double Database::calculateEditDistance(const std::wstring &compareName, const std::wstring &inputValue)
{
    int m = compareName.size();
    int n = inputValue.size();
    std::vector<std::vector<int>> dp(m + 1, std::vector<int>(n + 1, 0));

    for (int i = 0; i <= m; ++i) {
        dp[i][0] = 0;
    }
    for (int j = 1; j <= n; ++j) {
        dp[0][j] = j;
    }

    // Fill the dp array
    for (int i = 1; i <= m; ++i) {
        for (int j = 1; j <= n; ++j) {
            if (compareName[i - 1] == inputValue[j - 1]) {
                dp[i][j] = dp[i - 1][j - 1];
            }
            else {
                dp[i][j] = std::min({ dp[i - 1][j - 1] + 1,
                                     dp[i - 1][j] + 1 });
            }
        }
    }

    double min_operations = dp[m][n];
    for (int i = n; i <= m; ++i) {
        min_operations = std::min(min_operations, (double)dp[i][n]);
    }

    double value = 1 - min_operations / inputValue.size();
    return calculateWeight(inputValue.size()) * std::exp(3 * value - 2);
}

void Database::debugProgramNode()
{
    qDebug() << R"(
########################################################################
########################################################################)";
    for (auto & i : programs) {
        qDebug() << "------------------------------------------------------";
        qDebug() << i.showName << " " << i.compatibility;
        QString str;
        for (auto &j : i.nameParts) {
            str += j;
            str += " ";
        }
        qDebug() << str;

    }
}

bool Database::isValidName(const std::wstring &s)
{
    for (const auto& i : forbiddenNames) {
        if (s.find(i) != std::wstring::npos) {
            return false;
        }
    }
    return true;
}


void Database::testCompareAlgorithm(std::wstring inputValue)
{

    QString header = QString("%1 | %2 | %3 | %4 | %5")
                         .arg(QString("名称").leftJustified(60), "总分", "编辑距离", "kmp值", "固定值");
    qDebug() << header;
    updateScores(inputValue);

    debugProgramNode();

}

void Database::addLaunchTime(int index)
{
    programs[index].launchTime ++;
}

void Database::clearProgramInfo()
{
    programs.clear();
    cache.clear();
}

void Database::addForbiddenName(const std::wstring &name)
{
    forbiddenNames.push_back(name);
}
