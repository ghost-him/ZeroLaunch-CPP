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
    const std::wstring& showName = programName;

    std::wstring filteredName = preprocess(showName);

    std::wstring uppercaseLetters = tolowerString(getUppercaseLetters(filteredName));

    std::wstring compareName = tolowerString(filteredName);
    if (isExist(compareName))
        return ;

    double stableBias = getStableBias(compareName) + stableLevel;

    // 将中文转为拼音
    ChineseConvertPinyin& converter = ChineseConvertPinyin::getInstance();
    std::wstring splitString = converter.getPinyin(compareName);

    if (splitString.empty()) {
        splitString = compareName;
    }

    std::wstring firstLatterName = extractInitials(splitString);
    std::wstring pinyin = removeStringSpace(splitString);


    std::vector<std::wstring> compareNames{
        compareName,
        pinyin,
        firstLatterName,
        uppercaseLetters,
    };

    size_t maxNameLength = std::max({compareName.size(), pinyin.size(), firstLatterName.size()});


    ProgramNode app {
        .showName = showName,
        .compareNames = compareNames,
        .programPath = programPath,
        .iconPath = iconPath,
        .compatibility = 0,
        .stableBias = stableBias,
        .launchTime = 0,
        .isUWPApp = isUWPApp,
        .maxNameLength = maxNameLength,
    };

    programs.emplace_back(app);
}

void Database::updateScores(const std::wstring& inputValue)
{
    //qDebug() << "update score name: " << inputName;
    std::wstring inputName = tolowerString(inputValue);
    for (auto& app : programs) {
        double compatibility = calculateCompatibility(app, inputName);
        compatibility += app.stableBias;
        app.compatibility = compatibility;
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


std::wstring Database::tolowerString(const std::wstring &other)
{
    std::wstring result;
    for (const auto& i : other) {
        result.push_back(std::towlower(i));
    }
    return result;
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
    return ret;
}

double Database::calculateStringCompatibility(const std::wstring& compareName, const std::wstring& inputName)
{
    double score = calculateEditDistance(compareName, inputName);
    score = score * calculateWeight((double) inputName.size() / compareName.size());
    score = score + calculateKMP(compareName, inputName);
    return score;
}

double Database::calculateWeight(double inputLen)
{
    return 3 * std::log2(inputLen + 1);
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

double Database::calculateCompatibility(const ProgramNode &node, const std::wstring &inputName)
{
    double result = -10000;
    for (auto& i : node.compareNames) {
        if (i.size() < inputName.size()) {
            continue;
        }
        double score = calculateStringCompatibility(i, inputName);
        result = std::max(result, score);
    }
    return result;
}

double Database::calculateKMP(const std::wstring &compareName, const std::wstring &inputValue)
{
    double ret { 0 };
    for (int i = 0; i < compareName.size() && i < inputValue.size(); i ++) {
        if (compareName[i] == inputValue[i]) {
            ret ++;
        } else {
            break;
        }
    }
    return ret;
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
        for (auto &j : i.compareNames) {
            str += j;
            str += "|";
        }
        qDebug() << str;

    }
}

double Database::getStableBias(const std::wstring &key)
{
    double result = 0;
    for (const auto& i : keyFilters) {
        if (key.find(i.first) != std::wstring::npos) {
            result += i.second;
        }
    }
    return result;
}

std::wstring Database::extractInitials(const std::wstring &input)
{
    std::wstring result;
    std::wistringstream iss(input);
    std::wstring word;

    while (iss >> word) {
        if (!word.empty()) {
            result += word[0];
        }
    }
    return result;
}

std::wstring Database::removeStringSpace(const std::wstring &str)
{
    std::wstring result;
    for (auto& i : str) {
        if (i != L' ') {
            result.push_back(i);
        }
    }
    return result;
}

std::wstring Database::getUppercaseLetters(const std::wstring &str)
{
    std::wstring result;
    for (const auto& i : str) {
        if (iswupper(i)) {
            result.push_back(i);
        }
    }
    return result;
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
    keyFilters.clear();
}

void Database::addKeyFilter(const std::wstring &key, double stableBias)
{
    keyFilters.push_back({key, stableBias});
}
