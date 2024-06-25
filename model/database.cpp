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

double Database::computeCombinedValue(const std::wstring &storeName, const std::wstring &inputName)
{
    double editValue = editSubstrDistance(storeName, inputName);
    double kmpValue = kmp(storeName, inputName);
    double lcsValue = LCS(storeName, inputName);

    return editValue + kmpValue + lcsValue * 0.2;
}

void Database::insertProgramInfo(const std::wstring &programName, const std::wstring &programPath, int stableLevel)
{
    int t = programName.find_last_of(L".");
    std::wstring showName = programName.substr(0, t);

    std::wstring compareName = preprocess(showName);
    tolower(compareName);

    if (isExist(compareName))
        return ;

    // 将中文转为拼音
    ChineseConvertPinyin& converter = ChineseConvertPinyin::getInstance();
    std::wstring pinyin = converter.getPinyin(compareName);
    std::wstring firstLatterName = simplifiedPinyin(pinyin);

    programs.emplace_back(showName, compareName, pinyin, firstLatterName, programPath, 0, stableLevel, 0);
}

void Database::updateProgramInfo(const std::wstring &inputValue)
{
    for (auto & i : programs) {
        const std::wstring& compareName = i.compareName;
        const std::wstring& pinyinName = i.pinyinName;
        const std::wstring& firstLatterName = i.firstLatterName;
        // 如果输入的长度已经大于其本身的长度了，则一定不是该软件
        if (inputValue.size() > std::max(compareName.size(), pinyinName.size())) {
            i.programLevel = 50000;
            continue;
        }
        std::wstring compareKey = inputValue;
        tolower(compareKey);

        double directValue = computeCombinedValue(compareName, compareKey);
        double pinyinValue = computeCombinedValue(pinyinName, compareKey);
        double firstLatterValue = computeCombinedValue(firstLatterName, compareKey);

        i.programLevel = std::min({directValue, pinyinValue, firstLatterValue}) + i.stableLevel;

    }

    std::sort(programs.begin(), programs.end());
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

double Database::LCS(const std::wstring &compareName, const std::wstring &inputValue)
{
    int m = compareName.size();
    int n = inputValue.size();

    // 使用两个一维数组来代替二维数组
    std::vector<int> previous(n + 1, 0);
    std::vector<int> current(n + 1, 0);

    for (int i = 1; i <= m; ++i) {
        for (int j = 1; j <= n; ++j) {
            if (compareName[i - 1] == inputValue[j - 1]) {
                current[j] = previous[j - 1] + 1;
            } else {
                current[j] = std::max(previous[j], current[j - 1]);
            }
        }
        // 将当前行复制到前一行，以便下一次迭代使用
        std::swap(previous, current);
    }

    return (m + n - 2 * previous[n]);
}

double Database::LCS_MAX(const std::wstring &compareName, const std::wstring &compareKey)
{

    int res = 0;
    int n = compareName.size();
    int m = compareKey.size();
    std::vector<int> prev(m + 1, 0), curr(m + 1, 0);

    for (int i = 1; i <= n; ++i) {
        for (int j = 1; j <= m; ++j) {
            if (compareName[i - 1] == compareKey[j - 1]) {
                curr[j] = prev[j - 1] + 1;
            } else {
                curr[j] = 0;
            }
            if (curr[j] > res) {
                res = curr[j];
            }
        }
        std::swap(prev, curr);
        std::fill(curr.begin(), curr.end(), 0);
    }

    return res;
}


double Database::editSubstrDistance(const std::wstring& compareName, const std::wstring& inputValue) {
    int m = compareName.size();
    int n = inputValue.size();
    std::vector<std::vector<int>> dp(m + 1, std::vector<int>(n + 1, 0));

    // Initialize the dp array
    for (int i = 0; i <= m; ++i) {
        dp[i][0] = 0;  // No operation needed to match an empty substring
    }
    for (int j = 1; j <= n; ++j) {
        dp[0][j] = j;  // Need j operations to delete all characters in b
    }

    // Fill the dp array
    for (int i = 1; i <= m; ++i) {
        for (int j = 1; j <= n; ++j) {
            if (compareName[i - 1] == inputValue[j - 1]) {
                dp[i][j] = dp[i - 1][j - 1];
            }
            else {
                dp[i][j] = std::min({ dp[i - 1][j - 1] + 1,  // Replace
                                     dp[i][j - 1] + 1,      // Insert
                                     dp[i - 1][j] + 1 });    // Delete
            }
        }
    }

    // Find the minimum value in the last column
    double min_operations = dp[m][n];
    for (int i = n; i <= m; ++i) {
        min_operations = std::min(min_operations, (double)dp[i][n]);
    }

    min_operations /= inputValue.size();
    return pow(3, 3 * min_operations - 2) * 25;
}


double Database::editDistance(const std::wstring& compareName, const std::wstring& inputValue) {

    int n = compareName.size();
    int m = inputValue.size();

    // Ensure b is the shorter string to minimize space usage
    if (n < m) {
        return editDistance(inputValue, compareName);
    }

    std::vector<int> dp(m + 1);
    std::vector<int> temp(m + 1);

    for (int j = 0; j <= m; j++) {
        dp[j] = j;
    }

    for (int i = 1; i <= n; i++) {
        temp[0] = i;
        for (int j = 1; j <= m; j++) {
            if (compareName[i - 1] == inputValue[j - 1]) {
                temp[j] = dp[j - 1];
            } else {
                temp[j] = std::min({dp[j - 1] + 1, dp[j] + 1, temp[j - 1] + 1});
            }
        }
        dp.swap(temp);
    }

    return dp[m];
}

double Database::kmp(const std::wstring &compareName, const std::wstring &inputValue)
{
    int kmpValue = 0;
    auto it = compareName.find(inputValue);
    if (it != std::wstring::npos) {
        kmpValue += inputValue.size();
    } else {
        return 0;
    }

    if (compareName.starts_with(inputValue)) {
        kmpValue += inputValue.size();
    }
    return -kmpValue;
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
    for (auto& i : inputText) {
        if (i == L'(') {
            s ++;
        } else if (i == L')') {
            s --;
        } else if (s == 0){
            ret.push_back(i);
        }
    }
    return ret;
}

std::wstring Database::simplifiedPinyin(const std::wstring& input)
{
    std::wstring ret;
    bool inWord = false;
    for (const auto& i : input) {
        if (i != L' ') {
            if (!inWord)
                ret.push_back(i);
            inWord = true;
            continue;
        } else {
            inWord = false;
        }
    }
    return ret;
}




void Database::testCompareAlgorithm(std::wstring inputValue)
{

    QString header = QString("%1 | %2 | %3 | %4 | %5")
                         .arg(QString("名称").leftJustified(60), "总分", "编辑距离", "kmp值", "固定值");
    qDebug() << header;


    for (auto & i : programs) {
        const std::wstring& compareName = i.compareName;
        // 如果输入的长度已经大于其本身的长度了，则一定不是该软件
        if (inputValue.size() > compareName.size()) {
            i.programLevel = 50000;
            continue;
        }


        std::wstring compareKey = inputValue;
        tolower(compareKey);

        double editValue = editSubstrDistance(compareName, compareKey);
        double kmpValue = kmp(compareName, compareKey);

        i.programLevel = editValue + kmpValue + i.stableLevel;

        QString header = QString("%1 | %2 | %3 | %4 | %5")
                             .arg(QString::fromStdWString(i.programName).leftJustified(60),
                                  QString::fromStdWString(std::to_wstring(i.programLevel)),
                                  QString::fromStdWString(std::to_wstring(editValue)),
                                  QString::fromStdWString(std::to_wstring(kmpValue)),
                                  QString::fromStdWString(std::to_wstring(i.stableLevel)));
        qDebug() << header;


    }

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
