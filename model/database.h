#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <vector>
#include <unordered_set>

struct ProgramNode {
    std::wstring programName;

    std::wstring compareName;
    std::wstring pinyinName;
    std::wstring firstLatterName;

    std::wstring programPath;
    double programLevel;
    int stableLevel;
    int launchTime;

    const bool operator<(const ProgramNode& other) const {
        if (programLevel != other.programLevel) {
            return programLevel < other.programLevel;
        } else if (launchTime != other.launchTime) {
            return launchTime > other.launchTime;
        } else {
            return compareName < other.compareName;
        }

    }

};

class Database
{

public:
    // 删除拷贝构造函数和拷贝赋值操作符
    Database(Database const&) = delete;
    Database& operator=(Database const&) = delete;
    // 提供一个全局唯一的接口
    static Database& getInstance() {
        static Database instance;
        return instance;
    }

    void insertProgramInfo(const std::wstring& programName, const std::wstring& programPath, int stableLevel);

    void updateProgramInfo(const std::wstring& inputValue);

    const std::vector<ProgramNode>& getProgramsFile();

    bool isExist(const std::wstring& key);

    void testCompareAlgorithm(std::wstring inputValue);

    void addLaunchTime(int index);

    void clearProgramInfo();

private:
    Database();
    std::vector<ProgramNode> programs;
    std::unordered_set<std::wstring> cache;

    double computeCombinedValue(const std::wstring& storeName, const std::wstring& inputName);

    double LCS(const std::wstring& compareName, const std::wstring& inputValue);
    double LCS_MAX(const std::wstring& compareName, const std::wstring& inputValue);
    double editSubstrDistance(const std::wstring& compareName, const std::wstring& inputValue);
    double editDistance(const std::wstring& compareName, const std::wstring& inputValue);
    double kmp(const std::wstring& compareName, const std::wstring& inputValue);

    std::wstring& tolower(std::wstring& other);
    std::wstring preprocess(const std::wstring& inputText);

    std::wstring simplifiedPinyin(const std::wstring& input);

};

#endif // DATABASE_H
